// CJTabCtrlBar.h : header file
//
// DevStudio Style Resizable Docking Tab Control Bar.
//
// The code contained in this file is based on the original
// CSizingTabCtrlBar class written by Dirk Clemens,
//		mailto:dirk_clemens@hotmail.com
//		http://www.codeguru.com/docking/sizing_tabctrl_bar.shtml
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
//	<>  To Dirk Clemens (dirk_clemens@hotmail.com) for his CSizingTabCtrlBar
//		class, which is where the idea for this came from.
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
//          1.01    03 Jan 1999 - Application freezing bug fixed
//                                by LiangYiBin.Donald(mailto:lybd@yahoo.com)
//			1.02	17 Jan 1999 - Added helper class CCJTabCtrl to eliminate
//								  re-painting problems such as when the app
//								  is minimized then restored.
//			1.03    31 Jan 1999 - Commented out 2 calls to SetActiveView(). This
//								  somewhat fixes the problem with CTRL+TAB in
//								  activatation of MDI child windows, however
//								  if you click inside the view, this breaks 
//							      CTRL+TAB again, will look at this when I 
//								  get some free time. Thank to Pierre Guimont
//								  (Pierre_Guimont@mitel.com) for pointing this out.
//			1.04    31 Jan 1999 - Added m_pTabCtrl->GetRowCount() as per 
//								  Rashid Thadha (rashidt@consort.co.uk) request.
//			1.05	24 Feb 1999 - Phil Baxter [pgb@mrjinx.demon.co.uk] - Fixed the
//								  following problems when tabs are added after
//								  creation:
//								  1.	The tab was created, but the view that
//										was showing on the tab was not visible.
//								  2.	Get an assertion when the AddView function 
//										attempts to perform the AddTool function.
//								  3.	When a new view is added, although you set it 
//										to be the active view, it isn't raised to the 
//										top of the stack in the display.
//								  4.	The Tab name as stored in the TCB_ITEM structure 
//										is now a CString, this removes the 32 byte limit 
//										that it previously had.
//								  5.	New methods for obtaining the name as stored in 
//										the TCB_ITEM structure.
//								  6.	New Methods for deleting a view from the control, 
//										this removes the view entirely including its 
//										associated tab.
//								  7.	Some minor fixes around assertions when trying 
//										to manipulate the control when it has not had 
//										any tabs created.
//								  8.    Added two new member functions:
//										void SetViewToolTip(int nView, LPCTSTR lpszLabel);
//										void SetViewToolTip(CRuntimeClass *pViewClass, LPCTSTR lpszLabel);
//			1.06	14 Mar 1999 - Gonzalo Pereyra [ persys@adinet.com.uy ] -
//								  I wanted to trap NM_RETURN notifications in my 
//								  CTreeView Class. I couldn't... It turns out that 
//								  CControlbar::PreTranslateMessage() will eat all dialog 
//								  messages...It will never get dispatched to my window. 
//								  So i Overrided it in : CCJTabCtrlBar::PreTranslateMessage().
//								  this will allow for message reflection and we will get 
//								  NM_RETURN. I don't know if this a good solution but for 
//								  now , it works
// ==========================================================================
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __CJTABCTRLBAR_H__
#define __CJTABCTRLBAR_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afxtempl.h>
#include "CJControlBar.h"

typedef struct
{
	CWnd    *pWnd;
	CString szTabLabel;
	CString szToolTipLabel;
    UINT    uiToolTipId;
}TCB_ITEM;

/////////////////////////////////////////////////////////////////////////////
// CCJTabCtrlBar tab control bar

class CCJTabCtrl;
class AFX_EXT_CLASS CCJTabCtrlBar : public CCJControlBar
{
// Construction
public:
	CCJTabCtrlBar();

// Attributes
public:
protected:
	int				m_nActiveTab;
    int             m_nNextToolTipId;
	CFont			m_TabFont;
	CToolTipCtrl*	m_pToolTip;
	CCJTabCtrl*		m_pTabCtrl;
	CView*			m_pActiveView;
	CList <TCB_ITEM *,TCB_ITEM *> m_views;

// Operations
public:
	BOOL AddView(LPCTSTR lpszLabel, CRuntimeClass *pViewClass, CCreateContext *pContext=NULL);
	void RemoveView(int nView);
	void SetActiveView(int nNewTab);
	void SetActiveView(CRuntimeClass *pViewClass);
	CView* GetActiveView();
	CView* GetView(int nView);
	CView* GetView(CRuntimeClass *pViewClass);
	CImageList* SetTabImageList(CImageList *pImageList);
	BOOL ModifyTabStyle(DWORD dwRemove, DWORD dwAdd, UINT nFlags=0);
    LPCTSTR GetViewName(int nView);
    LPCTSTR GetViewName(CRuntimeClass *pViewClass);
    void DeleteView(int nView);
    void DeleteView(CRuntimeClass *pViewClass);
    void SetViewToolTip(int nView, LPCTSTR lpszLabel);
    void SetViewToolTip(CRuntimeClass *pViewClass, LPCTSTR lpszLabel);

// Overrides
public:
    // ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJTabCtrlBar)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL
	virtual void GetChildRect(CRect &rect);

// Implementation
public:
	virtual ~CCJTabCtrlBar();

// Generated message map functions
protected:
	//{{AFX_MSG(CCJTabCtrlBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	//}}AFX_MSG
	afx_msg void OnTabSelChange(NMHDR* pNMHDR, LRESULT* pResult) ;

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __CJTABCTRLBAR_H__
