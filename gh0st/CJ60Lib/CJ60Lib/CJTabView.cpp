////////////////////////////////////////////////////////////////
// Copyright 1999 Mike Manzo
// Class: CCJTabView
// ==========================================================================  
// HISTORY:	  
// ==========================================================================  
//			1.00	14 Feb 1999	- Initial release.  
// ==========================================================================  
//  
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CJTabView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCJTabView
IMPLEMENT_DYNCREATE(CCJTabView, CCtrlView)

CCJTabView::CCJTabView() : CCtrlView(_T("SysTabControl32"),
	TCS_TOOLTIPS | WS_VISIBLE | WS_CHILD | WS_BORDER)
{
	m_pTabCtrl = (CTabCtrl*) this;
	m_nActiveTab = 0;
}

CCJTabView::~CCJTabView()
{
	while(!m_views.IsEmpty()) {
		CTV_ITEM *pMember=m_views.RemoveHead();
		safe_delete(pMember);
	}
}

BEGIN_MESSAGE_MAP(CCJTabView, CCtrlView)
	//{{AFX_MSG_MAP(CCJTabView)
	ON_WM_CREATE()
	ON_WM_WINDOWPOSCHANGED()
	ON_NOTIFY_REFLECT(TCN_SELCHANGE, OnSelchange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCJTabView message handlers
void CCJTabView::GetChildRect(CRect &rect) 
{
	m_pTabCtrl->GetClientRect(&rect);
}

void CCJTabView::OnInitialUpdate() 
{
	CCtrlView::OnInitialUpdate();
}

CView* CCJTabView::GetActiveView()
{
	return m_pActiveView;
}

CView* CCJTabView::GetView(int nView)
{
	ASSERT_VALID(this);
	ASSERT(nView >= 0);
	
	if (nView!=-1) {
        CTV_ITEM *pMember=m_views.GetAt(m_views.FindIndex(nView));
		return (CView*)pMember->pWnd;
	}
	else
		return NULL;
}

CView* CCJTabView::GetView(CRuntimeClass *pViewClass)
{
	ASSERT_VALID(this);
	ASSERT(pViewClass != NULL);
	ASSERT(pViewClass->IsDerivedFrom(RUNTIME_CLASS(CWnd)));
	ASSERT(AfxIsValidAddress(pViewClass, sizeof(CRuntimeClass), FALSE));
	
	for (POSITION pos=m_views.GetHeadPosition(); pos; m_views.GetNext(pos)) {
		CTV_ITEM *pMember=m_views.GetAt(pos);
		if (pMember->pWnd->IsKindOf(pViewClass)) {
			return (CView*)pMember->pWnd;
		}
    }
	return NULL;
}

BOOL CCJTabView::ModifyTabStyle(DWORD dwRemove, DWORD dwAdd, UINT nFlags)
{
	DWORD dwCurrentStyle = GetStyle();

	// We are in horizontal mode...go vertical
	if ((dwAdd & TCS_VERTICAL) && !(dwCurrentStyle & TCS_VERTICAL)) {
		// Get the log font.
		NONCLIENTMETRICS ncm;
		ncm.cbSize = sizeof(NONCLIENTMETRICS);
		VERIFY(::SystemParametersInfo(SPI_GETNONCLIENTMETRICS,
			sizeof(NONCLIENTMETRICS), &ncm, 0));
		m_TabFont.Detach();
		ncm.lfMessageFont.lfOrientation = (dwAdd & TCS_RIGHT) ? 2700 : 900;
		ncm.lfMessageFont.lfEscapement  = (dwAdd & TCS_RIGHT) ? 2700 : 900;
		m_TabFont.CreateFontIndirect(&ncm.lfMessageFont);
		m_pTabCtrl->SetFont(&m_TabFont);
	}
	// We are in vertical mode....go horizontal
	else if ((dwCurrentStyle & TCS_VERTICAL) && !(dwAdd & TCS_VERTICAL)) {
		// Get the log font.
		NONCLIENTMETRICS ncm;
		ncm.cbSize = sizeof(NONCLIENTMETRICS);
		VERIFY(::SystemParametersInfo(SPI_GETNONCLIENTMETRICS,
			sizeof(NONCLIENTMETRICS), &ncm, 0));
		m_TabFont.Detach();
		ncm.lfMessageFont.lfOrientation = 0;
		ncm.lfMessageFont.lfEscapement  = 0;
		m_TabFont.CreateFontIndirect(&ncm.lfMessageFont);
		m_pTabCtrl->SetFont(&m_TabFont);
	}

	return m_pTabCtrl->ModifyStyle(dwRemove, dwAdd, nFlags);
}

int CCJTabView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CCtrlView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	ASSERT(m_pTabCtrl);			// Should already be constructed
	m_pTabCtrl->SetParent(this);
	m_pTabCtrl->ModifyStyle(0, WM_PARENTNOTIFY);

	// Get the log font.
	NONCLIENTMETRICS ncm;
	ncm.cbSize = sizeof(NONCLIENTMETRICS);
	VERIFY(::SystemParametersInfo(SPI_GETNONCLIENTMETRICS,
		sizeof(NONCLIENTMETRICS), &ncm, 0));
	m_TabFont.CreateFontIndirect(&ncm.lfMessageFont);
	m_pTabCtrl->SetFont(&m_TabFont);
	
	// VC5 Support.
#if _MSC_VER >= 1200
	m_pToolTip = m_pTabCtrl->GetToolTips();
	m_pTabCtrl->SetToolTips(m_pToolTip);
#else
	m_pToolTip = m_pTabCtrl->GetTooltips();
	m_pTabCtrl->SetTooltips(m_pToolTip);
#endif

	return 0;
}

// Determine where the views should be placed in the tab...
void CCJTabView::CalcViewRect(CRect* pRect)
{
	GetChildRect((*pRect));
	DWORD dwStyle = GetStyle();

	if ((dwStyle & TCS_BOTTOM) && !(dwStyle & TCS_VERTICAL)) {		// Bottom
		(*pRect).top	+= TABVIEW_BORDER;
		(*pRect).left	+= TABVIEW_BORDER;
		(*pRect).right	-= TABVIEW_BORDER;
		(*pRect).bottom	-= 26;
	}
	else if ((dwStyle & TCS_RIGHT) && (dwStyle & TCS_VERTICAL)) {	// Right
		(*pRect).top	+= TABVIEW_BORDER;
		(*pRect).left	+= TABVIEW_BORDER;
		(*pRect).right	-= 26;
		(*pRect).bottom	-= TABVIEW_BORDER;
	}
	else if (dwStyle & TCS_VERTICAL) {								// Left
		(*pRect).top	+= TABVIEW_BORDER;
		(*pRect).left	+= 26;
		(*pRect).right	-= TABVIEW_BORDER;
		(*pRect).bottom	-= TABVIEW_BORDER;
	}
	else {															// Top
		(*pRect).top	+= 26;
		(*pRect).left	+= TABVIEW_BORDER;
		(*pRect).right	-= TABVIEW_BORDER;
		(*pRect).bottom	-= TABVIEW_BORDER;
	}
}

void CCJTabView::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
{
	CRect rcChild;
	CWnd *pWnd;

	CalcViewRect(&rcChild);
	for (POSITION pos=m_views.GetHeadPosition(); pos; m_views.GetNext(pos)) {
		pWnd = m_views.GetAt(pos)->pWnd;
		pWnd->MoveWindow(rcChild);
	}
	
	// *** LiangYiBin.Donald
	if( lpwndpos->flags & SWP_HIDEWINDOW ) {
		CFrameWnd* pFrame = GetParentFrame();
		POSITION pos = m_views.GetHeadPosition();
		while ( pos != NULL ) {
			// check whether the views in the controlbar are focused
			CTV_ITEM *pItem = (CTV_ITEM *) m_views.GetNext(pos);
			if ( pFrame != NULL && pFrame->GetActiveView() == pItem->pWnd ) {
				// To avoid mainwindow freezing, we must deativate the view,
				// because it's not visible now.
				pFrame->SetActiveView(NULL);
			}
		}
	}
	
	CCtrlView::OnWindowPosChanged(lpwndpos);
}

void CCJTabView::RemoveView(int nView)
{
	ASSERT_VALID(this);
	ASSERT(nView >= 0);
	
	// remove the page from internal list
	m_views.RemoveAt(m_views.FindIndex(nView));
}

void CCJTabView::SetActiveView(CRuntimeClass *pViewClass)
{
	ASSERT_VALID(this);
	ASSERT(pViewClass != NULL);
	ASSERT(pViewClass->IsDerivedFrom(RUNTIME_CLASS(CWnd)));
	ASSERT(AfxIsValidAddress(pViewClass, sizeof(CRuntimeClass), FALSE));
	
	int nNewTab = 0;
	for (POSITION pos=m_views.GetHeadPosition(); pos; m_views.GetNext(pos))	{
		CTV_ITEM *pMember=m_views.GetAt(pos);
		if (pMember->pWnd->IsKindOf(pViewClass)) {
			//first hide old first view
            m_pActiveView->EnableWindow(FALSE);
            m_pActiveView->ShowWindow(SW_HIDE);
			
			// set new active view
			m_pActiveView = (CView*)pMember->pWnd;
			// enable, show, set focus to new view
			m_pActiveView->EnableWindow(TRUE);
			m_pActiveView->ShowWindow(SW_SHOW);
			m_pActiveView->SetFocus();
			
			// *** LiangYiBin.Donald
			//((CFrameWnd *)GetParent())->SetActiveView(m_pActiveView);
			//GetParentFrame()->SetActiveView(m_pActiveView);
			
			m_nActiveTab = nNewTab;
			// select the tab
			m_pTabCtrl->SetCurSel(m_nActiveTab);
			
			break;
		}
		nNewTab++;
    }
}

void CCJTabView::SetActiveView(int nNewTab)
{
	ASSERT_VALID(this);
	ASSERT(nNewTab >= 0);
	
	if (nNewTab!=-1 && nNewTab!=m_nActiveTab) {
        CTV_ITEM *newMember=m_views.GetAt(m_views.FindIndex(nNewTab));
        CTV_ITEM *oldMember=NULL;
		
        if (m_nActiveTab!=-1) {
            oldMember=m_views.GetAt(m_views.FindIndex(m_nActiveTab));
            oldMember->pWnd->EnableWindow(FALSE);
            oldMember->pWnd->ShowWindow(SW_HIDE);
        }
        newMember->pWnd->EnableWindow(TRUE);
        newMember->pWnd->ShowWindow(SW_SHOW);
        newMember->pWnd->SetFocus();
		
        m_pActiveView = (CView *)newMember->pWnd;
		
        m_nActiveTab = nNewTab;
		// select the tab (if tab programmatically changed)
		m_pTabCtrl->SetCurSel(m_nActiveTab);
    }
}

CImageList* CCJTabView::SetTabImageList(CImageList *pImageList)
{
	return m_pTabCtrl->SetImageList (pImageList);
}

BOOL CCJTabView::AddView(LPCTSTR lpszLabel, CRuntimeClass *pViewClass, CDocument* pDoc/*=NULL*/, CCreateContext *pContext/*=NULL*/)
{	
	
#ifdef _DEBUG
	ASSERT_VALID(this);
	ASSERT(pViewClass != NULL);
	ASSERT(pViewClass->IsDerivedFrom(RUNTIME_CLASS(CWnd)));
	ASSERT(AfxIsValidAddress(pViewClass, sizeof(CRuntimeClass), FALSE));
#endif
	
	CCreateContext context;
	if (pContext == NULL) {
		// *** LiangYiBin.Donald
		context.m_pCurrentDoc		= pDoc;
		context.m_pCurrentFrame		= GetParentFrame();
		context.m_pLastView			= NULL;
		context.m_pNewDocTemplate	= NULL;
		context.m_pNewViewClass		= pViewClass;
		pContext = &context;
	}
	
	CWnd* pWnd;
	TRY	{
		pWnd = (CWnd*)pViewClass->CreateObject();
		if (pWnd == NULL)
			AfxThrowMemoryException();
	}
	CATCH_ALL(e) {
		TRACE0(_T("Out of memory creating a view.\n"));
		// Note: DELETE_EXCEPTION(e) not required
		return FALSE;
	}
	END_CATCH_ALL
		
	ASSERT_KINDOF(CWnd, pWnd);
	ASSERT(pWnd->m_hWnd == NULL);       // not yet created
	
	DWORD dwStyle = AFX_WS_DEFAULT_VIEW;
	CRect rect;

	// Create with the right size and position
	if (!pWnd->Create(NULL, NULL, dwStyle, rect, this, 0, pContext)) {
		TRACE0(_T("Warning: couldn't create client pane for view.\n"));
		// pWnd will be cleaned up by PostNcDestroy
		return FALSE;
	}
	m_pActiveView = (CView*) pWnd;
	
	CTV_ITEM *pMember=new CTV_ITEM;
	pMember->pWnd=pWnd;
	_tcscpy(pMember->szLabel, lpszLabel);
	m_views.AddTail(pMember);
	
	// ToolTip support for tabs.
	if((m_views.GetCount()-1)==0) {
		m_pToolTip->AddTool( m_pTabCtrl, lpszLabel,
			NULL, m_views.GetCount()-1 );
	}
	else {
		m_pToolTip->AddTool( m_pTabCtrl, lpszLabel,
			CRect(0,0,0,0), m_views.GetCount()-1 );
	}
	
	int nViews = m_views.GetCount();
	if (nViews != 1) {
		pWnd->EnableWindow(FALSE);
		pWnd->ShowWindow(SW_HIDE);
	}
	
	TC_ITEM tci;
	tci.mask = TCIF_TEXT | TCIF_IMAGE;
	tci.pszText = (LPTSTR)(LPCTSTR)lpszLabel;
	tci.iImage = nViews-1;
	m_pTabCtrl->InsertItem(nViews, &tci);

	CRect rcChild;
	CalcViewRect(&rcChild);
	m_pActiveView->MoveWindow(rcChild);

	return TRUE;
}

// Used for tab change notifications
void CCJTabView::OnSelchange(NMHDR* pNMHDR, LRESULT* pResult) 
{
	SetActiveView(m_pTabCtrl->GetCurSel());
	
	*pResult = 0;
}
