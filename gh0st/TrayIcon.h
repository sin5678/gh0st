/////////////////////////////////////////////////////////////////////////////
// TrayIcon.h : header file
//

#ifndef _INCLUDED_TRAYICON_H_
#define _INCLUDED_TRAYICON_H_

/////////////////////////////////////////////////////////////////////////////
// CTrayIcon window

class CTrayIcon : public CObject
{
// Construction/destruction
public:
	CTrayIcon();
	CTrayIcon(CWnd* pWnd, UINT uCallbackMessage, LPCTSTR szTip, HICON icon, UINT uID);
	virtual ~CTrayIcon();

// Operations
public:
	BOOL Enabled() { return m_bEnabled; }
	BOOL Visible() { return !m_bHidden; }

	//Create the tray icon
	int Create(CWnd* pWnd, UINT uCallbackMessage, LPCTSTR szTip, HICON icon, UINT uID, BOOL bIsNotify = FALSE);

	//Change or retrieve the Tooltip text
	BOOL    SetTooltipText(LPCTSTR pszTooltipText);
	BOOL    SetTooltipText(UINT nID);
	CString GetTooltipText() const;

	//Change or retrieve the icon displayed
	BOOL  SetIcon(HICON hIcon);
	BOOL  SetIcon(LPCTSTR lpIconName);
	BOOL  SetIcon(UINT nIDResource);
	BOOL  SetStandardIcon(LPCTSTR lpIconName);
	BOOL  SetStandardIcon(UINT nIDResource);
	HICON GetIcon() const;
	void  HideIcon();
	void  ShowIcon();
	void  RemoveIcon();
	void  MoveToRight();

	//Change or retrieve the window to send notification messages to
	BOOL  SetNotificationWnd(CWnd* pNotifyWnd);
	CWnd* GetNotificationWnd() const;

	//Default handler for tray notification message
	virtual LRESULT OnTrayNotification(WPARAM uID, LPARAM lEvent);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTrayIcon)
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL			m_bEnabled;	// does O/S support tray icon?
	BOOL			m_bHidden;	// Has the icon been hidden?
	BOOL			m_bNotify;  // Has Notify style?
	NOTIFYICONDATA	m_tnd;

	DECLARE_DYNAMIC(CTrayIcon)
};


#endif

/////////////////////////////////////////////////////////////////////////////
