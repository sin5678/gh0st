/////////////////////////////////////////////////////////////////////////////
// TrayIcon.cpp : implementation file
//
// This is a conglomeration of ideas from the MSJ "Webster" application,
// sniffing round the online docs, and from other implementations such
// as PJ Naughter's "CTrayIconifyIcon" (http://indigo.ie/~pjn/ntray.html)
// especially the "CTrayIcon::OnTrayNotification" member function.
//
// This class is a light wrapper around the windows system tray stuff. It
// adds an icon to the system tray with the specified ToolTip text and 
// callback notification value, which is sent back to the Parent window.
//
// The tray icon can be instantiated using either the constructor or by
// declaring the object and creating (and displaying) it later on in the
// program. eg.
//
//		CTrayIcon m_TrayIcon;	// Member variable of some class
//		
//		... 
//		// in some member function maybe...
//		m_TrayIcon.Create(pParentWnd, WM_MY_NOTIFY, "Click here", 
//						  hIcon, nTrayIconID);
//
// Clobbered together by Chris Maunder.
// 
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TrayIcon.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CTrayIcon, CObject)

/////////////////////////////////////////////////////////////////////////////
// CTrayIcon construction/creation/destruction

CTrayIcon::CTrayIcon()
{
	memset(&m_tnd, 0, sizeof(m_tnd));
	m_bEnabled = FALSE;
	m_bHidden  = FALSE;
}

CTrayIcon::CTrayIcon(CWnd* pWnd, UINT uCallbackMessage, LPCTSTR szToolTip, 
					 HICON icon, UINT uID)
{
	Create(pWnd, uCallbackMessage, szToolTip, icon, uID);
	m_bHidden = FALSE;
}

BOOL CTrayIcon::Create(CWnd* pWnd, UINT uCallbackMessage, LPCTSTR szToolTip, 
					   HICON icon, UINT uID, BOOL bIsNotify)
{
	// if bIsNotify == TRUE
	// Add these code to stdafx.h
	/*
	#ifndef _WIN32_IE // 允许使用 IE 4.0 或更高版本的特定功能。
	#define _WIN32_IE 0x0500 //为 IE 5.0 及更新版本改变为适当的值。
	#endif
	*/
	// this is only for Windows 95 (or higher)
	VERIFY(m_bEnabled = ( GetVersion() & 0xff ) >= 4);
	if (!m_bEnabled) return FALSE;

	//Make sure Notification window is valid
	VERIFY(m_bEnabled = (pWnd && ::IsWindow(pWnd->GetSafeHwnd())));
	if (!m_bEnabled) return FALSE;
	
	//Make sure we avoid conflict with other messages
	ASSERT(uCallbackMessage >= WM_USER);

	//Tray only supports tooltip text up to 64 characters
	ASSERT(_tcslen(szToolTip) <= 64);

	// load up the NOTIFYICONDATA structure
	m_bNotify = bIsNotify;

	m_tnd.cbSize = sizeof(NOTIFYICONDATA);
	m_tnd.hWnd	 = pWnd->GetSafeHwnd();
	m_tnd.uID	 = uID;
	m_tnd.hIcon  = icon;
	m_tnd.uCallbackMessage = uCallbackMessage;

	if (m_bNotify)
	{
		m_tnd.uTimeout = 1;
		m_tnd.dwInfoFlags= NIIF_INFO;
		m_tnd.uFlags = NIF_MESSAGE | NIF_INFO | NIF_ICON;
		CString WindowTitle;
		pWnd->GetWindowText(WindowTitle);
		strcpy (m_tnd.szInfo, szToolTip);
		strcpy (m_tnd.szInfoTitle, WindowTitle);
	}
	else
	{
		m_tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
		strcpy (m_tnd.szTip, szToolTip);
	}
	// Set the tray icon
	VERIFY(m_bEnabled = Shell_NotifyIcon(NIM_ADD, &m_tnd));



// 	m_tnd.cbSize = sizeof(NOTIFYICONDATA);
// 	m_tnd.hWnd	 = pWnd->GetSafeHwnd();
// 	m_tnd.uID	 = uID;
// 	m_tnd.hIcon  = icon;
// 	m_tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
// 	m_tnd.uCallbackMessage = uCallbackMessage;
// 	strcpy (m_tnd.szTip, szToolTip);
// 	VERIFY(m_bEnabled = Shell_NotifyIcon(NIM_ADD, &m_tnd));

	return m_bEnabled;
}

CTrayIcon::~CTrayIcon()
{
	RemoveIcon();
}


/////////////////////////////////////////////////////////////////////////////
// CTrayIcon icon manipulation

void CTrayIcon::MoveToRight()
{
	HideIcon();
	ShowIcon();
}

void CTrayIcon::RemoveIcon()
{
	if (!m_bEnabled) return;

	m_tnd.uFlags = 0;
    Shell_NotifyIcon(NIM_DELETE, &m_tnd);
    m_bEnabled = FALSE;
}

void CTrayIcon::HideIcon()
{
	if (m_bEnabled && !m_bHidden) {
		m_tnd.uFlags = NIF_ICON;
		Shell_NotifyIcon (NIM_DELETE, &m_tnd);
		m_bHidden = TRUE;
	}
}

void CTrayIcon::ShowIcon()
{
	if (m_bEnabled && m_bHidden) {
		m_tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
		Shell_NotifyIcon(NIM_ADD, &m_tnd);
		m_bHidden = FALSE;
	}
}

BOOL CTrayIcon::SetIcon(HICON hIcon)
{
	if (!m_bEnabled) return FALSE;

	m_tnd.uFlags = NIF_ICON;
	m_tnd.hIcon = hIcon;

	return Shell_NotifyIcon(NIM_MODIFY, &m_tnd);
}

BOOL CTrayIcon::SetIcon(LPCTSTR lpszIconName)
{
	HICON hIcon = AfxGetApp()->LoadIcon(lpszIconName);

	return SetIcon(hIcon);
}

BOOL CTrayIcon::SetIcon(UINT nIDResource)
{
	HICON hIcon = AfxGetApp()->LoadIcon(nIDResource);

	return SetIcon(hIcon);
}

BOOL CTrayIcon::SetStandardIcon(LPCTSTR lpIconName)
{
	HICON hIcon = LoadIcon(NULL, lpIconName);

	return SetIcon(hIcon);
}

BOOL CTrayIcon::SetStandardIcon(UINT nIDResource)
{
	HICON hIcon = LoadIcon(NULL, MAKEINTRESOURCE(nIDResource));

	return SetIcon(hIcon);
}

HICON CTrayIcon::GetIcon() const
{
	HICON hIcon = NULL;
	if (m_bEnabled)
		hIcon = m_tnd.hIcon;

	return hIcon;
}

/////////////////////////////////////////////////////////////////////////////
// CTrayIcon tooltip text manipulation

BOOL CTrayIcon::SetTooltipText(LPCTSTR pszTip)
{
	if (!m_bEnabled) return FALSE;
	
	if (m_bNotify)
	{
		_tcscpy(m_tnd.szInfo, pszTip);
		m_tnd.uTimeout = 1000;
		m_tnd.uVersion = NOTIFYICON_VERSION;
	}
	else
	{
		m_tnd.uFlags = NIF_TIP;
		_tcscpy(m_tnd.szTip, pszTip);
	}
	

	return Shell_NotifyIcon(NIM_MODIFY, &m_tnd);
}

BOOL CTrayIcon::SetTooltipText(UINT nID)
{
	CString strText;
	VERIFY(strText.LoadString(nID));

	return SetTooltipText(strText);
}

CString CTrayIcon::GetTooltipText() const
{
	CString strText;
	if (m_bEnabled)
		strText = m_tnd.szTip;

	return strText;
}

/////////////////////////////////////////////////////////////////////////////
// CTrayIcon notification window stuff

BOOL CTrayIcon::SetNotificationWnd(CWnd* pWnd)
{
	if (!m_bEnabled) return FALSE;

	//Make sure Notification window is valid
	ASSERT(pWnd && ::IsWindow(pWnd->GetSafeHwnd()));

	m_tnd.hWnd = pWnd->GetSafeHwnd();
	m_tnd.uFlags = 0;

	return Shell_NotifyIcon(NIM_MODIFY, &m_tnd);
}

CWnd* CTrayIcon::GetNotificationWnd() const
{
	return CWnd::FromHandle(m_tnd.hWnd);
}

/////////////////////////////////////////////////////////////////////////////
// CTrayIcon implentation of OnTrayNotification

LRESULT CTrayIcon::OnTrayNotification(UINT wParam, LONG lParam) 
{
	//Return quickly if its not for this tray icon
	if (wParam != m_tnd.uID)
		return 0L;

	CMenu menu, *pSubMenu;

	// Clicking with right button brings up a context menu
	if (LOWORD(lParam) == WM_RBUTTONUP)
	{	
		if (!menu.LoadMenu(m_tnd.uID)) return 0;
		if (!(pSubMenu = menu.GetSubMenu(0))) return 0;

		// Make first menu item the default (bold font)
		::SetMenuDefaultItem(pSubMenu->m_hMenu, 0, TRUE);

		//Display and track the popup menu
		CPoint pos;
		GetCursorPos(&pos);
		::SetForegroundWindow(m_tnd.hWnd);  
		::TrackPopupMenu(pSubMenu->m_hMenu, 0, pos.x, pos.y, 0, m_tnd.hWnd, NULL);

		//pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pos.x, pos.y, this, NULL);
		menu.DestroyMenu();
	} 
	else if (LOWORD(lParam) == WM_LBUTTONDBLCLK) 
	{
		if (!menu.LoadMenu(m_tnd.uID)) return 0;
		if (!(pSubMenu = menu.GetSubMenu(0))) return 0;

		// double click received, the default action is to execute first menu item
		::SetForegroundWindow(m_tnd.hWnd);
		::SendMessage(m_tnd.hWnd, WM_COMMAND, pSubMenu->GetMenuItemID(0), 0);
		menu.DestroyMenu();
	}

	return 1;
}