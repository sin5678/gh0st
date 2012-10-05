// CJFlatHeaderCtrl.cpp : implementation file
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
//      he has done with codeguru, enough can not be said, and for his article
//		'Indicating sort order in header control' which is where the code for
//		the sorting arrows in the header comes from.
//	<>  Many thanks to Microsoft for making the source code availiable for MFC. 
//		Since most of this work is a modification from existing classes and 
//		methods, this library would not have been possible.
//
// ==========================================================================  
// HISTORY:	  
// ==========================================================================  
//			1.00	16 Jan 1999	- Initial release.  
//			1.01	31 Jan 1999 - Fixed problem with sort arrow drawn. If hditem
//							      has an associated bitmap, the arrow is not drawn.
//			1.02	24 Feb 1999 - Fixed repainting problem when header is moved or
//								  window is resized. The class now uses log font
//								  instead of hard coded Tahoma font.
// ==========================================================================  
//  
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CJFlatHeaderCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCJFlatHeaderCtrl

CCJFlatHeaderCtrl::CCJFlatHeaderCtrl()
{
	m_nSortCol	  = -1;
	m_bFlatHeader = FALSE;
	m_bBoldFont	  = FALSE;
	m_nOffset	  = 6;
	m_bLBtnDown	  = FALSE;
}

CCJFlatHeaderCtrl::~CCJFlatHeaderCtrl()
{
}

BEGIN_MESSAGE_MAP(CCJFlatHeaderCtrl, CHeaderCtrl)
	//{{AFX_MSG_MAP(CCJFlatHeaderCtrl)
	ON_WM_PAINT()
	ON_WM_WINDOWPOSCHANGING()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCJFlatHeaderCtrl message handlers

void CCJFlatHeaderCtrl::FlatHeader(BOOL bBoldFont/*=TRUE*/)
{
	// Get the log font.
	NONCLIENTMETRICS ncm;
	memset(&ncm, 0, sizeof(NONCLIENTMETRICS));
	ncm.cbSize = sizeof(NONCLIENTMETRICS);

	VERIFY(::SystemParametersInfo(SPI_GETNONCLIENTMETRICS,
		sizeof(NONCLIENTMETRICS), &ncm, 0));
	ncm.lfMessageFont.lfWeight = bBoldFont?700:400;

	m_HeaderFont.CreateFontIndirect(&ncm.lfMessageFont);
	SetFont(&m_HeaderFont);

	m_bBoldFont   = bBoldFont;
	m_bFlatHeader = TRUE;
}

int CCJFlatHeaderCtrl::SetSortImage(int nCol, BOOL bAsc)
{
	int nPrevCol = m_nSortCol;
	
	m_nSortCol = nCol;
	m_bSortAsc = bAsc;
	
	// Change the item to ownder drawn
	HD_ITEM hditem;
	
	hditem.mask = HDI_BITMAP | HDI_FORMAT;
	GetItem( nCol, &hditem );
	if (hditem.hbm == NULL)
	{
		hditem.fmt |= HDF_OWNERDRAW;
		SetItem( nCol, &hditem );

		// Invalidate header control so that it gets redrawn
		Invalidate();
	}
	
	return nPrevCol;
}

void CCJFlatHeaderCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CDC dc;

	dc.Attach( lpDrawItemStruct->hDC );
	
	// Get the column rect
	CRect rcLabel( lpDrawItemStruct->rcItem );
	
	// Save DC
	int nSavedDC = dc.SaveDC();
	
	// Set clipping region to limit drawing within column
	CRgn rgn;
	rgn.CreateRectRgnIndirect( &rcLabel );
	dc.SelectObject( &rgn );
	rgn.DeleteObject();
	
	// Draw the background
	dc.FillRect(rcLabel, &CBrush(::GetSysColor(COLOR_3DFACE)));
	dc.SetBkMode(TRANSPARENT);

	// Get the column text and format
	TCHAR buf[256];
	HD_ITEM hditem;
	
	hditem.mask = HDI_TEXT | HDI_FORMAT;
	hditem.pszText = buf;
	hditem.cchTextMax = 255;
	
	GetItem( lpDrawItemStruct->itemID, &hditem );
	
	// Determine format for drawing column label
	UINT uFormat = DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP 
		| DT_VCENTER | DT_END_ELLIPSIS ;
	
	if( hditem.fmt & HDF_CENTER)
		uFormat |= DT_CENTER;
	else if( hditem.fmt & HDF_RIGHT)
		uFormat |= DT_RIGHT;
	else
		uFormat |= DT_LEFT;
	
	// Adjust the rect if the mouse button is pressed on it
	if( lpDrawItemStruct->itemState == ODS_SELECTED )
	{
		rcLabel.left++;
		rcLabel.top += 2;
		rcLabel.right++;
	}
	
	// Adjust the rect further if Sort arrow is to be displayed
	if( lpDrawItemStruct->itemID == (UINT)m_nSortCol )
	{
		rcLabel.right -= 3 * m_nOffset;
	}
	
	rcLabel.left += m_nOffset;
	rcLabel.right -= m_nOffset;

	// Draw column label
	if( rcLabel.left < rcLabel.right )
	{
		if (m_bBoldFont) {
			dc.SelectObject(&m_HeaderFont);
		}
		dc.DrawText(buf,-1,rcLabel, uFormat);
	}

	// Draw the Sort arrow
	if( lpDrawItemStruct->itemID == (UINT)m_nSortCol )
	{
		CRect rcIcon( lpDrawItemStruct->rcItem );
		
		// Set up pens to use for drawing the triangle
		CPen penLite(PS_SOLID, 1, ::GetSysColor(COLOR_3DHILIGHT));
		CPen penShad(PS_SOLID, 1, ::GetSysColor(COLOR_3DSHADOW));
		CPen *pOldPen = dc.SelectObject( &penLite );
		
		if( m_bSortAsc )
		{
			// Draw triangle pointing upwards
			dc.MoveTo( rcIcon.right - 2*m_nOffset, m_nOffset-1);
			dc.LineTo( rcIcon.right - 3*m_nOffset/2, rcIcon.bottom - m_nOffset );
			dc.LineTo( rcIcon.right - 5*m_nOffset/2-2, rcIcon.bottom - m_nOffset );
			dc.MoveTo( rcIcon.right - 5*m_nOffset/2-1, rcIcon.bottom - m_nOffset-1 );
			
			dc.SelectObject( &penShad );
			dc.LineTo( rcIcon.right - 2*m_nOffset, m_nOffset-2);
		}
		else
		{
			// Draw triangle pointing downwords
			dc.MoveTo( rcIcon.right - 3*m_nOffset/2, m_nOffset-1);
			dc.LineTo( rcIcon.right - 2*m_nOffset-1, rcIcon.bottom - m_nOffset + 1 );
			dc.MoveTo( rcIcon.right - 2*m_nOffset-1, rcIcon.bottom - m_nOffset );
			
			dc.SelectObject( &penShad );
			dc.LineTo( rcIcon.right - 5*m_nOffset/2-1, m_nOffset -1 );
			dc.LineTo( rcIcon.right - 3*m_nOffset/2, m_nOffset -1);
		}
		
		// Restore the pen
		dc.SelectObject( pOldPen );
	}

	// Restore dc
	dc.RestoreDC( nSavedDC );
	
	// Detach the dc before returning
	dc.Detach();
}

void CCJFlatHeaderCtrl::OnPaint() 
{
	Default();
	
	if (m_bFlatHeader)
		DrawFlatBorder();
}

void CCJFlatHeaderCtrl::DrawFlatBorder()
{
	CDC* pDC = GetDC();

	CRect rci;
	GetWindowRect(&rci);
	ScreenToClient(&rci);

	// Cover up thick 3D border.
	pDC->Draw3dRect(rci, ::GetSysColor(COLOR_3DFACE),
		::GetSysColor(COLOR_3DFACE));
	rci.DeflateRect(1,1);
	pDC->Draw3dRect(rci, ::GetSysColor(COLOR_3DFACE),
		::GetSysColor(COLOR_3DFACE));

	// Draw flat style border around entire header.
	rci.InflateRect(1,1);
	pDC->Draw3dRect(rci, ::GetSysColor(COLOR_3DHILIGHT),
		::GetSysColor(COLOR_3DSHADOW));

	// Create the pens for further cover-up.
	CPen penLite(PS_SOLID, 1, ::GetSysColor(COLOR_3DHILIGHT));
	CPen penShad(PS_SOLID, 1, ::GetSysColor(COLOR_3DSHADOW));
	CPen penFace(PS_SOLID, 1, ::GetSysColor(COLOR_3DFACE));
	CPen *pOldPen = pDC->SelectObject( &penLite );

	pDC->SelectObject(&penFace);
	pDC->MoveTo(rci.right-1, 2);
	pDC->LineTo(rci.right-1, rci.bottom-2);

	// Set up the header item struct.
	HD_ITEM hdi;
	memset (&hdi, 0, sizeof(HD_ITEM));
	hdi.fmt  = HDF_STRING | HDF_LEFT | HDF_OWNERDRAW;
	hdi.mask = HDI_WIDTH | HDI_TEXT | HDI_FORMAT;
	int cx = 0;

	// For each header item found, do further cover up.
	for (int i = 0; i < GetItemCount(); ++i)
	{
		GetItem(i, &hdi);
		cx += hdi.cxy;

		pDC->SelectObject(&penShad);
		pDC->MoveTo(cx, 2);
		pDC->LineTo(cx, rci.bottom-2);

		pDC->SelectObject(&penLite);
		pDC->MoveTo(cx+1, 2);
		pDC->LineTo(cx+1, rci.bottom-2);

		pDC->SelectObject(&penFace);
		pDC->MoveTo(cx-1, 2);
		pDC->LineTo(cx-1, rci.bottom-2);

		pDC->SelectObject(&penFace);
		pDC->MoveTo(cx-2, 2);
		pDC->LineTo(cx-2, rci.bottom-2);
	}

	// Restore the pen and release device context.
	pDC->SelectObject( pOldPen );
	ReleaseDC(pDC);
}

void CCJFlatHeaderCtrl::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos) 
{
	CHeaderCtrl::OnWindowPosChanging(lpwndpos);
	Invalidate();
}
