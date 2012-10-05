// CJFlatComboBox.cpp : implementation file
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
//			1.01	05 Oct 1998	- Fixed offset bug to use GetSystemMetrics(),
//								  instead of hard coded values -
//								  Todd Brannam (tbrannam@msn.com).
//			1.02	02 Nov 1998 - Added OnSetFocus(), OnKillFocus() and
//								  modified OnPaint() so the control remains
//								  beveled until it actually looses focus -
//								  (Vachik Hovhannissian).
//			1.03	02 Jan 1999 - Renamed class from CJComboBox to CCJFlatComboBox,
//								  changed DRAWSTATE to enum, removed method Offset(),
//								  and replaced with a direct call to ::GetSystemMetrics().
//			1.04	22 Jan 1999 - Fixed  problem reported by YOSH YAMASHITA
//								  (yosh@muj.biglobe.ne.jp) with Matt Weagle's solution of
//								  overloading PreTranslateMessage().
// ==========================================================================
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CJFlatComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCJFlatComboBox

CCJFlatComboBox::CCJFlatComboBox()
{
	m_bLBtnDown = FALSE;
	m_bPainted  = FALSE;
}

CCJFlatComboBox::~CCJFlatComboBox()
{
}

IMPLEMENT_DYNAMIC(CCJFlatComboBox, CComboBox)

BEGIN_MESSAGE_MAP(CCJFlatComboBox, CComboBox)
	//{{AFX_MSG_MAP(CCJFlatComboBox)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCJFlatComboBox message handlers

void CCJFlatComboBox::OnMouseMove(UINT nFlags, CPoint point) 
{
	SetTimer(1,10,NULL);
	OnTimer(1);
	CComboBox::OnMouseMove(nFlags, point);
}

void CCJFlatComboBox::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_bLBtnDown = TRUE;
	CComboBox::OnLButtonDown(nFlags, point);
}

void CCJFlatComboBox::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_bLBtnDown = FALSE;
	Invalidate();
	CComboBox::OnLButtonUp(nFlags, point);
}

void CCJFlatComboBox::OnTimer(UINT nIDEvent) 
{
	POINT pt;
	GetCursorPos(&pt);
	CRect rcItem;
	GetWindowRect(&rcItem);

	// OnLButtonDown, show pressed.
	if (m_bLBtnDown==TRUE) {
		KillTimer (1);
		if (m_bPainted == TRUE) {
			DrawCombo(FC_DRAWPRESSD, ::GetSysColor(COLOR_BTNSHADOW),
				::GetSysColor(COLOR_BTNHIGHLIGHT));
			m_bPainted = FALSE;
		}
		return;
	}

	// If mouse leaves, show flat.
	if (!rcItem.PtInRect(pt)) {
		KillTimer (1);
		if (m_bPainted == TRUE) {
			DrawCombo(FC_DRAWNORMAL, ::GetSysColor(COLOR_BTNFACE),
				::GetSysColor(COLOR_BTNFACE));
			m_bPainted = FALSE;
		}
		return;
	}

	// On mouse over, show raised.
	else {
		if (m_bPainted == TRUE)
			return;
		else {
			m_bPainted = TRUE;
			DrawCombo(FC_DRAWRAISED, ::GetSysColor(COLOR_BTNSHADOW),
				::GetSysColor(COLOR_BTNHIGHLIGHT));
		}
	}
	
	CComboBox::OnTimer(nIDEvent);
}

void CCJFlatComboBox::OnPaint() 
{
	ModifyStyleEx (WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE,
		0, SWP_FRAMECHANGED);
	
	Default();

	CPoint pt;
	GetCursorPos(&pt);

	CRect rcItem;
	GetWindowRect(&rcItem);

	if ((rcItem.PtInRect(pt)) || (::GetFocus() == m_hWnd))
		DrawCombo(FC_DRAWRAISED, ::GetSysColor(COLOR_BTNSHADOW),
			::GetSysColor(COLOR_BTNHIGHLIGHT));
	else
		DrawCombo(FC_DRAWNORMAL, ::GetSysColor(COLOR_BTNFACE),
			::GetSysColor(COLOR_BTNFACE));
}

void CCJFlatComboBox::DrawCombo(DRAWSTATE eStyle, COLORREF clrTopLeft, COLORREF clrBottomRight)
{
	CRect rcItem;
	GetClientRect(&rcItem);
	CDC* pDC = GetDC();
	
	// Cover up dark 3D shadow.
	pDC->Draw3dRect(rcItem, clrTopLeft, clrBottomRight);
	rcItem.DeflateRect(1,1);
	
	if (!IsWindowEnabled()) {
		pDC->Draw3dRect(rcItem, ::GetSysColor(COLOR_BTNHIGHLIGHT),
			::GetSysColor(COLOR_BTNHIGHLIGHT));
	}
	
	else {
		pDC->Draw3dRect(rcItem, ::GetSysColor(COLOR_BTNFACE),
			::GetSysColor(COLOR_BTNFACE));
	}

	// Cover up dark 3D shadow on drop arrow.
	rcItem.DeflateRect(1,1);
	rcItem.left = rcItem.right-::GetSystemMetrics(SM_CXHTHUMB);;
	pDC->Draw3dRect(rcItem, ::GetSysColor(COLOR_BTNFACE),
		::GetSysColor(COLOR_BTNFACE));
	
	// Cover up normal 3D shadow on drop arrow.
	rcItem.DeflateRect(1,1);
	pDC->Draw3dRect(rcItem, ::GetSysColor(COLOR_BTNFACE),
		::GetSysColor(COLOR_BTNFACE));
	
	if (!IsWindowEnabled()) {
		return;
	}

	switch (eStyle)
	{
	case FC_DRAWNORMAL:
		rcItem.top -= 1;
		rcItem.bottom += 1;
		pDC->Draw3dRect(rcItem, ::GetSysColor(COLOR_BTNHIGHLIGHT),
			::GetSysColor(COLOR_BTNHIGHLIGHT));
		rcItem.left -= 1;
		pDC->Draw3dRect(rcItem, ::GetSysColor(COLOR_BTNHIGHLIGHT),
			::GetSysColor(COLOR_BTNHIGHLIGHT));
		break;

	case FC_DRAWRAISED:
		rcItem.top -= 1;
		rcItem.bottom += 1;
		pDC->Draw3dRect(rcItem, ::GetSysColor(COLOR_BTNHIGHLIGHT),
			::GetSysColor(COLOR_BTNSHADOW));
		break;

	case FC_DRAWPRESSD:
		rcItem.top -= 1;
		rcItem.bottom += 1;
		rcItem.OffsetRect(1,1);
		pDC->Draw3dRect(rcItem, ::GetSysColor(COLOR_BTNSHADOW),
			::GetSysColor(COLOR_BTNHIGHLIGHT));
		break;
	}

	ReleaseDC(pDC);
}

void CCJFlatComboBox::OnSetFocus(CWnd* pOldWnd) 
{
	CComboBox::OnSetFocus(pOldWnd);
	DrawCombo(FC_DRAWRAISED, ::GetSysColor(COLOR_BTNSHADOW),
		::GetSysColor(COLOR_BTNHIGHLIGHT));
}

void CCJFlatComboBox::OnKillFocus(CWnd* pNewWnd) 
{
	CComboBox::OnKillFocus(pNewWnd);
	DrawCombo(FC_DRAWNORMAL, ::GetSysColor(COLOR_BTNFACE),
		::GetSysColor(COLOR_BTNFACE));	
}

// Stoopid win95 accelerator key problem workaround - Matt Weagle.
BOOL CCJFlatComboBox::PreTranslateMessage(MSG* pMsg) 
{
	// Make sure that the keystrokes continue to the appropriate handlers
	if (pMsg->message == WM_KEYDOWN || pMsg->message==WM_KEYUP)
	{
		::TranslateMessage(pMsg);
		::DispatchMessage(pMsg);
		return TRUE;
	}
	
	// Catch the Alt key so we don't choke if focus is going to an owner drawn button
	if (pMsg->message == WM_SYSCHAR)
		return TRUE;
		
	return CComboBox::PreTranslateMessage(pMsg);
}
