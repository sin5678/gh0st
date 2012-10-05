// CJPagerCtrl.h : header file
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
//			1.01	02 Jan 1999 - Cleaned up class source, wrapped all pager
//								  messages.
// ==========================================================================
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __CJPAGERCTRL_H__
#define __CJPAGERCTRL_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CCJPagerCtrl window

#ifndef PGM_SETCHILD
#error CJPagerCtrl requires a newer version of the SDK than you have!
#error Please update your SDK files.
#endif

class AFX_EXT_CLASS CCJPagerCtrl : public CWnd
{
// Construction
public:
	CCJPagerCtrl();
	virtual ~CCJPagerCtrl();

// Attributes
public:
protected:
	int m_nWidth;
	int m_nHeight;

// Operations
public:
	void SetScrollArea (int nWidth, int nHeight)
		{ m_nWidth = nWidth; m_nHeight = nHeight; }

	// CPagerCtrl class wrappers.
	void SetChild(HWND hwndChild) { ASSERT(::IsWindow(m_hWnd));
		Pager_SetChild(m_hWnd, hwndChild); }

	void RecalcSize() { ASSERT(::IsWindow(m_hWnd));
		Pager_RecalcSize(m_hWnd); }

	void ForwardMouse(BOOL bForward) { ASSERT(::IsWindow(m_hWnd));
        Pager_ForwardMouse(m_hWnd, bForward); }

	COLORREF SetBkColor(COLORREF clr) { ASSERT(::IsWindow(m_hWnd));
        return Pager_SetBkColor(m_hWnd, clr); }

	COLORREF GetBkColor() { ASSERT(::IsWindow(m_hWnd));
        return Pager_GetBkColor(m_hWnd); }

	int SetBorder(int iBorder) { ASSERT(::IsWindow(m_hWnd));
        return Pager_SetBorder(m_hWnd, iBorder); }

	int GetBorder() { ASSERT(::IsWindow(m_hWnd));
		return Pager_GetBorder(m_hWnd); }

	int SetPos(int iPos) { ASSERT(::IsWindow(m_hWnd));
		return Pager_SetPos(m_hWnd, iPos); }

	int GetPos() { ASSERT(::IsWindow(m_hWnd));
		return Pager_GetPos(m_hWnd); }

	int SetButtonSize(int iSize) { ASSERT(::IsWindow(m_hWnd));
		return Pager_SetButtonSize(m_hWnd, iSize); }

	int GetButtonSize() { ASSERT(::IsWindow(m_hWnd));
		return Pager_GetButtonSize(m_hWnd); }

	DWORD GetButtonState(int iButton) { ASSERT(::IsWindow(m_hWnd));
		return Pager_GetButtonState(m_hWnd, iButton); }

	void GetDropTarget(IDropTarget **ppdt) { ASSERT(::IsWindow(m_hWnd));
        Pager_GetDropTarget(m_hWnd, ppdt); }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJPagerCtrl)
	public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
protected:
	//{{AFX_MSG(CCJPagerCtrl)
	virtual BOOL OnPagerScroll(NMPGSCROLL* pNMPGScroll, LRESULT * pResult);
	virtual BOOL OnPagerCalcSize(NMPGCALCSIZE * pNMPGCalcSize, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __CJPAGERCTRL_H__
