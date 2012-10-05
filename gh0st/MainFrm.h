// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__3F531B4D_21DF_49EA_B4AF_63AA5555104F__INCLUDED_)
#define AFX_MAINFRM_H__3F531B4D_21DF_49EA_B4AF_63AA5555104F__INCLUDED_

#include "TrayIcon.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TabSDIFrameWnd.h"

#define CFrameWnd CTabSDIFrameWnd

class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	void ShowToolTips(LPCTSTR lpszText);
	void ShowConnectionsNumber();
	static void ProcessReceiveComplete(ClientContext *pContext);
	static void ProcessReceive(ClientContext *pContext);
	void Activate(UINT nPort, UINT nMaxConnections);
	static void CALLBACK NotifyProc(LPVOID lpParam, ClientContext* pContext, UINT nCode);
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif //_DEBUG

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg LRESULT OnTrayNotification(WPARAM wParam,LPARAM lParam);
	afx_msg void OnUpdateStatusBar(CCmdUI *pCmdUI);
	afx_msg void OnShow();
	afx_msg void OnHide();
	afx_msg void OnExit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CTrayIcon m_TrayIcon;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__3F531B4D_21DF_49EA_B4AF_63AA5555104F__INCLUDED_)
