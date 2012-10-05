// CJMDIFrameWnd.cpp : implementation file
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

#include "stdafx.h"
#include "CJMDIFrameWnd.h"
#include "CJDockBar.h"
#include "CJSizeDockBar.h"
#include "CJControlBar.h"
#include "CJMiniDockFrameWnd.h"
#include "CJToolBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCJMDIFrameWnd

CCJMDIFrameWnd::CCJMDIFrameWnd()
{
	for (int i = 0; i < 4; ++i)
		m_aInitialSize[i] = 0;
}

CCJMDIFrameWnd::~CCJMDIFrameWnd()
{
	// TODO: add destruction code here.
}

IMPLEMENT_DYNCREATE(CCJMDIFrameWnd, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CCJMDIFrameWnd, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CCJMDIFrameWnd)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCJMDIFrameWnd message handlers

// dwDockBarMap
DWORD CCJMDIFrameWnd::dwDockBarMap[4][2] =
{
	{ AFX_IDW_DOCKBAR_TOP,      CBRS_TOP    },
	{ AFX_IDW_DOCKBAR_BOTTOM,   CBRS_BOTTOM },
	{ AFX_IDW_DOCKBAR_LEFT,     CBRS_LEFT   },
	{ AFX_IDW_DOCKBAR_RIGHT,    CBRS_RIGHT  },
};

void CCJMDIFrameWnd::DockControlBarLeftOf(CControlBar* Bar, CControlBar* LeftOf)
{
	CRect rect;
	DWORD dw;
	UINT n;
	
	// get MFC to adjust the dimensions of all docked ToolBars
	// so that GetWindowRect will be accurate
	RecalcLayout(TRUE);
	
	LeftOf->GetWindowRect(&rect);
	rect.OffsetRect(1,0);
	dw=LeftOf->GetBarStyle();
	n = 0;
	n = (dw&CBRS_ALIGN_TOP)				 ? AFX_IDW_DOCKBAR_TOP		: n;
	n = (dw&CBRS_ALIGN_BOTTOM	&& n==0) ? AFX_IDW_DOCKBAR_BOTTOM	: n;
	n = (dw&CBRS_ALIGN_LEFT		&& n==0) ? AFX_IDW_DOCKBAR_LEFT		: n;
	n = (dw&CBRS_ALIGN_RIGHT	&& n==0) ? AFX_IDW_DOCKBAR_RIGHT	: n;
	
	// When we take the default parameters on rect, DockControlBar will dock
	// each Toolbar on a seperate line. By calculating a rectangle, we in effect
	// are simulating a Toolbar being dragged to that location and docked.
	DockControlBar(Bar,n,&rect);
}

void CCJMDIFrameWnd::EnableDocking(DWORD dwDockStyle)
{
	// must be CBRS_ALIGN_XXX or CBRS_FLOAT_MULTI only
	ASSERT((dwDockStyle & ~(CBRS_ALIGN_ANY|CBRS_FLOAT_MULTI)) == 0);
	CMDIFrameWnd::EnableDocking(dwDockStyle);
	
	for (int i = 0; i < 4; i++) {
		if (dwDockBarMap[i][1] & dwDockStyle & CBRS_ALIGN_ANY) {
			CDockBar* pDock = (CDockBar*)GetControlBar(dwDockBarMap[i][0]);
			
			// make sure the dock bar is of correct type
			if( pDock || !pDock->IsKindOf(RUNTIME_CLASS(CCJDockBar)) ) {
				BOOL bNeedDelete = ! pDock->m_bAutoDelete;
				pDock->m_pDockSite->RemoveControlBar(pDock);
				pDock->m_pDockSite = 0;	// avoid problems in destroying the dockbar
				pDock->DestroyWindow();
				if( bNeedDelete )
					delete pDock;
				pDock = 0;
			}
			
			if( pDock == 0 ) {
				pDock = new CCJDockBar;
				if (!pDock->Create(this,
					WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_CHILD|WS_VISIBLE |
					dwDockBarMap[i][1], dwDockBarMap[i][0])) {
					AfxThrowResourceException();
				}
			}
		}
	}
}

void CCJMDIFrameWnd::RecalcLayout(BOOL bNotify) 
{
	CMDIFrameWnd::RecalcLayout(bNotify);
	RecalcAllExcept(NULL);
}

void CCJMDIFrameWnd::EnableDockingSizeBar(DWORD dwDockStyle)
{
	// must be CBRS_ALIGN_XXX or CBRS_FLOAT_MULTI only
	ASSERT((dwDockStyle & ~(CBRS_ALIGN_ANY|CBRS_FLOAT_MULTI)) == 0);

	m_pFloatingFrameClass = RUNTIME_CLASS(CCJMiniDockFrameWnd);
	for (int i = 0; i < 4; i++)
	{
		if (dwSizeBarMap[i][1] & dwDockStyle & CBRS_ALIGN_ANY)
		{
			CCJSizeDockBar* pDock = (CCJSizeDockBar*)GetControlBar(dwSizeBarMap[i][0]);
			if (pDock == NULL)
			{
				pDock = new CCJSizeDockBar;
				if( m_aInitialSize[i] )
					pDock->SetActualSize(m_aInitialSize[i]);
				if (!pDock->Create(this,
					WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_CHILD|WS_VISIBLE |
						dwSizeBarMap[i][1], dwSizeBarMap[i][0]))
				{
					AfxThrowResourceException();
				}
			}
		}
	}
}

void CCJMDIFrameWnd::RecalcAllExcept(CCJSizeDockBar *pBar)
{
	for (int i = 0; i < 4; i++)
	{
		CCJSizeDockBar* pDock = (CCJSizeDockBar*)GetControlBar(dwSizeBarMap[i][0]);
		if (pDock != NULL && pDock != pBar)
		{
			pDock->CalcSizeBarLayout();	
		}
	}
}

void CCJMDIFrameWnd::SetInitialSize(int top, int bottom, int left, int right)
{
	m_aInitialSize[0] = top;	
	m_aInitialSize[1] = bottom;	
	m_aInitialSize[2] = left;	
	m_aInitialSize[3] = right;	
}

void CCJMDIFrameWnd::DockSizeBar(CControlBar * pBar,CCJSizeDockBar *pDockBar, LPRECT lpRect)
{
	ASSERT(pBar != NULL);
	ASSERT(pBar->m_pDockContext != NULL);
	
	if( pDockBar == NULL )
	{
		for (int i = 0; i < 4; i++)
		{
			if ((dwSizeBarMap[i][1] & CBRS_ALIGN_ANY) ==
				(pBar->m_dwStyle & CBRS_ALIGN_ANY))
			{
				pDockBar = (CCJSizeDockBar*)GetControlBar(dwSizeBarMap[i][0]);
				ASSERT(pDockBar != NULL);
				break;
			}
		}
	}
	
	ASSERT(pDockBar != NULL);
	ASSERT(m_listControlBars.Find(pBar) != NULL);
	ASSERT(pBar->m_pDockSite == this);
	((CCJControlBar*)pBar)->Normalize();
	pDockBar->DockControlBar(pBar, lpRect);
}

