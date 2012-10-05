#ifndef __CJTABVIEW_H__
#define __CJTABVIEW_H__

////////////////////////////////////////////////////////////////
// Copyright 1999 Mike Manzo
// Class: CCJTabView
// ==========================================================================  
// HISTORY:	  
// ==========================================================================  
//			1.00	14 Feb 1999	- Initial release.  
// ==========================================================================  
//  
/////////////////////////////////////////////////////////////////////////////

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CJTabView.h : header file
//

typedef struct
{
	CWnd *pWnd;
	char szLabel[32];
}CTV_ITEM;

#define TABVIEW_BORDER 3
typedef CList <CTV_ITEM*, CTV_ITEM *>	CListViews;

/////////////////////////////////////////////////////////////////////////////
// CCJTabView window

class AFX_EXT_CLASS CCJTabView : public CCtrlView
{
// Construction
protected:
	CCJTabView();

	DECLARE_DYNCREATE(CCJTabView)

// Attributes
public:

// Operations
public:
	BOOL AddView(LPCTSTR lpszLabel, CRuntimeClass *pViewClass, CDocument* pDoc = NULL, CCreateContext *pContext = NULL);
	BOOL ModifyTabStyle(DWORD dwRemove, DWORD dwAdd, UINT nFlags=0);

	void SetActiveView(CRuntimeClass *pViewClass);
	void SetActiveView(int nNewTab);
	void RemoveView(int nView);

	CView* GetView(CRuntimeClass *pViewClass);
	CView* GetView(int nView);
	CView* GetActiveView();

	CImageList* SetTabImageList(CImageList *pImageList);
	CTabCtrl* GetTabCtrl() {return m_pTabCtrl;}

protected:
	CView*			m_pActiveView;
	int				m_nActiveTab;
	CTabCtrl*		m_pTabCtrl;
	CToolTipCtrl*	m_pToolTip;
	CFont			m_TabFont;
	CListViews		m_views;

private:
	void			CalcViewRect(CRect* pRect);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJTabView)
	public:
	virtual void OnInitialUpdate();
	//}}AFX_VIRTUAL
	public:
	virtual void GetChildRect(CRect &rect);

// Implementation
public:
	virtual ~CCJTabView();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCJTabView)
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSelchange(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __CJTABVIEW_H__
