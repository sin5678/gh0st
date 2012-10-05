// CJMiniDockFrameWnd.cpp : implementation file
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
//			1.00	16 Jan 1999	- Initial release.  
// ==========================================================================  
//  
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CJMiniDockFrameWnd.h"
#include "CJDockContext.h"
#include "CJSizeDockBar.h"
#include "CJControlBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCJMiniDockFrameWnd

CCJMiniDockFrameWnd::CCJMiniDockFrameWnd()
{
	// TODO: add construction code here.
}

CCJMiniDockFrameWnd::~CCJMiniDockFrameWnd()
{
	// TODO: add destruction code here.
}

IMPLEMENT_DYNCREATE(CCJMiniDockFrameWnd, CMiniDockFrameWnd)

BEGIN_MESSAGE_MAP(CCJMiniDockFrameWnd, CMiniDockFrameWnd)
	//{{AFX_MSG_MAP(CCJMiniDockFrameWnd)
	ON_WM_NCLBUTTONDOWN()
	ON_WM_NCLBUTTONDBLCLK()
	ON_WM_ERASEBKGND()
	ON_WM_MBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCJMiniDockFrameWnd message handlers

void CCJMiniDockFrameWnd::OnNcLButtonDown(UINT nHitTest, CPoint point) 
{
	CCJSizeDockBar *pSizeBar = (CCJSizeDockBar*)&m_wndDockBar;
	
	if (nHitTest == HTCAPTION)
	{
		ActivateTopParent();
		if ((m_wndDockBar.m_dwStyle & CBRS_FLOAT_MULTI) == 0)
		{
			int nPos = 1;
			CControlBar* pBar = NULL;
			while(pBar == NULL && nPos < m_wndDockBar.m_arrBars.GetSize())
				pBar = pSizeBar->GetDockedSizeBar(nPos++);
			ASSERT(pBar != NULL);
			ASSERT_KINDOF(CControlBar, pBar);
			ASSERT(pBar->m_pDockContext != NULL);
			
			if (!pBar->IsKindOf(RUNTIME_CLASS(CCJControlBar))) {
				CMiniDockFrameWnd::OnNcLButtonDown(nHitTest,point);
				return;
			}

			((CCJDockContext *)pBar->m_pDockContext)->StartDragDockBar(point);
			return;
		}
	}

	else if (nHitTest >= HTSIZEFIRST && nHitTest <= HTSIZELAST)
	{
		ActivateTopParent();
		int nPos = 1;
		CControlBar* pBar = NULL;
		while(pBar == NULL && nPos < m_wndDockBar.m_arrBars.GetSize())
			pBar = pSizeBar->GetDockedSizeBar(nPos++);
		
		ASSERT(pBar != NULL);
		ASSERT_KINDOF(CControlBar, pBar);
		ASSERT(pBar->m_pDockContext != NULL);
		ASSERT((m_wndDockBar.m_dwStyle & CBRS_FLOAT_MULTI) == 0);
		
		if (!pBar->IsKindOf(RUNTIME_CLASS(CCJControlBar))) {
			CMiniDockFrameWnd::OnNcLButtonDown(nHitTest,point);
			return;
		}

		pBar->m_pDockContext->StartResize(nHitTest, point);
		return;
	}
	CMiniFrameWnd::OnNcLButtonDown(nHitTest, point);
}

void CCJMiniDockFrameWnd::OnNcLButtonDblClk(UINT nHitTest, CPoint point) 
{
	if (nHitTest == HTCAPTION)
	{
		ActivateTopParent();
		CCJSizeDockBar * m_pSizeBar = (CCJSizeDockBar *)&m_wndDockBar;
		
		if ((m_pSizeBar->m_dwStyle & CBRS_FLOAT_MULTI) == 0)
		{
			int nPos = 1;
			CControlBar* pBar = NULL;
			while(pBar == NULL && nPos < m_pSizeBar->m_arrBars.GetSize())
				pBar = m_pSizeBar->GetDockedSizeBar(nPos++);
			
			ASSERT(pBar != NULL);
			ASSERT_KINDOF(CControlBar, pBar);
			ASSERT(pBar->m_pDockContext != NULL);

			if (pBar->IsKindOf(RUNTIME_CLASS(CCJControlBar)))
				pBar->m_pDockContext->ToggleDocking();
			
			return;
		}
	}
}
