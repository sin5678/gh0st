// CJCaption.h : header file
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
// ==========================================================================  
// HISTORY:	  
// ==========================================================================  
//			1.00	16 Jan 1999	- Initial first release.  
//			1.01	01 Feb 1999 - Made some cosmetic fixes.  
//			1.02	03 Mar 1999 - Added further functionality to class so that
//								  you can define text size, text color, border
//								  size, border and background color. Basically to
//								  create a look similar to Outlook's Captions.
// ==========================================================================  
//  
/////////////////////////////////////////////////////////////////////////////

#ifndef __CJCAPTION_H__
#define __CJCAPTION_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CCJCaption window

class AFX_EXT_CLASS CCJCaption : public CStatic
{
// Construction
public:
	CCJCaption();

// Attributes
public:
protected:
	COLORREF		m_clrBtnHilite;
	COLORREF		m_clrBtnShadow;
	COLORREF		m_clrBtnFace;
	COLORREF		m_clrBorder;
	COLORREF		m_clrBackgnd;
	COLORREF		m_clrFont;
	int				m_nBorder;
	CFont*			m_pFont;
	HICON			m_hIcon;
	BOOL			m_bBorder;
	BOOL			m_bBannerOnly;

// Operations
public:
	void SetBorderColor(COLORREF clr);
	void SetBackGndColor(COLORREF clr);
	void SetFontColor(COLORREF clr);
	void SetBorderThickness(int nBorder);
	void SetCaptionFont(CFont* pFont);
	void SetCaptionText(LPCTSTR lpszString, HICON hIcon=NULL);
	void ShowNoBorder(BOOL bShow=FALSE);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJCaption)
	//}}AFX_VIRTUAL

// Implementation
public:
	void PaintBanner(CRect rect, CDC *pDC);
	void PaintBackground(const CRect rect, CDC* pDC);
	CFont m_Font;
	virtual ~CCJCaption();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCJCaption)
	afx_msg void OnPaint();
	afx_msg void OnSysColorChange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif //__CJCAPTION_H__
