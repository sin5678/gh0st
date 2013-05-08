// CJSizeDockBar.cpp : implementation file
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
//			1.00	12 Jan 1999	- Initial release to add side-by-side 
//								  docking support for CCJControlBar class.
//			1.01	29 Jan 1999 - Made some cosmetic enhancements to more
//								  closely match DevStudio docking windows.
// ==========================================================================
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CJSizeDockBar.h"
#include "CJControlBar.h"
#include "CJToolBar.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCJSizeDockBar

CCJSizeDockBar::CCJSizeDockBar()
{
	m_iTrackBorderSize	= 4;	
	m_cxLeftBorder		= 0;
	m_cyTopBorder		= 0;
	m_cxRightBorder		= 0;
	m_cyBottomBorder	= 0;
	m_iActualSize		= 100;
	m_iSafeSpace		= 25;
	m_bOkToDrag			= FALSE;
	m_bDragging			= FALSE;
	m_bAutoDelete		= TRUE;
	m_clrBtnHilite		= ::GetSysColor(COLOR_BTNHILIGHT);
	m_clrBtnShadow		= ::GetSysColor(COLOR_BTNSHADOW);
	m_clrBtnFace		= ::GetSysColor(COLOR_BTNFACE);
}

CCJSizeDockBar::~CCJSizeDockBar()
{
	// TODO: add destruction code here.
}

IMPLEMENT_DYNAMIC(CCJSizeDockBar, CDockBar)

BEGIN_MESSAGE_MAP(CCJSizeDockBar, CDockBar)
	//{{AFX_MSG_MAP(CCJSizeDockBar)
	ON_WM_NCPAINT()
	ON_WM_NCCALCSIZE()
	ON_WM_SETCURSOR()
	ON_WM_NCHITTEST()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SYSCOLORCHANGE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCJSizeDockBar overrides

CSize CCJSizeDockBar::CalcDynamicLayout(int nLength, DWORD nMode)
{
	int ActualSize = 0;
	for(int i=0;i<= m_arrBars.GetUpperBound();++i)
	{
		CCJControlBar *pBar = (CCJControlBar *)GetDockedControlBar(i);
		if (pBar != NULL && pBar->IsVisible())
		{
			ActualSize = m_iActualSize;
			break;
		}
	}

	int cx,cy;
	if (nMode & LM_VERTDOCK) {
		cx = ActualSize ;
		cy = 32767;
	}
	
	if (nMode & LM_HORZDOCK) {
		cx= 32767;
		cy = ActualSize;
	}

	CalcSizeBarLayout();
	return CSize(cx,cy);
}

void CCJSizeDockBar::DoPaint(CDC *pDC)
{
	CDockBar::DoPaint(pDC);
}

BOOL CCJSizeDockBar::IsDockBar() const
{
	return FALSE;
}

BOOL CCJSizeDockBar::IsDockSizeBar() const
{
	return TRUE;
}

void CCJSizeDockBar::OnInvertTracker(const CRect& rect)
{
	ASSERT_VALID(this);
	ASSERT(!rect.IsRectEmpty());
	
	CDC* pDC = GetDockingFrame()->GetDC();
	
	CBrush* pBrush = CDC::GetHalftoneBrush();
	HBRUSH hOldBrush = NULL;
	
	if (pBrush != NULL)
		hOldBrush = (HBRUSH)SelectObject(pDC->m_hDC, pBrush->m_hObject);
	pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATINVERT);
	
	if (hOldBrush != NULL)
		SelectObject(pDC->m_hDC, hOldBrush);
	GetDockingFrame()->ReleaseDC(pDC);
}

void CCJSizeDockBar::HitTest(const CPoint & point)
{
	UINT nID = ((UINT)(WORD)::GetDlgCtrlID(m_hWnd));
	CRect rcWin;
	GetWindowRect(&rcWin);
	HCURSOR hCur;
	static BOOL bHitting,bUnItting;
	bHitting = FALSE;
	bUnItting = TRUE;
	BOOL bHit = FALSE;
	
	if (nID == AFX_IDW_SIZEBAR_LEFT) {
		rcWin.left = rcWin.right-m_iTrackBorderSize;
		hCur = AfxGetApp()->LoadCursor(AFX_IDC_HSPLITBAR);
		bHit = rcWin.PtInRect(point);
	}
	
	else if (nID == AFX_IDW_SIZEBAR_TOP) {
		rcWin.top = rcWin.bottom-m_iTrackBorderSize;
		hCur = AfxGetApp()->LoadCursor(AFX_IDC_VSPLITBAR);
		bHit = rcWin.PtInRect(point);	
	}
	
	else if (nID == AFX_IDW_SIZEBAR_RIGHT) {
		rcWin.right = rcWin.left+m_iTrackBorderSize;
		hCur = AfxGetApp()->LoadCursor(AFX_IDC_HSPLITBAR);
		bHit = rcWin.PtInRect(point);		
	}
	
	else if (nID == AFX_IDW_SIZEBAR_BOTTOM) {
		rcWin.bottom = rcWin.top+m_iTrackBorderSize;
		hCur = AfxGetApp()->LoadCursor(AFX_IDC_VSPLITBAR);
		bHit = rcWin.PtInRect(point);		
	}
	
	if (bHit) {
		SetCursor(hCur);
	}
	
	else  {
		hCur = ::LoadCursor(NULL,IDC_ARROW);
		SetCursor(hCur);
	}
	
	m_bOkToDrag = bHit;
}

/////////////////////////////////////////////////////////////////////////////
// CCJSizeDockBar message handlers

void CCJSizeDockBar::OnNcPaint() 
{
	EraseNonClient();
}

void CCJSizeDockBar::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
	UINT nID = ((UINT)(WORD)::GetDlgCtrlID(m_hWnd));
	
	if (nID == AFX_IDW_SIZEBAR_LEFT) {
		m_cyBottomBorder += m_iTrackBorderSize;
		CDockBar::OnNcCalcSize(bCalcValidRects, lpncsp);
		m_cyBottomBorder -= m_iTrackBorderSize;
	}
	
	else if (nID == AFX_IDW_SIZEBAR_TOP) {
		m_cyBottomBorder += m_iTrackBorderSize;
		CDockBar::OnNcCalcSize(bCalcValidRects, lpncsp);
		m_cyBottomBorder -= m_iTrackBorderSize;
	}
	
	else if (nID == AFX_IDW_SIZEBAR_RIGHT) {
		m_cyTopBorder += m_iTrackBorderSize;
		CDockBar::OnNcCalcSize(bCalcValidRects, lpncsp);
		m_cyTopBorder -= m_iTrackBorderSize;
	}
	
	else if (nID == AFX_IDW_SIZEBAR_BOTTOM) {
		m_cyTopBorder += m_iTrackBorderSize;
		CDockBar::OnNcCalcSize(bCalcValidRects, lpncsp);
		m_cyTopBorder -= m_iTrackBorderSize;
	}
}

BOOL CCJSizeDockBar::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	return (nHitTest == HTCLIENT)?
		CDockBar::OnSetCursor(pWnd, nHitTest, message):FALSE;
}

UINT CCJSizeDockBar::OnNcHitTest(CPoint point) 
{
	HitTest(point);
	return (m_bOkToDrag)?HTBORDER:CDockBar::OnNcHitTest(point);
}

void CCJSizeDockBar::OnNcLButtonDown(UINT nHitTest, CPoint point) 
{
	if( m_bOkToDrag )
	{
		UINT nID = ((UINT)(WORD)::GetDlgCtrlID(m_hWnd));
		CFrameWnd *pFrame=GetDockingFrame();
		GetWindowRect(m_rcTrack);
		
		if (nID == AFX_IDW_SIZEBAR_LEFT)
			m_rcTrack.left = m_rcTrack.right-m_iTrackBorderSize;
		
		else if (nID == AFX_IDW_SIZEBAR_TOP)
			m_rcTrack.top = m_rcTrack.bottom-m_iTrackBorderSize;
		
		else if (nID == AFX_IDW_SIZEBAR_RIGHT)
			m_rcTrack.right = m_rcTrack.left+m_iTrackBorderSize;
		
		else if (nID == AFX_IDW_SIZEBAR_BOTTOM)
			m_rcTrack.bottom = m_rcTrack.top+m_iTrackBorderSize;
		
		pFrame->ScreenToClient(&m_rcTrack);
		pFrame->ScreenToClient(&point);
		
		m_ptStartDrag = point;
		m_ptCurDrag = point;
		
		SetCapture();
		m_bDragging = TRUE;
		OnInvertTracker(m_rcTrack);
	}
	
	CDockBar::OnNcLButtonDown(nHitTest, point);
}

void CCJSizeDockBar::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if( m_bDragging )
	{
		CRect rectWin;
		CRect rectAvail;
		GetWindowRect(&rectWin);
		ReleaseCapture();
		m_bDragging = FALSE;
		OnInvertTracker(m_rcTrack);
		GetAvailableRect(rectAvail);
		UINT nID = ((UINT)(WORD)::GetDlgCtrlID(m_hWnd));
		
		if (nID == AFX_IDW_SIZEBAR_LEFT ) {
			int maxWidth = rectAvail.Width()-m_iSafeSpace;
			int newWidth = m_rcTrack.left-m_ptStartDrag.x;
			m_iActualSize = newWidth>maxWidth ? maxWidth: newWidth;
			m_iActualSize += rectWin.Width();
		}
		
		else if (nID == AFX_IDW_SIZEBAR_TOP) {
			int maxHeight = rectAvail.Height()-m_iSafeSpace;
			int newHeight = m_rcTrack.top-m_ptStartDrag.y;
			m_iActualSize = newHeight>maxHeight ? maxHeight : newHeight;
			m_iActualSize += rectWin.Height();
		}
		
		else if (nID == AFX_IDW_SIZEBAR_RIGHT) {
			int maxWidth = rectAvail.Width()-m_iSafeSpace;
			int newWidth = m_ptStartDrag.x-m_rcTrack.left;
			m_iActualSize = newWidth>maxWidth ? maxWidth: newWidth;
			m_iActualSize += rectWin.Width();
		}
		
		else if (nID == AFX_IDW_SIZEBAR_BOTTOM) {
			int maxHeight = rectAvail.Height()-m_iSafeSpace;
			int newHeight = m_ptStartDrag.y-m_rcTrack.top;
			m_iActualSize = newHeight>maxHeight ? maxHeight : newHeight;
			m_iActualSize += rectWin.Height();
		}
		
		if(m_iActualSize<m_iTrackBorderSize )
			m_iActualSize = m_iTrackBorderSize;
		
		GetDockingFrame()->RecalcLayout();
		RecalcAllExcept(NULL);
	}
	
	CDockBar::OnLButtonUp(nFlags, point);
}

void CCJSizeDockBar::OnMouseMove(UINT nFlags, CPoint point) 
{
	if( m_bDragging )
	{
		CRect rectWin;
		GetWindowRect(&rectWin);
		CRect rectAvail;
		GetAvailableRect(rectAvail);
		CFrameWnd *pFrame=GetDockingFrame();
		ClientToScreen(&point);
		pFrame->ScreenToClient(&point);
		UINT nID = ((UINT)(WORD)::GetDlgCtrlID(m_hWnd));
		
		if (nID == AFX_IDW_SIZEBAR_LEFT) {
			point.x = point.x>rectAvail.right ? rectAvail.right:point.x;
			point.x = point.x<m_iTrackBorderSize ? m_iTrackBorderSize:point.x;
		}
		
		else if (nID == AFX_IDW_SIZEBAR_TOP) {
			point.y = point.y>rectAvail.bottom ? rectAvail.bottom:point.y;
			point.y = point.y<m_iTrackBorderSize ? m_iTrackBorderSize:point.y;
		}
		
		else if (nID == AFX_IDW_SIZEBAR_RIGHT) {
			point.x = point.x<rectAvail.left ? rectAvail.left:point.x;
			point.x = point.x>rectAvail.right+m_iActualSize-m_iTrackBorderSize ? rectAvail.right-m_iTrackBorderSize+m_iActualSize:point.x;
		}
		
		else if (nID == AFX_IDW_SIZEBAR_BOTTOM) {
			point.y = point.y<rectAvail.top ? rectAvail.top:point.y;
			point.y = point.y>rectAvail.bottom+m_iActualSize-m_iTrackBorderSize ? rectAvail.bottom-m_iTrackBorderSize+m_iActualSize:point.y;		
		}
		
		int deltaX = point.x-m_ptCurDrag.x;
		int deltaY = point.y-m_ptCurDrag.y;
		
		m_ptCurDrag = point;
		
		if (nID == AFX_IDW_SIZEBAR_LEFT || nID == AFX_IDW_SIZEBAR_RIGHT && deltaX)
		{	
			OnInvertTracker(m_rcTrack);
			m_rcTrack.left+=deltaX;
			m_rcTrack.right+=deltaX;
			OnInvertTracker(m_rcTrack);
		}
		
		else if( nID == AFX_IDW_SIZEBAR_TOP || nID == AFX_IDW_SIZEBAR_BOTTOM && deltaY)
		{
			OnInvertTracker(m_rcTrack);
			m_rcTrack.top+=deltaY;
			m_rcTrack.bottom+=deltaY;
			OnInvertTracker(m_rcTrack);
		}
	}
	
	CDockBar::OnMouseMove(nFlags, point);
}

void CCJSizeDockBar::OnSysColorChange() 
{
	CDockBar::OnSysColorChange();
	
	m_clrBtnHilite	= ::GetSysColor(COLOR_BTNHILIGHT);
	m_clrBtnShadow	= ::GetSysColor(COLOR_BTNSHADOW);
	m_clrBtnFace	= ::GetSysColor(COLOR_BTNFACE);
}

/////////////////////////////////////////////////////////////////////////////
// CCJControlBar operations

void CCJSizeDockBar::GetAvailableRect(CRect &rect)
{
	GetDockingFrame()->GetClientRect(&rect);
	GetDockingFrame()->RepositionBars(0xffff, 0xffff, AFX_IDW_PANE_FIRST,reposQuery,
		&rect, &rect, TRUE);
}

BOOL CCJSizeDockBar::IsLastControlBar(int index)
{
	for( int i=index; i<m_arrBars.GetSize(); ++i)
	{
		CCJControlBar *pOther = (CCJControlBar *)GetDockedControlBar(i);
		if (pOther != NULL && pOther->IsVisible())
		{
			return FALSE;
		}
	}
	
	return TRUE;
}

CCJControlBar* CCJSizeDockBar::GetDockedSizeBar(int nPos)
{
	return (CCJControlBar*)GetDockedControlBar(nPos);
}

void CCJSizeDockBar::RecalcAllExcept(CCJSizeDockBar *pBar)
{
	CFrameWnd *pFrame = (CFrameWnd*)AfxGetApp()->m_pMainWnd;
	ASSERT_VALID(pFrame);
	
	for (int i = 0; i < 4; i++)
	{
		CCJSizeDockBar* pDock = (CCJSizeDockBar*)pFrame->GetControlBar(dwSizeBarMap[i][0]);
		if (pDock != NULL && pDock != pBar) {
			pDock->CalcSizeBarLayout();	
		}
	}
}

void CCJSizeDockBar::DrawBorders(CDC *pDC, CRect &rect)
{
	int cxBorderLeft	= 0;
	int cxBorderRight	= 0;
	int cyBorderTop		= 0;
	int cyBorderBottom	= 0;
	
	CRect rc;
	rc.CopyRect(&rect);
	
	if (m_dwStyle & CBRS_BORDER_TOP) {
		pDC->FillSolidRect(rc.left,rc.top,rc.Width(),1,m_clrBtnShadow);
		pDC->FillSolidRect(rc.left,rc.top+1,rc.Width(),1,m_clrBtnHilite);
		cyBorderTop+=2;
	}
	
	if (m_dwStyle & CBRS_BORDER_BOTTOM) {
		pDC->FillSolidRect(rc.left,rc.bottom-1,rc.Width(),1,m_clrBtnHilite);
		pDC->FillSolidRect(rc.left,rc.bottom-2,rc.Width(),1,m_clrBtnShadow);
		cyBorderBottom+=2;
	}
	
	if (m_dwStyle & CBRS_BORDER_LEFT) {
		pDC->FillSolidRect(rc.left,rc.top,1,rc.Height(),m_clrBtnShadow);
		pDC->FillSolidRect(rc.left+1,rc.top,1,rc.Height(),m_clrBtnHilite);
		cxBorderLeft+=2;
	}
	
	if (m_dwStyle & CBRS_BORDER_RIGHT) {
		pDC->FillSolidRect(rc.right-2,rc.top,1,rc.Height(),m_clrBtnShadow);
		pDC->FillSolidRect(rc.right-1,rc.top,1,rc.Height(),m_clrBtnHilite);
		cxBorderRight+=2;
	}
	
	UINT nID = ((UINT)(WORD)::GetDlgCtrlID(m_hWnd));
	
	CRect rcTrack;
	rcTrack.CopyRect(&rc);
	
	if (nID == AFX_IDW_SIZEBAR_LEFT) {
		rcTrack.left    = rc.right-m_iTrackBorderSize;
		rcTrack.right  += 1;
		rcTrack.top    += 1;
		rcTrack.bottom -= 1;
		pDC->FillSolidRect(rcTrack,m_clrBtnFace);
		pDC->Draw3dRect(rcTrack,m_clrBtnHilite,m_clrBtnShadow);
		cxBorderRight = m_iTrackBorderSize;
	}
	
	else if (nID == AFX_IDW_SIZEBAR_TOP) {
		rcTrack.top = rc.bottom-m_iTrackBorderSize;
		rcTrack.bottom+=1;
		pDC->FillSolidRect(rcTrack,m_clrBtnFace);
		pDC->Draw3dRect(rcTrack,m_clrBtnHilite,m_clrBtnShadow);
		cyBorderBottom = m_iTrackBorderSize;	
	}
	
	else if (nID == AFX_IDW_SIZEBAR_RIGHT) {
		rcTrack.right   = rc.left+m_iTrackBorderSize;
		rcTrack.left   -= 1;
		rcTrack.top    += 1;
		rcTrack.bottom -= 1;
		pDC->FillSolidRect(rcTrack,m_clrBtnFace);
		pDC->Draw3dRect(rcTrack,m_clrBtnHilite,m_clrBtnShadow);
		cxBorderLeft = m_iTrackBorderSize;		
	}
	
	else if (nID == AFX_IDW_SIZEBAR_BOTTOM) {
		rcTrack.bottom = rc.top+m_iTrackBorderSize;
		rcTrack.top-=1;
		pDC->FillSolidRect(rcTrack,m_clrBtnFace);
		pDC->Draw3dRect(rcTrack,m_clrBtnHilite,m_clrBtnShadow);
		cyBorderTop = m_iTrackBorderSize;	
	}
	
	rect.left	+= cxBorderLeft;
	rect.right	-= cxBorderRight;
	rect.top	+= cyBorderTop;
	rect.bottom	-= cyBorderBottom;
}

void CCJSizeDockBar::EraseNonClient()
{
	CWindowDC dc(this);
	CRect rectClient;
	GetClientRect(rectClient);
	CRect rectWindow;
	GetWindowRect(rectWindow);
	ScreenToClient(rectWindow);
	rectClient.OffsetRect(-rectWindow.left, -rectWindow.top);
	dc.ExcludeClipRect(rectClient);
	rectWindow.OffsetRect(-rectWindow.left, -rectWindow.top);
	DrawBorders(&dc, rectWindow);
	dc.IntersectClipRect(rectWindow);
	SendMessage(WM_ERASEBKGND, (WPARAM)dc.m_hDC);
}

void CCJSizeDockBar::SetActualSize(int iSize)
{
	m_iActualSize=iSize;
}

void CCJSizeDockBar::CalcSizeBarLayout()
{
	HDWP hDWP = ::BeginDeferWindowPos(m_arrBars.GetSize());
	CRect rectAvail;
	GetClientRect(&rectAvail);
	
	int nCount = 0;
	int lastLeft = 0;
	int lastRight = 0;
	int lastBottom = 0;
	int lastTop = 0;
	
	for( int i=0; i<m_arrBars.GetSize(); ++i)
	{
		CCJControlBar *pBar = (CCJControlBar *)GetDockedControlBar(i);
		if (pBar != NULL && pBar->IsVisible())
		{
			CRect rectBar;
			++nCount;
			pBar->GetWindowRect(&rectBar);
			pBar->m_bUnique = FALSE;
			ScreenToClient(&rectBar);
			CSize sizeIdeal = pBar->CalcDynamicLayout(-1,0);
			
			if( pBar->IsLeftDocked() )
			{
				rectBar.top = lastLeft;
				rectBar.left = 0;
				rectBar.right = rectAvail.Width();
				rectBar.bottom = rectBar.top + sizeIdeal.cy;
				
				if (rectBar.top>lastLeft)
					rectBar.top = lastLeft;
				
				lastLeft = rectBar.bottom;
				if (IsLastControlBar(i+1))
				{
					rectBar.bottom = rectAvail.bottom;
					pBar->m_bToFit = TRUE;
					if( nCount == 1 )
						pBar->m_bUnique = TRUE;
				}
				
				else
					pBar->m_bToFit = FALSE;
			}
			
			else if (pBar->IsTopDocked())
			{
				rectBar.left = lastTop;
				rectBar.top = 0;
				rectBar.bottom = rectAvail.Height();
				rectBar.right = rectBar.left + sizeIdeal.cx;	
				
				if( rectBar.left>lastTop )
					rectBar.left = lastTop;
				
				lastTop = rectBar.right;
				if (IsLastControlBar(i+1))
				{
					rectBar.right = rectAvail.right;
					pBar->m_bToFit = TRUE;
					
					if (nCount == 1)
						pBar->m_bUnique = TRUE;
				}
				
				else
					pBar->m_bToFit = FALSE;
			}
			
			else if (pBar->IsRightDocked())
			{
				rectBar.top = lastRight;
				rectBar.left = 0;
				rectBar.right = rectAvail.Width();
				rectBar.bottom = rectBar.top + sizeIdeal.cy;
				
				if (rectBar.top>lastRight)
					rectBar.top = lastRight;
				
				lastRight = rectBar.bottom;
				if (IsLastControlBar(i+1))
				{
					rectBar.bottom = rectAvail.bottom;
					pBar->m_bToFit = TRUE;
					
					if (nCount == 1)
						pBar->m_bUnique = TRUE;
				}
				
				else
					pBar->m_bToFit = FALSE;
			}
			
			else if (pBar->IsBottomDocked())
			{
				rectBar.left = lastBottom;
				rectBar.top = 0;
				rectBar.bottom = rectAvail.Height();
				rectBar.right = rectBar.left + sizeIdeal.cx;
				
				if( rectBar.left>lastBottom )
					rectBar.left = lastBottom;
				
				lastBottom = rectBar.right;
				if (IsLastControlBar(i+1))
				{
					rectBar.right = rectAvail.right;
					pBar->m_bToFit = TRUE;
					if( nCount == 1 )
						pBar->m_bUnique = TRUE;
				}
				
				else
					pBar->m_bToFit = FALSE;
			}
			
			pBar->SetWindowPos(NULL,rectBar.left,rectBar.top,rectBar.Width(),rectBar.Height(),SWP_NOZORDER);
//			pBar->Invalidate();
		}
	}
	
	::EndDeferWindowPos(hDWP);
}

void CCJSizeDockBar::Maximize(CCJControlBar* pBar)
{
	int iExt=0;
	CRect rectAvail;
	GetClientRect(rectAvail);
	
	for (int i=0; i<m_arrBars.GetSize(); ++i)
	{
		CCJControlBar *pBarDock = (CCJControlBar *)GetDockedControlBar(i);
		if (pBarDock && pBarDock->IsVisible() && pBarDock != pBar) {
			pBarDock->Minimize();
			iExt += pBarDock->GetMinExt();
		}
	}
	
	if (pBar->IsLeftDocked() || pBar->IsRightDocked()) {
		iExt = rectAvail.Height()-iExt;
	}
	
	if (pBar->IsTopDocked() || pBar->IsBottomDocked()) {
		iExt = rectAvail.Width()-iExt;
	}
	
	pBar->Maximize(iExt);
	CalcSizeBarLayout();
}

void CCJSizeDockBar::Normalize(CCJControlBar *)
{
	for( int i=0; i<m_arrBars.GetSize(); ++i) {
		CCJControlBar *pBarDock = (CCJControlBar*)GetDockedControlBar(i);
		if (pBarDock && pBarDock->IsVisible())
			pBarDock->Normalize();
	}
	
	CalcSizeBarLayout();
}
