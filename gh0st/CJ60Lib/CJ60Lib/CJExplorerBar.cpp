// CJExplorerBar.cpp : implementation file
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
//			1.01	31 Jan 1999 - Made some cosmetic enhancements to more
//								  closely match the windows explorer bar.
// ==========================================================================  
//  
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Resource.h"
#include "CJExplorerBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCJExplorerBar

CCJExplorerBar::CCJExplorerBar()
{
	ShowFrameControls(FALSE, FALSE);
	m_strCaption = _T("");

	// Create the image list used by frame buttons.
	m_ImageList.Create(IDB_BTN_EXPLORER,
		18, 1, RGB(255,0,255));
}

CCJExplorerBar::~CCJExplorerBar()
{
}

IMPLEMENT_DYNAMIC(CCJExplorerBar, CCJControlBar)

BEGIN_MESSAGE_MAP(CCJExplorerBar, CCJControlBar)
	//{{AFX_MSG_MAP(CCJExplorerBar)
	ON_WM_CREATE()
	ON_COMMAND(IDC_BAR_BUTTON, OnButtonClose)
	ON_UPDATE_COMMAND_UI(IDC_BAR_BUTTON, OnUpdateButtonClose)
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCJExplorerBar message handlers

BOOL CCJExplorerBar::Create(CWnd *pParentWnd, UINT nID, LPCTSTR lpszWindowName, CSize sizeDefault, DWORD dwStyle)
{
	m_strCaption = lpszWindowName;

	return CCJControlBar::Create(
		pParentWnd, nID, lpszWindowName, sizeDefault, dwStyle);
}

int CCJExplorerBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CCJControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// Create the caption.
	if (!m_Caption.Create(m_strCaption,
		WS_VISIBLE|SS_CENTER|SS_CENTERIMAGE,
		CRect(0,0,0,0), this, IDC_BAR_CAPTION))
	{
		TRACE0(_T("Unable to create caption.\n"));
		return -1;
	}

	// Create the caption button.
	if (!m_CaptionButton.Create(NULL,
		WS_VISIBLE|BS_ICON|BS_OWNERDRAW|BS_CENTER|BS_VCENTER,
		CRect(0,0,0,0), this, IDC_BAR_BUTTON))
	{
		TRACE0(_T("Unable to create caption button.\n"));
		return -1;
	}
	
	// Associate the button icon
	m_CaptionButton.SetIcon(m_ImageList.ExtractIcon(0), CSize(18,15));
	return 0;
}

void CCJExplorerBar::OnButtonClose()
{
	GetDockingFrame()->ShowControlBar(this, FALSE, FALSE);
}

void CCJExplorerBar::OnUpdateButtonClose(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CCJExplorerBar::GetChildRect(CRect &rect)
{
	GetClientRect(&rect);
	
	if (!IsFloating())
	{
		rect.left	+= 1;
		rect.right  -= 1;
		rect.top	+= (IsVertDocked()?26:22);
	}

	else
		rect.DeflateRect(2,2);
}

void CCJExplorerBar::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
{
	CControlBar::OnWindowPosChanged(lpwndpos);
	
	int nCmdShow = IsFloating() ? SW_HIDE:SW_SHOW;
	m_Caption.ShowWindow(nCmdShow);
	m_CaptionButton.ShowWindow(nCmdShow);

	if (!IsFloating())
	{
		if (IsVertDocked())
		{
			m_Caption.MoveWindow(1,4,lpwndpos->cx-1,22);
			m_CaptionButton.MoveWindow(CRect(lpwndpos->cx-25,
				7, lpwndpos->cx-5, 24));
		}
		else
		{
			m_Caption.MoveWindow(1,0,lpwndpos->cx-1,22);
			m_CaptionButton.MoveWindow(CRect(lpwndpos->cx-25,
				3, lpwndpos->cx-5, 20));
		}
	}

	if (m_pChildWnd->GetSafeHwnd())
	{
		CRect rc;
		GetChildRect(rc);
		m_pChildWnd->MoveWindow(rc);
	}
}

BOOL CCJExplorerBar::OnEraseBkgnd(CDC* pDC) 
{
	int result = CControlBar::OnEraseBkgnd(pDC);

	CRect rect;
	GetClientRect(rect);
	rect.top+=(IsVertDocked()?3:-1);
	rect.bottom += 1;
	pDC->Draw3dRect(rect, m_clrBtnShadow, m_clrBtnShadow);
	return result;
}

void CCJExplorerBar::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// bypass CCJControlBar.
}

void CCJExplorerBar::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// bypass CCJControlBar.
}
