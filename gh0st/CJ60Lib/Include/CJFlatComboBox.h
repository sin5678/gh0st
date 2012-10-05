// CJFlatComboBox.h : header file
// 
// Copyright © 1998-99 Kirk Stowell   
//		mailto:kstowell@codejockeys.com
//		http://www.codejockeys.com/kstowell/
//
// This source code may be used in compiled form in any way you desire. 
// Source file(s) may be redistributed unmodified by any means PROVIDING
// they are not sold for profit without the authors expressed written consent,
// and providing that this notice and the authors name and all copyright
// notices remain intact. If the source code is used in any commercial
// applications then a statement along the lines of:
//
// "Portions Copyright © 1998-99 Kirk Stowell" must be included in the
// startup banner, "About" box or printed documentation. An email letting
// me know that you are using it would be nice as well. That's not much to ask
// considering the amount of work that went into this.
//
// This software is provided "as is" without express or implied warranty. Use
// it at your own risk! The author accepts no liability for any damage/loss of
// business that this product may cause.
//
// ==========================================================================  
//
// Acknowledgements:
//	<>  Many thanks to all of you, who have encouraged me to update my articles
//		and code, and who sent in bug reports and fixes.
//  <>  Many thanks Zafir Anjum (zafir@codeguru.com) for the tremendous job that
//      he has done with codeguru, enough can not be said!
//	<>  Many thanks to Microsoft for making the source code availiable for MFC. 
//		Since most of this work is a modification from existing classes and 
//		methods, this library would not have been possible.
//
// ==========================================================================  
// HISTORY:	
// ==========================================================================
//			1.00	02 Oct 1998	- First release version.
//			1.01	05 Oct 1998	- Fixed offset bug to use GetSystemMetrics(),
//								  instead of hard coded values -
//								  Todd Brannam (tbrannam@msn.com).
//			1.02	02 Nov 1998 - Added OnSetFocus(), OnKillFocus() and
//								  modified OnPaint() so the control remains
//								  beveled until it actually looses focus -
//								  (Vachik Hovhannissian).
//			1.03	02 Jan 1999 - Renamed class from CJComboBox to CCJFlatComboBox,
//								  changed DRAWSTATE to enum, removed method Offset(),
//								  and replaced with a direct call to ::GetSystemMetrics().
//			1.04	22 Jan 1999 - Fixed accelerator problem reported by YOSH YAMASHITA
//								  (yosh@muj.biglobe.ne.jp) with Matt Weagle's solution of
//								  overloading PreTranslateMessage().
// ==========================================================================
//
/////////////////////////////////////////////////////////////////////////////
#ifndef __CJCOMBOBOX_H__
#define __CJCOMBOBOX_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CCJComboBox window

typedef enum tagDRAWSTATE
{
	FC_DRAWNORMAL = 0,
	FC_DRAWRAISED,
	FC_DRAWPRESSD
} DRAWSTATE;

class AFX_EXT_CLASS CCJFlatComboBox : public CComboBox
{
	DECLARE_DYNAMIC(CCJFlatComboBox)

// Construction
public:
	CCJFlatComboBox();

// Attributes
public:
protected:
	BOOL m_bLBtnDown;
	BOOL m_bPainted;

// Operations
public:
protected:
	void DrawCombo(DRAWSTATE eStyle, COLORREF clrTopLeft, COLORREF clrBottomRight);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJFlatComboBox)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCJFlatComboBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCJFlatComboBox)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __CJCOMBOBOX_H__
