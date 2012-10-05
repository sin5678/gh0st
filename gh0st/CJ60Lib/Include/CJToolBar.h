// CJToolBar.h : header file
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
//  <>  Thanks to Joerg Koenig (Joerg.Koenig@rhein-neckar.de), which is where I
//		got some ideas about drawing 3D looking toolbars, from his article
//		'Another Flat ToolBar (does not require MSIE)'.
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
//			1.00	17 Oct 1998	- Initial re-write and release.
//			1.02	02 Nov 1998 - Fixed bug with DrawNoGripper() method -
//								  (Christian Skovdal Andersen).
//			1.03	14 Dec 1998 - Changed class to derive from Paul DiLascia's
//								  CFlatToolBar, this class adds gripper and
//								  control insertion.
//			1.04	13 Mar 1999 - Changed class to handle both Office and DevStudio
//								  style to toolbars. To use Office toolbars, use
//								  the CreateEx() method, otherwise your toolbars 
//								  will look like DevStudio.
// ==========================================================================
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __CJTOOLBAR_H__
#define __CJTOOLBAR_H__

#include "FlatBar.h"

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CCJToolBar class

class AFX_EXT_CLASS CCJToolBar : public CFlatToolBar
{
	DECLARE_DYNAMIC(CCJToolBar)

// Construction
public:
	CCJToolBar();

// Attributes
public:
protected:
	CObList* m_pControls;
	BOOL m_bGripper;
	COLORREF m_clrHilite;
	COLORREF m_clrShadow;
	COLORREF m_clrNormal;
	BOOL	 m_bOfficeStyle;

// Operations
public:
	void DrawNoGripper() { m_bGripper = FALSE; }
	void DrawBorders(CDC* pDC, CRect& rect);
	void EraseNonClient(BOOL);
	void DrawNormalGripper(CDC & dc) const;
	void DrawOfficeGripper(CDC & dc) const;
	CWnd* InsertControl(CRuntimeClass* pClass, CString strTitle, CRect& pRect, UINT nID, DWORD dwStyle );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJToolBar)
	public:
	virtual BOOL CreateEx(CWnd* pParentWnd, DWORD dwCtrlStyle = TBSTYLE_FLAT, DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP, CRect rcBorders = CRect(0, 0, 0, 0), UINT nID = AFX_IDW_TOOLBAR);
	//}}AFX_VIRTUAL

// Implementation
public:
	void NcCalcNormal(NCCALCSIZE_PARAMS FAR* lpncsp);
	void NcCalcOffice(NCCALCSIZE_PARAMS FAR* lpncsp);
	virtual ~CCJToolBar();

// Generated message map functions
protected:
	//{{AFX_MSG(CCJToolBar)
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnNcPaint();
	afx_msg void OnSysColorChange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __CJTOOLBAR_H__

