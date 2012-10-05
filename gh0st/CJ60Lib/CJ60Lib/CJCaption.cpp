// CJCaption.cpp : implementation file
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
// ==========================================================================  
// HISTORY:	  
// ==========================================================================  
//			1.00	16 Jan 1999	- Initial first release.
//			1.01	01 Feb 1999 - Made some cosmetic fixes.  
//			1.02	03 Mar 1999 - Added further functionality to class so that
//								  you can define text size, text color, border
//								  size, border and background color. Basically to
//								  create a look similar to Outlook's Captions.
// ==========================================================================  
//  
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CJCaption.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCJCaption

CCJCaption::CCJCaption()
{
	m_clrBtnHilite  = ::GetSysColor(COLOR_BTNHILIGHT);
	m_clrBtnShadow  = ::GetSysColor(COLOR_BTNSHADOW);
	m_clrBtnFace    = ::GetSysColor(COLOR_BTNFACE);
	m_clrBorder		= m_clrBtnFace;
	m_clrBackgnd	= m_clrBtnFace;
	m_clrFont		= RGB(0,0,0);
	m_nBorder		= 0;
	m_pFont			= NULL;
	m_hIcon			= NULL;
	m_bBorder		= TRUE;
	m_bBannerOnly	= FALSE;

	// Get the log font.
	NONCLIENTMETRICS ncm;
	ncm.cbSize = sizeof(NONCLIENTMETRICS);
	VERIFY(::SystemParametersInfo(SPI_GETNONCLIENTMETRICS,
		sizeof(NONCLIENTMETRICS), &ncm, 0));
	m_Font.CreateFontIndirect(&ncm.lfMessageFont);
}

CCJCaption::~CCJCaption()
{
}

BEGIN_MESSAGE_MAP(CCJCaption, CStatic)
	//{{AFX_MSG_MAP(CCJCaption)
	ON_WM_PAINT()
	ON_WM_SYSCOLORCHANGE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCJCaption message handlers

void CCJCaption::SetBorderColor(COLORREF clr)
{
	m_clrBorder = clr;
}

void CCJCaption::SetBackGndColor(COLORREF clr)
{
	m_clrBackgnd = clr;
}

void CCJCaption::SetFontColor(COLORREF clr)
{
	m_clrFont = clr;
}

void CCJCaption::SetBorderThickness(int nBorder)
{
	m_nBorder = nBorder;
}

void CCJCaption::SetCaptionFont(CFont* pFont)
{
	m_pFont = pFont;
}

void CCJCaption::SetCaptionText(LPCTSTR lpszString, HICON hIcon)
{
	SetWindowText(lpszString);
	m_hIcon = hIcon;
	Invalidate();
}

void CCJCaption::ShowNoBorder(BOOL bShow)
{
	m_bBorder = bShow;
}

void CCJCaption::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// Get the client rect for this control.
	CRect rc;
	GetClientRect(&rc);

	PaintBackground(rc, &dc);
	PaintBanner(rc, &dc);
}

void CCJCaption::OnSysColorChange() 
{
	CStatic::OnSysColorChange();

	m_clrBtnHilite = ::GetSysColor(COLOR_BTNHILIGHT);
	m_clrBtnShadow = ::GetSysColor(COLOR_BTNSHADOW);
	m_clrBtnFace   = ::GetSysColor(COLOR_BTNFACE);
}

void CCJCaption::PaintBackground(const CRect rect, CDC *pDC)
{
	// Set the background to transparent, and draw a 3D
	// border around the control.
	pDC->SetBkMode(TRANSPARENT);
	pDC->FillSolidRect(rect, m_clrBtnFace);

	pDC->Draw3dRect(rect, m_clrBtnHilite, m_clrBtnShadow);

	if (!m_bBorder)
	{
		CPen pen;
		pen.CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_BTNFACE));

		pDC->SelectObject(&pen);
		pDC->MoveTo(rect.left+1, rect.bottom-1);
		pDC->LineTo(rect.right-1,rect.bottom-1);
	}
}

void CCJCaption::PaintBanner(CRect rect, CDC *pDC)
{
	pDC->SetTextColor(m_clrFont);
	
	if (m_nBorder!=0)
	{
		CRect rcBackGnd(rect);
		rcBackGnd.DeflateRect(m_nBorder, m_nBorder);
		pDC->FillSolidRect(rcBackGnd, m_clrBackgnd);
	}

	// Get the window text for this control.
	CString strText;
	GetWindowText(strText);

	rect.DeflateRect(2,2);
	rect.left += 8;

	// Shuffle fonts and paint the text.
	CFont* oldFont = pDC->SelectObject((m_pFont == NULL)?&m_Font:m_pFont);
	pDC->DrawText(strText, rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	pDC->SelectObject(oldFont);

	if (m_hIcon!=NULL)
	{
		int cx = ::GetSystemMetrics(SM_CXSMICON);
		int cy = ::GetSystemMetrics(SM_CYSMICON);
		int top = (rect.Height()-cy)/2+2;

		::DrawIconEx(pDC->GetSafeHdc(),
			rect.right-21, top,
			m_hIcon, cx, cy, NULL, (HBRUSH)NULL, DI_NORMAL); 
	}
}
