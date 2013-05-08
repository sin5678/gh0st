// CJTabCtrlBar.cpp : implementation file
//
// DevStudio Style Resizable Docking Tab Control Bar.
//
// The code contained in this file is based on the original
// CSizingTabCtrlBar class written by Dirk Clemens,
//		mailto:dirk_clemens@hotmail.com
//		http://www.codeguru.com/docking/sizing_tabctrl_bar.shtml
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
//	<>  To Dirk Clemens (dirk_clemens@hotmail.com) for his CSizingTabCtrlBar
//		class, which is where the idea for this came from.
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
//          1.01    03 Jan 1999 - Application freezing bug fixed
//                                by LiangYiBin.Donald(mailto:lybd@yahoo.com)
//			1.02	17 Jan 1999 - Added helper class CCJTabCtrl to eliminate
//								  re-painting problems such as when the app
//								  is minimized then restored.
//			1.03    31 Jan 1999 - Commented out 2 calls to SetActiveView(). This
//								  somewhat fixes the problem with CTRL+TAB in
//								  activatation of MDI child windows, however
//								  if you click inside the view, this breaks 
//							      CTRL+TAB again, will look at this when I 
//								  get some free time. Thank to Pierre Guimont
//								  (Pierre_Guimont@mitel.com) for pointing this out.
//			1.04    31 Jan 1999 - Added m_pTabCtrl->GetRowCount() as per 
//								  Rashid Thadha (rashidt@consort.co.uk) request.
//			1.05	24 Feb 1999 - Phil Baxter [pgb@mrjinx.demon.co.uk] - Fixed the
//								  following problems when tabs are added after
//								  creation:
//								  1.	The tab was created, but the view that
//										was showing on the tab was not visible.
//								  2.	Get an assertion when the AddView function 
//										attempts to perform the AddTool function.
//								  3.	When a new view is added, although you set it 
//										to be the active view, it isn't raised to the 
//										top of the stack in the display.
//								  4.	The Tab name as stored in the TCB_ITEM structure 
//										is now a CString, this removes the 32 byte limit 
//										that it previously had.
//								  5.	New methods for obtaining the name as stored in 
//										the TCB_ITEM structure.
//								  6.	New Methods for deleting a view from the control, 
//										this removes the view entirely including its 
//										associated tab.
//								  7.	Some minor fixes around assertions when trying 
//										to manipulate the control when it has not had 
//										any tabs created.
//								  8.    Added two new member functions:
//										void SetViewToolTip(int nView, LPCTSTR lpszLabel);
//										void SetViewToolTip(CRuntimeClass *pViewClass, LPCTSTR lpszLabel);
//			1.06	14 Mar 1999 - Gonzalo Pereyra [ persys@adinet.com.uy ] -
//								  I wanted to trap NM_RETURN notifications in my 
//								  CTreeView Class. I couldn't... It turns out that 
//								  CControlbar::PreTranslateMessage() will eat all dialog 
//								  messages...It will never get dispatched to my window. 
//								  So i Overrided it in : CCJTabCtrlBar::PreTranslateMessage().
//								  this will allow for message reflection and we will get 
//								  NM_RETURN. I don't know if this a good solution but for 
//								  now , it works
// ==========================================================================
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CJTabCtrlBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCJTabCtrl - helper class fixes problems with repainting.

class CCJTabCtrl : public CTabCtrl
{
protected:
	//{{AFX_MSG(CCJTabCtrl)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CCJTabCtrl, CTabCtrl)
	//{{AFX_MSG_MAP(CCJTabCtrl)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CCJTabCtrl::OnEraseBkgnd(CDC* pDC) 
{
	CRect rcChild;
	GetClientRect(&rcChild);
	rcChild.DeflateRect(3,3);
	rcChild.bottom -= 21;
	pDC->ExcludeClipRect(rcChild);
	return CTabCtrl::OnEraseBkgnd(pDC);
}

/////////////////////////////////////////////////////////////////////////////
// CCJTabCtrlBar

CCJTabCtrlBar::CCJTabCtrlBar()
{
	m_nActiveTab = 0;
    m_nNextToolTipId = 0;

	m_pTabCtrl = NULL;
}

CCJTabCtrlBar::~CCJTabCtrlBar()
{
	safe_delete(m_pTabCtrl);
	while(!m_views.IsEmpty()) {
		TCB_ITEM *pMember=m_views.RemoveHead();
		safe_delete(pMember);
	}
}

#define IDC_TABCTRLBAR 1000

BEGIN_MESSAGE_MAP(CCJTabCtrlBar, CCJControlBar)
	//{{AFX_MSG_MAP(CCJTabCtrlBar)
	ON_WM_CREATE()
	ON_WM_WINDOWPOSCHANGED()
	//}}AFX_MSG_MAP
	ON_NOTIFY(TCN_SELCHANGE, IDC_TABCTRLBAR, OnTabSelChange)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCJTabCtrlBar message handlers

void CCJTabCtrlBar::GetChildRect(CRect &rect) 
{
	CCJControlBar::GetChildRect(rect);

	if (IsFloating()) {
		rect.DeflateRect(2,2);
	}
}

void CCJTabCtrlBar::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
{
	CRect rcChild;
	GetChildRect(rcChild);
	rcChild.DeflateRect(3,3);
	rcChild.bottom -= (21 * m_pTabCtrl->GetRowCount());

	CWnd *pWnd;

	for (POSITION pos=m_views.GetHeadPosition(); pos; m_views.GetNext(pos))
	{
		pWnd = m_views.GetAt(pos)->pWnd;
		pWnd->MoveWindow(rcChild);
	}
	
	// *** LiangYiBin.Donald
	if( lpwndpos->flags & SWP_HIDEWINDOW )
	{
		CFrameWnd* pFrame = GetParentFrame();
		POSITION pos=m_views.GetHeadPosition();
		while ( pos != NULL )
		{
			// check whether the views in the controlbar are focused
			TCB_ITEM *pItem = (TCB_ITEM *) m_views.GetNext(pos);
			if ( pFrame != NULL && pFrame->GetActiveView() == pItem->pWnd )
			{
				// To avoid mainwindow freezing, we must deativate the view,
				// because it's not visible now.
				pFrame->SetActiveView(NULL);
			}
		}
	}
	
	CCJControlBar::OnWindowPosChanged(lpwndpos);
}

CImageList* CCJTabCtrlBar::SetTabImageList(CImageList *pImageList)
{
	return m_pTabCtrl->SetImageList (pImageList);
}

BOOL CCJTabCtrlBar::ModifyTabStyle(DWORD dwRemove, DWORD dwAdd, UINT nFlags)
{
	return m_pTabCtrl->ModifyStyle(dwRemove, dwAdd, nFlags);
}

int CCJTabCtrlBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CCJControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	ModifyStyle(WS_CLIPCHILDREN, 0);

	m_pTabCtrl = new CCJTabCtrl;
	ASSERT(m_pTabCtrl);

	//Create the Tab Control
	if (!m_pTabCtrl->Create(WS_VISIBLE|WS_CHILD|TCS_BOTTOM|TCS_TOOLTIPS, 
		CRect(0,0,0,0), this, IDC_TABCTRLBAR))
	{
		TRACE0(_T("Unable to create tab control bar\n"));
		return -1;
	}
	
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

	SetChild(m_pTabCtrl);
	return 0;
}

void CCJTabCtrlBar::OnTabSelChange(NMHDR* pNMHDR, LRESULT* pResult)
{
	SetActiveView(m_pTabCtrl->GetCurSel());
}

//////////////////////////////////////////////////
// The remainder of this class was written by Dirk Clemens...

BOOL CCJTabCtrlBar::AddView(LPCTSTR lpszLabel, CRuntimeClass *pViewClass, CCreateContext *pContext)
{	
	
#ifdef _DEBUG
	ASSERT_VALID(this);
	ASSERT(pViewClass != NULL);
	ASSERT(pViewClass->IsDerivedFrom(RUNTIME_CLASS(CWnd)));
	ASSERT(AfxIsValidAddress(pViewClass, sizeof(CRuntimeClass), FALSE));
#endif
	
	CCreateContext context;
	if (pContext == NULL)
	{
		// *** LiangYiBin.Donald
		context.m_pCurrentDoc = NULL;
		context.m_pCurrentFrame = GetParentFrame();
		context.m_pLastView = NULL;
		context.m_pNewDocTemplate = NULL;
		context.m_pNewViewClass = pViewClass;
		pContext = &context;
	}
	
	CWnd* pWnd;
	TRY
	{
		pWnd = (CWnd*)pViewClass->CreateObject();
		if (pWnd == NULL)
			AfxThrowMemoryException();
	}
	CATCH_ALL(e)
	{
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
	if (!pWnd->Create(NULL, NULL, dwStyle, rect, this, 0, pContext))
	{
		TRACE0(_T("Warning: couldn't create client pane for view.\n"));
		// pWnd will be cleaned up by PostNcDestroy
		return FALSE;
	}
	m_pActiveView = (CView*) pWnd;
	
	TCB_ITEM *pMember=new TCB_ITEM;
	pMember->pWnd=pWnd;
	pMember->szTabLabel = lpszLabel;
	pMember->szToolTipLabel = lpszLabel;
	m_views.AddTail(pMember);
	
	// ToolTip support for tabs.

    // ****************************
    // *** Phil Baxter - Edit Start
    //
    // Obtain the tooltip control here rather than using the value
    // obtained when the tab control was created and stored in m_pToolTip.
    //
    // The reason for this is that doing it the original way causes an
    // assertion when the tab control has a view added dynamically (i.e.
    // in response to a user choosing to add one during the program execution,
    // rather than the view being added at program initialisation time). It
    // would appear that the original code ended up with the m_pToolTip variable
    // pointing to the correct tooltip control, but, the tooltip had an invalid
    // m_hWnd member variable!!

    CToolTipCtrl* pToolTip = NULL;

	// VC5 Support.
#if _MSC_VER >= 1200
	pToolTip = m_pTabCtrl->GetToolTips();
#else
	pToolTip = m_pTabCtrl->GetTooltips();
#endif

    pMember->uiToolTipId = m_nNextToolTipId++;
    if (pToolTip!=NULL)
    {
	    if(pMember->uiToolTipId==0) {
		    pToolTip->AddTool( m_pTabCtrl, lpszLabel,
			    NULL, pMember->uiToolTipId );
	    }
	    else {
		    pToolTip->AddTool( m_pTabCtrl, lpszLabel,
			    CRect(0,0,0,0), pMember->uiToolTipId );
	    }

        // Now, because tooltips work in a wierd way, if we have added
        // tooltips before and then removed them until none were left,
        // then they will get out of sync with the tabs on the tab control,
        // so, the best way to overcome this that I have found so far is
        // to re-do the tooltip text values here.

        int index = 0;
        POSITION pos;
	    for (pos=m_views.GetHeadPosition(); pos; m_views.GetNext(pos))
	    {
		    TCB_ITEM *pMember=m_views.GetAt(pos);

            // Update the tooltip for the view

            pMember->uiToolTipId = index;
            pToolTip->UpdateTipText( pMember->szToolTipLabel, m_pTabCtrl, 
			        pMember->uiToolTipId );

            index++;
        }
        m_nNextToolTipId=index;
    }
    	
    // *** Phil Baxter - Edit End
    // **************************	

	int nViews = m_views.GetCount();
	if (nViews!=1)
	{
		pWnd->EnableWindow(FALSE);
		pWnd->ShowWindow(SW_HIDE);
	}
//	else
//	{
//		// *** LiangYiBin.Donald
//		//((CFrameWnd *)GetParent())->SetActiveView((CView *)m_pActiveView);
//		GetParentFrame()->SetActiveView(m_pActiveView);
//	}
	
	TC_ITEM tci;
	tci.mask = TCIF_TEXT | TCIF_IMAGE;
	tci.pszText = (LPTSTR)(LPCTSTR)lpszLabel;
	tci.iImage = nViews-1;
	m_pTabCtrl->InsertItem(nViews, &tci);

    // ****************************
    // *** Phil Baxter - Edit Start
    //
    // Make the new view sized correctly for the current tab control bar size. This
    // ensures that it is visible when created!
    // This is taken from the beginning of CCJTabCtrlBar::OnWindowPosChanged(). I
    // suppose, it should be put in a function of its own so that both routines can
    // use it from a common source.

	CRect rcChild;
	GetChildRect(rcChild);
	rcChild.DeflateRect(3,3);
	rcChild.bottom -= (21 * m_pTabCtrl->GetRowCount());

	pWnd->MoveWindow(rcChild);

    // Now make the newly added tab the currently selected one. This is probably
    // personal preference, I think it is more logical this way.

    SetActiveView(nViews - 1);

    // *** Phil Baxter - Edit End
    // **************************	
	
	return TRUE;
}

void CCJTabCtrlBar::RemoveView(int nView)
{
	ASSERT_VALID(this);
	ASSERT(nView >= 0);
	
    // ****************************
    // *** Phil Baxter - Edit Start
    //
    // FindIndex will return NULL if the new tab position
    // doesnt exist! such as when the list is empty. Passing
    // NULL to GetAt causes an assertion, so, this mod gets
    // the position and only if it is not NULL does it carry
    // on processing.

    POSITION pos=m_views.FindIndex(nView);

    if (pos)
    {
	    // remove the page from internal list
	    m_views.RemoveAt(pos);
    }

    // *** Phil Baxter - Edit End
    // **************************
}

void CCJTabCtrlBar::SetActiveView(int nNewTab)
{
	ASSERT_VALID(this);
	ASSERT(nNewTab >= 0);
	
	if (nNewTab!=-1 && nNewTab!=m_nActiveTab)
	{
        // ****************************
        // *** Phil Baxter - Edit Start
        //
        // FindIndex will return NULL if the new tab position
        // doesnt exist! such as when the list is empty. Passing
        // NULL to GetAt causes an assertion, so, this mod gets
        // the position and only if it is not NULL does it carry
        // on processing.

        TCB_ITEM *newMember=NULL;
        TCB_ITEM *oldMember=NULL;
        POSITION pos=m_views.FindIndex(nNewTab);
        
        if (pos)
        {
            newMember=m_views.GetAt(pos);

            if (m_nActiveTab!=-1)
            {
                POSITION oldPos=m_views.FindIndex(m_nActiveTab);
                if (oldPos)
                {
                    oldMember=m_views.GetAt(oldPos);
                    oldMember->pWnd->EnableWindow(FALSE);
                    oldMember->pWnd->ShowWindow(SW_HIDE);
                }
            }
            newMember->pWnd->EnableWindow(TRUE);
            newMember->pWnd->ShowWindow(SW_SHOW);
            newMember->pWnd->SetFocus();
		    
            m_pActiveView = (CView *)newMember->pWnd;
		    // *** LiangYiBin.Donald
		    //((CFrameWnd *)GetParent())->SetActiveView(m_pActiveView);
//		    GetParentFrame()->SetActiveView(m_pActiveView);
		    
            m_nActiveTab = nNewTab;
		    // select the tab (if tab programmatically changed)
		    m_pTabCtrl->SetCurSel(m_nActiveTab);
        }
        // *** Phil Baxter - Edit End
        // **************************
    }
}

void CCJTabCtrlBar::SetActiveView(CRuntimeClass *pViewClass)
{
	ASSERT_VALID(this);
	ASSERT(pViewClass != NULL);
	ASSERT(pViewClass->IsDerivedFrom(RUNTIME_CLASS(CWnd)));
	ASSERT(AfxIsValidAddress(pViewClass, sizeof(CRuntimeClass), FALSE));
	
	int nNewTab = 0;
	for (POSITION pos=m_views.GetHeadPosition(); pos; m_views.GetNext(pos))
	{
		TCB_ITEM *pMember=m_views.GetAt(pos);
		if (pMember->pWnd->IsKindOf(pViewClass))
		{
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

CView* CCJTabCtrlBar::GetActiveView()
{
	return m_pActiveView;
}

CView* CCJTabCtrlBar::GetView(int nView)
{
	ASSERT_VALID(this);
	ASSERT(nView >= 0);
	
	if (nView!=-1)
	{
        POSITION pos=m_views.FindIndex(nView);

        if (pos)
        {
            TCB_ITEM *pMember=m_views.GetAt(pos);
            if (pMember != NULL)
        		return (CView*)pMember->pWnd;
            else
                return NULL;
        }
        else
            return NULL;
	}
	else
		return NULL;
}

CView* CCJTabCtrlBar::GetView(CRuntimeClass *pViewClass)
{
	ASSERT_VALID(this);
	ASSERT(pViewClass != NULL);
	ASSERT(pViewClass->IsDerivedFrom(RUNTIME_CLASS(CWnd)));
	ASSERT(AfxIsValidAddress(pViewClass, sizeof(CRuntimeClass), FALSE));
	
	for (POSITION pos=m_views.GetHeadPosition(); pos; m_views.GetNext(pos))
	{
		TCB_ITEM *pMember=m_views.GetAt(pos);
		if (pMember->pWnd->IsKindOf(pViewClass))
		{
			return (CView*)pMember->pWnd;
		}
    }
	return NULL;
}

LPCTSTR CCJTabCtrlBar::GetViewName(int nView)
{
	ASSERT_VALID(this);
	ASSERT(nView >= 0);
	
	if (nView!=-1)
	{
        POSITION pos=m_views.FindIndex(nView);

        if (pos)
        {
            TCB_ITEM *pMember=m_views.GetAt(pos);
            if (pMember != NULL)
        		return (LPCTSTR) pMember->szTabLabel;
            else
                return NULL;
        }
        else
            return NULL;
	}
	else
		return NULL;
}

LPCTSTR CCJTabCtrlBar::GetViewName(CRuntimeClass *pViewClass)
{
	ASSERT_VALID(this);
	ASSERT(pViewClass != NULL);
	ASSERT(pViewClass->IsDerivedFrom(RUNTIME_CLASS(CWnd)));
	ASSERT(AfxIsValidAddress(pViewClass, sizeof(CRuntimeClass), FALSE));
	
	for (POSITION pos=m_views.GetHeadPosition(); pos; m_views.GetNext(pos))
	{
		TCB_ITEM *pMember=m_views.GetAt(pos);
		if (pMember->pWnd->IsKindOf(pViewClass))
		{
    		return (LPCTSTR) pMember->szTabLabel;
		}
    }
	return NULL;
}

void CCJTabCtrlBar::DeleteView(int nView)
{
	ASSERT_VALID(this);
	ASSERT(nView >= 0);

    POSITION pos;

    // Delete all the tooltips from the view, we
    // will re-add them as we iterate through the list of views
	
    // VC5 Support.
#if _MSC_VER >= 1200
    CToolTipCtrl* pToolTip = m_pTabCtrl->GetToolTips();
#else
    CToolTipCtrl* pToolTip = m_pTabCtrl->GetTooltips();
#endif

    // Now find the view we want to delete and remove it

    pos=m_views.FindIndex(nView);
    if (pos)
    {
        TCB_ITEM *pMember=NULL;

	    // remove the page from internal list
        pMember = m_views.GetAt(pos);
	    m_views.RemoveAt(pos);

        // And delete the member window, freeing our stored
        // values relating to it
        if (pMember->pWnd != NULL)
        {
            pMember->pWnd->DestroyWindow();
            pMember->pWnd = NULL;
        }

        delete pMember;

        // remove it from the tab control
  	    m_pTabCtrl->DeleteItem(nView);

        // Finally, if we have just deleted the active view, reset the
        // active tab to be the first view in the list

        if (nView == m_nActiveTab)
        {
            m_nActiveTab = -1;
            SetActiveView(0);
        }
        else
        {
            SetActiveView(m_nActiveTab - 1);
        }
    }

    // Reset the tooltips for the views we have left...

    if (pToolTip != NULL)
    {
        int index = 0;
	    for (pos=m_views.GetHeadPosition(); pos; m_views.GetNext(pos))
	    {
		    TCB_ITEM *pMember=m_views.GetAt(pos);

            // Update the tooltip for the view

            pMember->uiToolTipId = index;
            pToolTip->UpdateTipText( pMember->szToolTipLabel, m_pTabCtrl, 
			        pMember->uiToolTipId );

            index++;
        }

        m_nNextToolTipId=index;
        if (m_nNextToolTipId == 0)
            m_nNextToolTipId = 1;
    }

    return;
}

void CCJTabCtrlBar::DeleteView(CRuntimeClass *pViewClass)
{
	ASSERT_VALID(this);
	ASSERT(pViewClass != NULL);
	ASSERT(pViewClass->IsDerivedFrom(RUNTIME_CLASS(CWnd)));
	ASSERT(AfxIsValidAddress(pViewClass, sizeof(CRuntimeClass), FALSE));
	
    INT index=0;
	for (POSITION pos=m_views.GetHeadPosition(); pos; m_views.GetNext(pos))
	{
		TCB_ITEM *pMember=m_views.GetAt(pos);
		if (pMember->pWnd->IsKindOf(pViewClass))
		{
            DeleteView(index);
            break;
		}
        index++;
    }
	return;
}

void CCJTabCtrlBar::SetViewToolTip(int nView, LPCTSTR lpszLabel)
{
	ASSERT_VALID(this);
	ASSERT(nView >= 0);

    // Delete all the tooltips from the view, we
    // will re-add them as we iterate through the list of views
	
    // VC5 Support.
#if _MSC_VER >= 1200
    CToolTipCtrl* pToolTip = m_pTabCtrl->GetToolTips();
#else
    CToolTipCtrl* pToolTip = m_pTabCtrl->GetTooltips();
#endif

    if (pToolTip!=NULL)
    {
        POSITION pos;

        // Now find the view we want to change the tooltip for and
        // reset its tooltip text

        pos=m_views.FindIndex(nView);
        if (pos)
        {
            TCB_ITEM *pMember=NULL;

	        // Get the private data for the view
            pMember = m_views.GetAt(pos);

            // Reset the tooltip text to the new value
            if (lpszLabel != NULL)
                pMember->szToolTipLabel=lpszLabel;
            else
                pMember->szToolTipLabel.Empty();

            // Update the tooltip for the view

            pToolTip->UpdateTipText( pMember->szToolTipLabel, m_pTabCtrl, 
			        pMember->uiToolTipId );
        }
    }

    return;
}

void CCJTabCtrlBar::SetViewToolTip(CRuntimeClass *pViewClass, LPCTSTR lpszLabel)
{
	ASSERT_VALID(this);
	ASSERT(pViewClass != NULL);
	ASSERT(pViewClass->IsDerivedFrom(RUNTIME_CLASS(CWnd)));
	ASSERT(AfxIsValidAddress(pViewClass, sizeof(CRuntimeClass), FALSE));
	
    INT index=0;
	for (POSITION pos=m_views.GetHeadPosition(); pos; m_views.GetNext(pos))
	{
		TCB_ITEM *pMember=m_views.GetAt(pos);
		if (pMember->pWnd->IsKindOf(pViewClass))
		{
            SetViewToolTip(index, lpszLabel);
            break;
		}
        index++;
    }
	return;
}

BOOL CCJTabCtrlBar::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN && (int)pMsg->wParam == VK_RETURN) 
	{ 
		m_pActiveView->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam); 
		return true; 
	} 
	
	return CCJControlBar::PreTranslateMessage(pMsg);
}
