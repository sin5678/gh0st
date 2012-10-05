// CJControlBar.h : header file
//  
// DevStudio Style Resizable Docking Control Bar.  
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
//			1.00	17 Oct 1998	- Initial re-write and release.  
//			1.01	20 Oct 1998	- Fixed problem with gripper and buttons  
//								  disappearing when docking toggled. Overloaded  
//								  IsFloating() method from base class.  
//			1.02    22 Nov 1998 - Modified set cursor to display normal size
//								  cursor when static linked.
//			2.00    12 Jan 1999 - Total class re-write, added multiple/side-by-side
//								  controlbar docking. No longer uses CSizingControlBar
//								  base class.
//			2.01    31 Jan 1999 - Removed extra line (typo) from OnLButtonUp().
//								  Thanks to Ioannis Stamatopoulos (ystamat@mail.datamedia.gr)
//							      for pointing this out.
//			2.02	28 Feb 1999 - Calls default wnd proc if no context menu is defined
//								  Gonzalo Pereyra [persys@adinet.com.uy]
//			2.03	10 Mar 1999 - Added AfxRegisterWndClass() to create method to
//								  handle double clicks. Thanks to Takehiko Mizoguti [mizoguti@m2.sys.to.casio.co.jp]
//								  for some thoughts on this.
//								- Fixed memory leak with button tooltips.
//			2.04	13 Mar 1999 - Patrick Bergeron [patb@softimage.com] fixed the
//								  following bugs:
//
//								- I had a dialog nested in a CJControlBar. Unfortunately 
//								  when I tried to dock it to the left or right, the 
//								  bottom part of my dialog would be clipped off. 
//
//								  Fix: When in docked mode, ::CalcDynamicLayout needed 
//								  to return the desired size, while taking into	account 
//								  the size of the gripper bar (if any). Simply added a 
//								  simple "+GetMinExt()" Now my dialog's bottom doesn't 
//								  get clipped. Woo-ha.
//									 
//								- When nesting a CDialog based window inside a 
//								  CJControlBar, the 3D rect drawn by ::OnEraseBkgnd would
//								  be overwritten by the top and left dialog borders. The 
//							      problem was caused by the fact that in the 
//								  ::OnWindowPosChanged() method, the rectangle which is 
//								  used to move the window contains the border. 
//									 
//								  A simple call to rc.DeflateRect(1,1) solved this problem.
//
//								- Added a call to UpdateWindow() in ::OnEraseBkgnd(). 
//								  This helps with the flickering a bit, but I Am not sure
//								  this is the correct way to go. Incorporate at your own risks.
//
//								- Added 2 ASSERT()s in ::OnMouseMove(). This was not to 
//								  fix a bug, but at one point I was seeing strange things
//								  that lead me to add these asserts. They don't do 
//								  anything bad, so why remove them?
//
// ==========================================================================  
//  
/////////////////////////////////////////////////////////////////////////////

#ifndef __CJCONTROLBAR_H__
#define __CJCONTROLBAR_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "CJFlatButton.h"

/////////////////////////////////////////////////////////////////////////////
// CCJControlBar class

class AFX_EXT_CLASS CCJControlBar : public CControlBar
{
	DECLARE_DYNAMIC(CCJControlBar)

// Construction
public:
	CCJControlBar();

// Attributes
public:
	BOOL			m_bUnique;
	BOOL			m_bToFit;
protected:			
	int				m_iTrackBorderSize;
	int				m_cxOffset;
	int				m_cyOffset;
	UINT			m_iAuxImage;
	UINT			m_menuID;
	BOOL			m_bOkToDrag;
	BOOL			m_bMaximized;
	BOOL			m_bDragging;
	BOOL			m_bGripper;
	BOOL			m_bButtons;
	CRect			m_rcTrack;
	CSize			m_sizeNormal;
	CSize			m_sizeDesired;
	CPoint			m_ptCurDrag;
	CPoint			m_ptStartDrag;
	HCURSOR			m_curHSplitBar;
	HCURSOR			m_curVSplitBar;
	CWnd*			m_pChildWnd;
	CImageList		m_ImageList;
	CCJFlatButton	m_btnClose;
	CCJFlatButton	m_btnMinim;
	CToolTipCtrl	m_ToolTip;
	COLORREF		m_clrBtnHilite;
	COLORREF		m_clrBtnShadow;
	COLORREF		m_clrBtnFace;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJControlBar)
	public:
	virtual BOOL Create(CWnd* pParentWnd, UINT nID, LPCTSTR lpszWindowName = NULL, CSize sizeDefault = CSize(200,100), DWORD dwStyle = CBRS_LEFT);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	virtual CSize CalcDynamicLayout(int nLength, DWORD nMode);
 	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCJControlBar();

// Generated message map functions
protected:
	//{{AFX_MSG(CCJControlBar)
	afx_msg void OnNcPaint();
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSysColorChange();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnButtonClose();
	afx_msg void OnUpdateButtonClose(CCmdUI* pCmdUI);
	afx_msg void OnButtonMinimize();
	afx_msg void OnUpdateButtonMinimize(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	virtual void GetChildRect(CRect &rect);
	int GetMinExt();
	BOOL IsLeftDocked();
	BOOL IsRightDocked();
	BOOL IsTopDocked();
	BOOL IsBottomDocked();
	BOOL IsHorzDocked();
	BOOL IsVertDocked();
	void SetNormalSize(const CSize &cs);
	void Normalize();
	void Minimize();
	void Maximize(int size);
	void EnableDockingOnSizeBar(DWORD dwDockStyle);
	void ShowFrameControls(BOOL bGripper = TRUE, BOOL bButtons = TRUE);
	void SetMenuID(UINT nID);
	UINT GetMenuID();
protected:
	CRect GetGripperRect();
	CRect GetButtonRect();
	void DrawGripper(CDC* pDC);
	void SetChild(CWnd* pWnd);
	void OnInvertTracker(const CRect& rect);
	void HitTest(CPoint &point);
	virtual void DrawBorders(CDC *pDC, CRect &rect);
	void EraseNonClient();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __CJCONTROLBAR_H__

