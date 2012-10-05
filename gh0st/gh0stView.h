// gh0stView.h : interface of the CGh0stView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_GH0STVIEW_H__14553897_2664_48B4_A82B_6D6F8F789ED3__INCLUDED_)
#define AFX_GH0STVIEW_H__14553897_2664_48B4_A82B_6D6F8F789ED3__INCLUDED_


#include "gh0stDoc.h"
#include "SEU_QQwry.h"	// Added by ClassView


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CGh0stDoc;
struct ClientContext;

#define CListView CCJListView

class CGh0stView : public CListView
{
protected: // create from serialization only
	CGh0stView();
	DECLARE_DYNCREATE(CGh0stView)

// Attributes
public:
	CGh0stDoc* GetDocument();
// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGh0stView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	//}}AFX_VIRTUAL

// Implementation
public:
	CIOCPServer* m_iocpServer;
	int m_nCount;
	virtual ~CGh0stView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CGh0stView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnFilemanager();
	afx_msg void OnScreenspy();
	afx_msg void OnDownexec();
	afx_msg void OnWebcam();
	afx_msg void OnRemove();
	afx_msg void OnKeyboard();
	afx_msg void OnSystem();
	afx_msg void OnRemoteshell();
	afx_msg void OnLogoff();
	afx_msg void OnReboot();
	afx_msg void OnShutdown();
	afx_msg void OnSelectAll();
	afx_msg void OnUnselectAll();
	afx_msg void OnOpenUrlHide();
	afx_msg void OnOpenUrlShow();
	afx_msg void OnCleanevent();
	afx_msg void OnRenameRemark();
	afx_msg void OnUpdateServer();
	afx_msg void OnAudioListen();
	afx_msg void OnDisconnect();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CListCtrl *m_pListCtrl;
	SEU_QQwry *m_QQwry;
	void SendSelectCommand(PBYTE pData, UINT nSize);
	afx_msg LRESULT OnMyInitialUpdate(WPARAM /*wParam*/, LPARAM /*lParam*/);
	afx_msg LRESULT OnOpenManagerDialog(WPARAM /*wParam*/, LPARAM /*lParam*/);
	afx_msg	LRESULT OnOpenScreenSpyDialog(WPARAM, LPARAM);
	afx_msg	LRESULT	OnOpenWebCamDialog(WPARAM, LPARAM);
	afx_msg	LRESULT	OnOpenAudioDialog(WPARAM, LPARAM);
	afx_msg LRESULT OnOpenKeyBoardDialog(WPARAM, LPARAM);
	afx_msg LRESULT OnOpenSystemDialog(WPARAM, LPARAM);
	afx_msg LRESULT OnOpenShellDialog(WPARAM, LPARAM);
	afx_msg LRESULT OnRemoveFromList(WPARAM, LPARAM);
	afx_msg LRESULT OnAddToList(WPARAM, LPARAM);
};

#ifndef _DEBUG  // debug version in gh0stView.cpp
inline CGh0stDoc* CGh0stView::GetDocument()
   { return (CGh0stDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GH0STVIEW_H__14553897_2664_48B4_A82B_6D6F8F789ED3__INCLUDED_)
