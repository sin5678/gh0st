// CJMDIFrameWnd.h : header file
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
//			2.00	12 Jan 1999 - Total re-write to add side-by-side 
//								  docking support for CCJControlBar class.
// ==========================================================================
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __CJMDIFRAMEWND_H__
#define __CJMDIFRAMEWND_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CCJMDIFrameWnd class

class CCJSizeDockBar;
class AFX_EXT_CLASS CCJMDIFrameWnd : public CMDIFrameWnd
{
	DECLARE_DYNCREATE(CCJMDIFrameWnd)

// Construction
public:
	CCJMDIFrameWnd();

// Attributes
public:
protected:
	int m_aInitialSize[4];
	static DWORD dwDockBarMap[4][2];

// Operations
public:
	void SetInitialSize(int top, int bottom, int left, int right);
	void RecalcAllExcept(CCJSizeDockBar *pBar);
	void EnableDocking(DWORD dwDockStyle);
	void EnableDockingSizeBar(DWORD dwDockStyle);
	void DockSizeBar(CControlBar *,CCJSizeDockBar* =NULL,LPRECT =NULL);
	void DockControlBarLeftOf(CControlBar* Bar, CControlBar* LeftOf);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJMDIFrameWnd)
	public:
	virtual void RecalcLayout(BOOL bNotify = TRUE);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCJMDIFrameWnd();

// Generated message map functions
protected:
	//{{AFX_MSG(CCJMDIFrameWnd)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __CJMDIFRAMEWND_H__

