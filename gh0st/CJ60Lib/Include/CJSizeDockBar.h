// CJSizeDockBar.h : header file
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
//			1.00	12 Jan 1999	- Initial release to add side-by-side 
//								  docking support for CCJControlBar class.
//			1.01	29 Jan 1999 - Made some cosmetic enhancements to more
//								  closely match DevStudio docking windows.
// ==========================================================================
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __CJSIZEDOCKBAR_H__
#define __CJSIZEDOCKBAR_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CCJSizeDockBar class
class CCJControlBar;
class AFX_EXT_CLASS CCJSizeDockBar : public CDockBar
{
	DECLARE_DYNAMIC(CCJSizeDockBar)

// Construction
public:
	CCJSizeDockBar();

// Attributes
public:
protected:
	int			m_iSafeSpace;
	CPoint		m_ptCurDrag;
	CRect		m_rcTrack;
	CPoint		m_ptStartDrag;
	BOOL		m_bDragging;
	BOOL		m_bOkToDrag;
	HCURSOR		m_curHSplitBar;
	HCURSOR		m_curVSplitBar;
	int			m_iTrackBorderSize;
	int			m_iActualSize;
	COLORREF	m_clrBtnHilite;
	COLORREF	m_clrBtnShadow;
	COLORREF	m_clrBtnFace;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJSizeDockBar)
	public:
	virtual CSize CalcDynamicLayout(int nLength, DWORD nMode);
	virtual void DoPaint(CDC *pDC);
	virtual BOOL IsDockBar() const;
	virtual BOOL IsDockSizeBar() const;
	virtual void OnInvertTracker(const CRect&);
	virtual void HitTest(const CPoint& point);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCJSizeDockBar();

// Generated message map functions
protected:
	//{{AFX_MSG(CCJSizeDockBar)
	afx_msg void OnNcPaint();
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSysColorChange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void Normalize(CCJControlBar*);
	void Maximize(CCJControlBar*);
	void CalcSizeBarLayout();
	void SetActualSize(int iSize);
	void EraseNonClient();
	void DrawBorders(CDC *pDC,CRect& rect);
	void RecalcAllExcept(CCJSizeDockBar *pBar);
	CCJControlBar* GetDockedSizeBar(int nPos);
protected:
	BOOL IsLastControlBar(int index);
	void GetAvailableRect(CRect &rect);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __CJSIZEDOCKBAR_H__

