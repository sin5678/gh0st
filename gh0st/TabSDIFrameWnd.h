/**********************************************************************
**
**	TabSDIFrameWnd.h : include file
**
**	by Andrzej Markowski July 2005
**
**********************************************************************/

#pragma once

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "stdafx.h"
#include "CustomTabCtrl.h"

#define IDC_TABCTRL		100

/////////////////////////////////////////////////////////////////////////////
// CTabSDIFrameWnd frame

class CTabSDIFrameWnd : public CFrameWnd
{
	DECLARE_DYNCREATE(CTabSDIFrameWnd)
protected:
	CTabSDIFrameWnd();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:
	CCustomTabCtrl& GetTabCtrl() {return m_wndTab;}
	BOOL AddView(CString sLabel, CView* pView, CString sTooltip);
	void DeleteContents();
	BOOL DeleteActiveView();
	BOOL SetCurView(int nNdx);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabSDIFrameWnd)
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CTabSDIFrameWnd();

	// Generated message map functions
	//{{AFX_MSG(CTabSDIFrameWnd)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	afx_msg void OnSelchangeTabctrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickTabctrl(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
protected:
	CCustomTabCtrl	m_wndTab;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

