#if !defined(AFX_BUILDVIEW_H__CA6BE8F7_F9BB_43BD_BCC7_04CCFD83AB7F__INCLUDED_)
#define AFX_BUILDVIEW_H__CA6BE8F7_F9BB_43BD_BCC7_04CCFD83AB7F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BuildView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBuildView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif
#include "control/HoverEdit.h"
#include "control/WinXPButtonST.h"
//#define CButton	CCJFlatButton

class CBuildView : public CFormView
{
	//将构造函数和构析函数改为共有函数
public:
	CBuildView();
	virtual ~CBuildView();
	DECLARE_DYNCREATE(CBuildView)
private:
	bool m_bFirstShow;
	CWinXPButtonST	m_btn_release;
	CHoverEdit	m_Edit[4];
// Form Data
public:
	//{{AFX_DATA(CBuildView)
	enum { IDD = IDD_BUILD };
	CString	m_url;
	CString	m_encode;
	BOOL	m_enable_http;
	CString	m_ServiceDescription;
	CString	m_ServiceDisplayName;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBuildView)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL

// Implementation
protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CBuildView)
	afx_msg void OnBuild();
	afx_msg void OnEnableHttp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BUILDVIEW_H__CA6BE8F7_F9BB_43BD_BCC7_04CCFD83AB7F__INCLUDED_)
