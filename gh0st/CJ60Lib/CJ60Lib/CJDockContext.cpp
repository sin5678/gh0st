// CJDockContext.cpp : implementation file
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
//			1.00	12 Jan 1999 - Initial creation to add side-by-side 
//								  docking support for CCJControlBar class.
//			1.01	03 Mar 1999 - Scott Killen [s_killen@yahoo.com] - If your 
//								  frame is not your main window in your app, 
//								  you cannot dock a floating CCJControlBar. 
//								  Changed (CFrameWnd*)AfxGetApp()->m_pMainWnd
//								  to (CFrameWnd*)m_pDockSite in CanDockDockBar()
//								  and DockSizeBar()
//			1.02	10 Mar 1999 - Un-commented toggle docking code.
// ==========================================================================
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CJDockContext.h"
#include "CJControlBar.h"
#include "CJSizeDockBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define HORZF(dw) (dw & CBRS_ORIENT_HORZ)
#define VERTF(dw) (dw & CBRS_ORIENT_VERT)

static void AdjustRectangle(CRect& rect, CPoint pt)
{
	int nXOffset = (pt.x < rect.left) ? (pt.x - rect.left) :
					(pt.x > rect.right) ? (pt.x - rect.right) : 0;
	int nYOffset = (pt.y < rect.top) ? (pt.y - rect.top) :
					(pt.y > rect.bottom) ? (pt.y - rect.bottom) : 0;
	rect.OffsetRect(nXOffset, nYOffset);
}

/////////////////////////////////////////////////////////////////////////////
// CCJDockContext

CCJDockContext::CCJDockContext(CControlBar* pBar) : CDockContext(pBar)
{
	// TODO: add construction code here.
}

CCJDockContext::~CCJDockContext()
{
	// TODO: add destruction code here.
}

/////////////////////////////////////////////////////////////////////////////

void CCJDockContext::StartDragDockBar(CPoint pt)
{
	ASSERT_VALID(m_pBar);
	m_bDragging = TRUE;
	
	InitLoop();
	
	if (m_pBar->m_dwStyle & CBRS_SIZE_DYNAMIC)
	{
		CRect rect;
		m_pBar->GetWindowRect(rect);
		m_ptLast = pt;
		CSize sizeHorz = m_pBar->CalcDynamicLayout(0, LM_HORZ | LM_HORZDOCK);
		CSize sizeVert = m_pBar->CalcDynamicLayout(0, LM_VERTDOCK);
		CSize sizeFloat = m_pBar->CalcDynamicLayout(0, LM_HORZ | LM_MRUWIDTH);
		
		m_rectDragHorz = CRect(rect.TopLeft(), sizeHorz);
		m_rectDragVert = CRect(rect.TopLeft(), sizeVert);
		
		// calculate frame dragging rectangle
		m_rectFrameDragHorz = CRect(rect.TopLeft(), sizeFloat);
		m_rectFrameDragVert = CRect(rect.TopLeft(), sizeFloat);
		
		CMiniFrameWnd::CalcBorders(&m_rectFrameDragHorz);
		CMiniFrameWnd::CalcBorders(&m_rectFrameDragVert);
		
		m_rectFrameDragHorz.InflateRect(-afxData.cxBorder2, -afxData.cyBorder2);
		m_rectFrameDragVert.InflateRect(-afxData.cxBorder2, -afxData.cyBorder2);
	}
	
	else if (m_pBar->m_dwStyle & CBRS_SIZE_FIXED)
	{
		CRect rect;
		m_pBar->GetWindowRect(rect);
		m_ptLast = pt;
		CSize sizeHorz = m_pBar->CalcDynamicLayout(-1, LM_HORZ | LM_HORZDOCK);
		CSize sizeVert = m_pBar->CalcDynamicLayout(-1, LM_VERTDOCK);
		
		m_rectFrameDragHorz = m_rectDragHorz = CRect(rect.TopLeft(), sizeHorz);
		m_rectFrameDragVert = m_rectDragVert = CRect(rect.TopLeft(), sizeVert);
		
		CMiniFrameWnd::CalcBorders(&m_rectFrameDragHorz);
		CMiniFrameWnd::CalcBorders(&m_rectFrameDragVert);
		m_rectFrameDragHorz.InflateRect(-afxData.cxBorder2, -afxData.cyBorder2);
		m_rectFrameDragVert.InflateRect(-afxData.cxBorder2, -afxData.cyBorder2);
	}
	
	else
	{
		CRect rect;
		m_pBar->GetWindowRect(rect);
		m_ptLast = pt;
		BOOL bHorz = HORZF(m_dwStyle);
		DWORD dwMode = !bHorz ? (LM_HORZ | LM_HORZDOCK) : LM_VERTDOCK;
		CSize size = m_pBar->CalcDynamicLayout(-1, dwMode);
		
		if (bHorz)
		{
			m_rectDragHorz = rect;
			m_rectDragVert = CRect(CPoint(pt.x - rect.Height()/2, rect.top), size);
		}
		
		else 
		{
			m_rectDragVert = rect;
			m_rectDragHorz = CRect(CPoint(rect.left, pt.y - rect.Width()/2), size);
		}
		
		m_rectFrameDragHorz = m_rectDragHorz;
		m_rectFrameDragVert = m_rectDragVert;
		
		CMiniFrameWnd::CalcBorders(&m_rectFrameDragHorz);
		CMiniFrameWnd::CalcBorders(&m_rectFrameDragVert);
		m_rectFrameDragHorz.InflateRect(-afxData.cxBorder2, -afxData.cyBorder2);
		m_rectFrameDragVert.InflateRect(-afxData.cxBorder2, -afxData.cyBorder2);
	}
	
	AdjustRectangle(m_rectDragHorz, pt);
	AdjustRectangle(m_rectDragVert, pt);
	AdjustRectangle(m_rectFrameDragHorz, pt);
	AdjustRectangle(m_rectFrameDragVert, pt);
	
	m_dwOverDockStyle = CanDockDockBar();
	MoveDockBar(pt);   
	TrackDockBar();
}

DWORD CCJDockContext::CanDockDockBar()
{
	BOOL bStyleHorz;
	DWORD dwDock = 0; 
	DWORD dwCurr = 0; 
	
	ASSERT(m_dwStyle != 0);
	ASSERT(m_pDockSite->IsKindOf(RUNTIME_CLASS(CFrameWnd)));
	bStyleHorz = HORZF(m_dwStyle);
	bStyleHorz = m_bFlip ? !bStyleHorz : bStyleHorz;
	
	if (bStyleHorz && HORZF(m_dwDockStyle))
	{
		dwDock = CanDockDockBar(m_rectDragHorz,
			m_dwDockStyle & ~CBRS_ORIENT_VERT);
	}
	
	else if (VERTF(m_dwDockStyle))
	{
		dwDock = CanDockDockBar(m_rectDragVert,
			m_dwDockStyle & ~CBRS_ORIENT_HORZ);
	}
	
	if (!m_bFlip)
	{
		if (dwDock == 0 && HORZF(m_dwDockStyle))
		{
			dwCurr = CanDockDockBar(m_rectDragVert,
				m_dwDockStyle & ~CBRS_ORIENT_VERT);
			dwDock = CanDockDockBar(m_rectDragHorz,
				m_dwDockStyle & ~CBRS_ORIENT_VERT);
			dwDock = (dwDock == dwCurr) ? dwDock : 0;
		}
		
		if (dwDock == 0 && VERTF(m_dwDockStyle))
		{
			dwCurr = CanDockDockBar(m_rectDragHorz,
				m_dwDockStyle & ~CBRS_ORIENT_HORZ);
			dwDock = CanDockDockBar(m_rectDragVert,
				m_dwDockStyle & ~CBRS_ORIENT_HORZ);
			dwDock = (dwDock == dwCurr) ? dwDock : 0;
		}
	}
	
	return dwDock;
}

DWORD CCJDockContext::CanDockDockBar(CRect rect, DWORD dwDockStyle, CDockBar** ppDockBar)
{
	CFrameWnd *pFrame = (CFrameWnd*)m_pDockSite;
	ASSERT_VALID(pFrame);

	dwDockStyle &= CBRS_ALIGN_ANY|CBRS_FLOAT_MULTI;

	if (ppDockBar != NULL)
		*ppDockBar = NULL;
	POSITION pos = pFrame->m_listControlBars.GetHeadPosition();
	while (pos != NULL)
	{
		CCJSizeDockBar* pDockBar = (CCJSizeDockBar*)pFrame->m_listControlBars.GetNext(pos);
		if( !pDockBar->IsKindOf(RUNTIME_CLASS(CCJSizeDockBar)) )
			continue;
		if (pDockBar->IsDockSizeBar() && pDockBar->IsWindowVisible() &&
			(pDockBar->m_dwStyle & dwDockStyle & CBRS_ALIGN_ANY) &&
			(!pDockBar->m_bFloating ||
				(dwDockStyle & pDockBar->m_dwStyle & CBRS_FLOAT_MULTI)))
		{
			CRect rectBar;
			pDockBar->GetWindowRect(&rectBar);
			if (rectBar.Width() == 0)
				rectBar.right++;
			if (rectBar.Height() == 0)
				rectBar.bottom++;
			if (rectBar.IntersectRect(rectBar, rect))
			{
				if (ppDockBar != NULL)
					*ppDockBar = pDockBar;
				return pDockBar->m_dwStyle & dwDockStyle;
			}
		}
	}
	return 0;
}

BOOL CCJDockContext::TrackDockBar()
{
	if (::GetCapture() != NULL)
		return FALSE;
	m_pBar->SetCapture();
	ASSERT(m_pBar == CWnd::GetCapture());
	
	while (CWnd::GetCapture() == m_pBar)
	{
		MSG msg;
		if (!::GetMessage(&msg, NULL, 0, 0))
		{
			AfxPostQuitMessage(msg.wParam);
			break;
		}
		
		switch (msg.message)
		{
		case WM_LBUTTONUP:
			if (m_bDragging)
				EndDragDockBar();
			else
				EndResize();
			return TRUE;
		case WM_MOUSEMOVE:
			if (m_bDragging)
				MoveDockBar(msg.pt);
			else
				Stretch(msg.pt);
			break;
		case WM_KEYUP:
			if (m_bDragging)
				OnKey((int)msg.wParam, FALSE);
			break;
		case WM_KEYDOWN:
			if (m_bDragging)
				OnKey((int)msg.wParam, TRUE);
			if (msg.wParam == VK_ESCAPE)
			{
				CancelLoop();
				return FALSE;
			}
			break;
		case WM_RBUTTONDOWN:
			CancelLoop();
			return FALSE;
			
			// just dispatch rest of the messages
		default:
			DispatchMessage(&msg);
			break;
		}
	}
	
	CancelLoop();
	return FALSE;
}

void CCJDockContext::EndDragDockBar()
{
	CancelLoop();
	
	if (m_dwOverDockStyle != 0)
	{
		CCJSizeDockBar* pDockBar = GetDockBar(m_dwOverDockStyle);
		ASSERT(pDockBar != NULL);
		
		CRect rect = (m_dwOverDockStyle & CBRS_ORIENT_VERT) ?
			m_rectDragVert : m_rectDragHorz;
		
		UINT uID = _AfxGetDlgCtrlID(pDockBar->m_hWnd);
		m_uMRUDockID = uID;
		m_rectMRUDockPos = rect;
		pDockBar->ScreenToClient(&m_rectMRUDockPos);
		DockSizeBar(m_pBar, pDockBar, &rect);
		m_pDockSite->RecalcLayout();
	}
	
	else if ((m_dwStyle & CBRS_SIZE_DYNAMIC) || (HORZF(m_dwStyle) && !m_bFlip) ||
		(VERTF(m_dwStyle) && m_bFlip))
	{
		m_dwMRUFloatStyle = CBRS_ALIGN_TOP | (m_dwDockStyle & CBRS_FLOAT_MULTI);
		m_ptMRUFloatPos = m_rectFrameDragHorz.TopLeft();
		m_pDockSite->FloatControlBar(m_pBar, m_ptMRUFloatPos, m_dwMRUFloatStyle);
		m_pDockSite->RecalcLayout();
	}
	
	else 
	{
		m_dwMRUFloatStyle = CBRS_ALIGN_LEFT | (m_dwDockStyle & CBRS_FLOAT_MULTI);
		m_ptMRUFloatPos = m_rectFrameDragVert.TopLeft();
		m_pDockSite->FloatControlBar(m_pBar, m_ptMRUFloatPos, m_dwMRUFloatStyle);
		m_pDockSite->RecalcLayout();
	}
}

CCJSizeDockBar* CCJDockContext::GetDockBar(DWORD dwOverDockStyle)
{
	DWORD dw = 0;
	CDockBar* pBar;
	if (HORZF(dwOverDockStyle))
	{
		ASSERT(m_pDockSite->IsKindOf(RUNTIME_CLASS(CFrameWnd)));
		dw = CanDockDockBar(m_rectDragHorz,
			dwOverDockStyle & ~CBRS_ORIENT_VERT, &pBar);
		ASSERT(dw != 0);
		ASSERT(pBar != NULL);
		return (CCJSizeDockBar*)pBar;
	}
	
	if (VERTF(dwOverDockStyle))
	{
		dw = CanDockDockBar(m_rectDragVert,
			dwOverDockStyle & ~CBRS_ORIENT_HORZ, &pBar);
		ASSERT(dw != 0);
		ASSERT(pBar != NULL);
		return (CCJSizeDockBar*)pBar;
	}
	
	return NULL;
}

void CCJDockContext::MoveDockBar(CPoint pt)
{
	CPoint ptOffset = pt - m_ptLast;
	
	m_rectDragHorz.OffsetRect(ptOffset);
	m_rectFrameDragHorz.OffsetRect(ptOffset);
	m_rectDragVert.OffsetRect(ptOffset);
	m_rectFrameDragVert.OffsetRect(ptOffset);
	m_ptLast = pt;
	m_dwOverDockStyle = m_bForceFrame ? 0 : CanDockDockBar();
	DrawFocusRect();
}

void CCJDockContext::DockSizeBar(CControlBar * pBar,CCJSizeDockBar *pDockBar, LPRECT lpRect)
{
	CFrameWnd *pFrame = (CFrameWnd*)m_pDockSite;
	ASSERT_VALID(pFrame);

	ASSERT(pBar != NULL);
	ASSERT(pBar->m_pDockContext != NULL);
	
	if( pDockBar == NULL )
	{
		for (int i = 0; i < 4; i++)
		{
			if ((dwSizeBarMap[i][1] & CBRS_ALIGN_ANY) ==
				(pBar->m_dwStyle & CBRS_ALIGN_ANY))
			{
				pDockBar = (CCJSizeDockBar*)pFrame->GetControlBar(dwSizeBarMap[i][0]);
				ASSERT(pDockBar != NULL);
				break;
			}
		}
	}
	
	ASSERT(pDockBar != NULL);
	ASSERT(pFrame->m_listControlBars.Find(pBar) != NULL);
	ASSERT(pBar->m_pDockSite == pFrame);
	((CCJControlBar*)pBar)->Normalize();
	pDockBar->DockControlBar(pBar, lpRect);
}

void CCJDockContext::ToggleDocking()
{
	if (m_pBar->IsKindOf(RUNTIME_CLASS(CCJControlBar)))
	{
		if (m_pBar->IsFloating())
		{
			// Dock it only if is allowed to be docked
			if (m_pBar->m_dwDockStyle & CBRS_ALIGN_ANY)
			{
				ASSERT((m_uMRUDockID >= AFX_IDW_DOCKBAR_TOP && m_uMRUDockID <= AFX_IDW_SIZEBAR_BOTTOM) ||
					m_uMRUDockID == 0);

				CRect rect = m_rectMRUDockPos;
				CCJSizeDockBar* pDockBar = NULL;
				if (m_uMRUDockID != 0)
				{
					pDockBar = (CCJSizeDockBar*)m_pDockSite->GetControlBar(m_uMRUDockID);
					pDockBar->ClientToScreen(&rect);
				}

				// dock it at the specified position, RecalcLayout will snap
				DockSizeBar(m_pBar, pDockBar, &rect);
				m_pDockSite->RecalcLayout();
			}
		}
		else
		{
			CPoint ptFloat = m_ptMRUFloatPos;
			if (ptFloat.x < 0 || ptFloat.y < 0)
			{
				ptFloat = m_rectMRUDockPos.TopLeft();
				m_pBar->GetParent()->ClientToScreen(&ptFloat);
			}
			m_pDockSite->FloatControlBar(m_pBar, ptFloat, m_dwMRUFloatStyle);
		}
	}
}