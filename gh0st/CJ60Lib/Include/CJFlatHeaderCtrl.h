// CJHeaderCtrl.h : header file
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
//      he has done with codeguru, enough can not be said, and for his article
//		'Indicating sort order in header control' which is where the code for
//		the sorting arrows in the header comes from.
//	<>  Many thanks to Microsoft for making the source code availiable for MFC. 
//		Since most of this work is a modification from existing classes and 
//		methods, this library would not have been possible.
//
// ==========================================================================  
// HISTORY:	  
// ==========================================================================  
//			1.00	16 Jan 1999	- Initial release.  
//			1.01	31 Jan 1999 - Fixed problem with sort arrow drawn. If hditem
//							      has an associated bitmap, the arrow is not drawn.
//			1.02	24 Feb 1999 - Fixed repainting problem when header is moved or
//								  window is resized. The class now uses log font
//								  instead of hard coded Tahoma font.
// ==========================================================================  
//  
/////////////////////////////////////////////////////////////////////////////

#ifndef __CJFLATHEADERCTRL_H__
#define __CJFLATHEADERCTRL_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CCJFlatHeaderCtrl window

class AFX_EXT_CLASS CCJFlatHeaderCtrl : public CHeaderCtrl
{
// Construction
public:
	CCJFlatHeaderCtrl();

// Attributes
public:
	BOOL  m_bFlatHeader;
	BOOL  m_bBoldFont;
	CFont m_HeaderFont;
	int	  m_nOffset;
	BOOL  m_bLBtnDown;

// Operations
public:
	void FlatHeader(BOOL bBoldFont = TRUE);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJFlatHeaderCtrl)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	void DrawFlatBorder();
	int SetSortImage( int nCol, BOOL bAsc );
	virtual ~CCJFlatHeaderCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCJFlatHeaderCtrl)
	afx_msg void OnPaint();
	afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:
	int  m_nSortCol;
	BOOL m_bSortAsc;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __CJFLATHEADERCTRL_H__
