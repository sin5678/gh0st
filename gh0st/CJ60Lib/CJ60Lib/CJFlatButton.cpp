// CJFlatButton.cpp : implementation file
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
//			1.01	02 Jan 1999 - Code clean up and re-organization, renamed
//								  CCJButton to CCJFlatButton.
//			1.02	13 Mar 1999 - Patrick Bergeron [patb@softimage.com] fixed the
//								  following bugs:
//
//								- Moving the cursor over the flat button doesn't 
//								  raise the button right away. We have to wait for 
//								  the first timer message to arrive, which is a bit 
//								  annoying. It gives the impression that the system 
//								  isn't very responsive.
//
//								  So in the OnMouseMove method, we call OnTimer(1) 
//								  to force the initial update of the button.
//
//								- Sometimes, a CJFlatButton would stay in a raised 
//								  state when moving the mouse quickly over the client 
//								  area of a few CJFlatButtons. The reason turned out 
//								  to be the use of a static (ahem, global) variable 
//								  "bPainted". The simple fix for this was to put 
//								  bPainted as a member variable ("m_bPainted") and use
//								  this member variable instead.
//
//								  Flat buttons are now much more responsive and behave 
//								  correctly in all cases.
//			1.03	14 Mar 1999 - Philip Lee [ pjl@nildram.co.uk ] - 
//								  the HICON returned by CButton::SetIcon should be 
//								  destroyed using ::DestroyIcon.
//
// ==========================================================================
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CJFlatButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCJFlatButton

CCJFlatButton::CCJFlatButton()
{
	m_nState = 0;
	m_bLBtnDown = FALSE;
	m_bFlatLook = TRUE;
	m_bPainted	= FALSE;
	m_clrHilite = ::GetSysColor(COLOR_BTNHIGHLIGHT);
	m_clrShadow = ::GetSysColor(COLOR_BTNSHADOW);
	m_clrDkShad = ::GetSysColor(COLOR_3DDKSHADOW);
	m_clrNormal = ::GetSysColor(COLOR_BTNFACE);
	m_clrTextGy = ::GetSysColor(COLOR_GRAYTEXT);
	m_clrTextNm = ::GetSysColor(COLOR_BTNTEXT);
}

CCJFlatButton::~CCJFlatButton()
{
}


BEGIN_MESSAGE_MAP(CCJFlatButton, CButton)
	//{{AFX_MSG_MAP(CCJFlatButton)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
	ON_WM_SYSCOLORCHANGE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCJFlatButton message handlers

void CCJFlatButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_bFlatLook) {
		SetTimer(1, 10, NULL);
		OnTimer(1);
	}

	CButton::OnMouseMove(nFlags, point);
}

void CCJFlatButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_bLBtnDown = TRUE;
	CButton::OnLButtonDown(nFlags, point);
}

void CCJFlatButton::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_bLBtnDown = FALSE;
	CButton::OnLButtonUp(nFlags, point);
}

void CCJFlatButton::OnTimer(UINT nIDEvent) 
{
	CRect rcItem;
	GetWindowRect(rcItem);

	CPoint ptCursor;
	GetCursorPos(&ptCursor);

	if ((m_bLBtnDown==TRUE) || (!rcItem.PtInRect(ptCursor)))
	{
		KillTimer (1);

		if (m_bPainted == TRUE) {
			InvalidateRect (NULL);
		}

		m_bPainted = FALSE;
		return;
	}

	// On mouse over, show raised button.
	else if(m_bFlatLook)
	{
		// Get the device context for the client area.
		CDC *pDC = GetDC();

		if (m_bPainted == FALSE)
		{
			// repaint client area.
			GetClientRect(rcItem);
			pDC->FillSolidRect(rcItem, m_clrNormal);

			DrawIcon(pDC);
			
			// draw the button rect.
			pDC->Draw3dRect(rcItem, m_clrHilite, m_clrShadow);
			m_bPainted = TRUE;
		}

		ReleaseDC (pDC);
	}
	
	CButton::OnTimer(nIDEvent);
}

void CCJFlatButton::OnSysColorChange() 
{
	CButton::OnSysColorChange();
	m_clrHilite = ::GetSysColor(COLOR_BTNHIGHLIGHT);
	m_clrShadow = ::GetSysColor(COLOR_BTNSHADOW);
	m_clrDkShad = ::GetSysColor(COLOR_3DDKSHADOW);
	m_clrNormal = ::GetSysColor(COLOR_BTNFACE);
	m_clrTextGy = ::GetSysColor(COLOR_GRAYTEXT);
	m_clrTextNm = ::GetSysColor(COLOR_BTNTEXT);
}

void CCJFlatButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	ASSERT(lpDrawItemStruct != NULL);	
	CDC* pDC = GetDC();
	
	// Get the button state.
	m_nState = lpDrawItemStruct->itemState;

	// Copy the rect, and fill the background.
	m_rcItem.CopyRect(&lpDrawItemStruct->rcItem);	
	pDC->FillSolidRect(m_rcItem, m_clrNormal);

	if(m_bFlatLook)
	{
		// Draw button pressed.
		if ((m_nState & ODS_SELECTED)) {
			pDC->Draw3dRect (m_rcItem, m_clrShadow, m_clrHilite);
		}
		// Draw button flat.
		else {
			pDC->Draw3dRect (m_rcItem, m_clrNormal, m_clrNormal);
		}
	}

	else
	{
		CRect rcItem(m_rcItem);
		rcItem.DeflateRect(1,1);

		// Draw button pressed.
		if ((m_nState & ODS_SELECTED)) {
			pDC->Draw3dRect (m_rcItem, m_clrDkShad, m_clrHilite);
			pDC->Draw3dRect (rcItem, m_clrShadow, m_clrNormal);
		}
		// Draw button raised.
		else {
			pDC->Draw3dRect (m_rcItem, m_clrHilite, m_clrDkShad);
			pDC->Draw3dRect (rcItem, m_clrNormal, m_clrShadow);
		}
	}

	// Save the item state, set background to transparent.
	pDC->SetBkMode( TRANSPARENT );
	DrawIcon(pDC);

	ReleaseDC (pDC);
}

void CCJFlatButton::DrawIcon(CDC *pDC)
{
	// if an icon is associated with button, draw it.
	HICON hIcon = GetIcon();
	HBITMAP hBitmap = GetBitmap();

	if (hIcon)
	{
		CRect rcWnd;
		GetWindowRect(&rcWnd);
		
		int left   = (rcWnd.Width()-m_sizeIcon.cx)/2;
		int right  = left+m_sizeIcon.cx;
		int top    = (rcWnd.Height()-m_sizeIcon.cy)/2;
		int bottom = top+m_sizeIcon.cy;

		m_rcItem.CopyRect(CRect(left,top,right,bottom));

		if ((m_nState & ODS_SELECTED)) {
			m_rcItem.OffsetRect(1,1);
		}

		DrawIconEx(
			pDC->GetSafeHdc(),
			m_rcItem.left,
			m_rcItem.top,
			hIcon,
			m_rcItem.Width(),
			m_rcItem.Height(),
			NULL,
			(HBRUSH)NULL,
			DI_NORMAL); 
	}
	else if (hBitmap)
	{
		// not implemented.
	}
	else
	{
		if (m_nState & ODS_DISABLED)
			pDC->SetTextColor(m_clrTextGy);
		else
			pDC->SetTextColor(m_clrTextNm);

		CFont newFont, *oldFont;

		NONCLIENTMETRICS ncm;
		ncm.cbSize = sizeof(NONCLIENTMETRICS);
		VERIFY(::SystemParametersInfo(SPI_GETNONCLIENTMETRICS,
			sizeof(NONCLIENTMETRICS), &ncm, 0));
		newFont.CreateFontIndirect(&ncm.lfMessageFont);
		oldFont = pDC->SelectObject(&newFont);

		CString strText;
		GetWindowText(strText);
		pDC->DrawText(strText, m_rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		pDC->SelectObject(oldFont);
	}
}

void CCJFlatButton::SetIcon(HICON hIcon, CSize size)
{
    m_sizeIcon = size;
    ::DestroyIcon( CButton::SetIcon(hIcon) );
}
