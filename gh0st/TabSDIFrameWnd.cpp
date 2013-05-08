/**********************************************************************
**
**	TabSDIFrameWnd.cpp : implementation file of CTabSDIFrameWnd class
**
**	by Andrzej Markowski July 2005
**
**********************************************************************/

#include "stdafx.h"
#include "TabSDIFrameWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabSDIFrameWnd

IMPLEMENT_DYNCREATE(CTabSDIFrameWnd, CFrameWnd)

CTabSDIFrameWnd::CTabSDIFrameWnd()
{
}

CTabSDIFrameWnd::~CTabSDIFrameWnd()
{
}


BEGIN_MESSAGE_MAP(CTabSDIFrameWnd, CFrameWnd)
	//{{AFX_MSG_MAP(CTabSDIFrameWnd)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_NOTIFY(CTCN_SELCHANGE, IDC_TABCTRL, OnSelchangeTabctrl)
	ON_NOTIFY(CTCN_CLICK, IDC_TABCTRL, OnClickTabctrl)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabSDIFrameWnd message handlers

void CTabSDIFrameWnd::OnSelchangeTabctrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	SetCurView(((CTC_NMHDR*)pNMHDR)->nItem);
	*pResult = 0;
}

void CTabSDIFrameWnd::OnClickTabctrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if(((CTC_NMHDR*)pNMHDR)->nItem==CTCHT_ONCLOSEBUTTON)
		DeleteActiveView();
	*pResult = 0;
}

BOOL CTabSDIFrameWnd::AddView(CString sLabel, CView *pView, CString sTooltip)
{
	CDocument* pDoc = GetActiveDocument();
	CString sTitle = pDoc->GetTitle();
	int nOldSel = m_wndTab.GetCurSel();
	int nCurSel = m_wndTab.InsertItem(m_wndTab.GetItemCount(),sLabel,(DWORD)pView);
	m_wndTab.SetCurSel(nCurSel);
	m_wndTab.SetItemTooltipText(nCurSel,sTooltip);

	if(nOldSel>=0)
	{
		DWORD pViewRemove;
		m_wndTab.GetItemData(nOldSel,pViewRemove);

		pView->SetDlgCtrlID(AFX_IDW_PANE_FIRST);
		((CView*)pViewRemove)->SetDlgCtrlID(AFX_IDW_PANE_FIRST+1);

		pView->ShowWindow(SW_SHOW);
		((CView*)pViewRemove)->ShowWindow(SW_HIDE);

		pDoc->AddView(pView);
		pDoc->RemoveView((CView*)pViewRemove);

		SetActiveView(pView);
		RecalcLayout();
		pDoc->SetTitle(sTitle);
	}
	return TRUE;
}

BOOL CTabSDIFrameWnd::DeleteActiveView()
{
	if(m_wndTab.GetItemCount()<2)
		return FALSE;

	CDocument* pDoc = GetActiveDocument();
	CString sTitle = pDoc->GetTitle();
	CView* pViewRemove = GetActiveView();

	for(int i=0; i<m_wndTab.GetItemCount(); i++)
	{
		DWORD dwData;
		m_wndTab.GetItemData(i,dwData);
		if(dwData==(DWORD)pViewRemove)
		{
			m_wndTab.DeleteItem(i);
			m_wndTab.GetItemData(m_wndTab.GetCurSel(),dwData);

			CView* pView = (CView*)dwData;
			pView->SetDlgCtrlID(AFX_IDW_PANE_FIRST);
			pViewRemove->SetDlgCtrlID(AFX_IDW_PANE_FIRST+1);

			pView->ShowWindow(SW_SHOW);
			pViewRemove->ShowWindow(SW_HIDE);

			pDoc->AddView(pView);
			pDoc->RemoveView(pViewRemove);
			
			SetActiveView(pView);
			pViewRemove->DestroyWindow();
			RecalcLayout();
			
			pDoc->SetTitle(sTitle);
			return TRUE;
		}
	}
	return FALSE;
}

void CTabSDIFrameWnd::DeleteContents()
{
	for(int i=0; i<m_wndTab.GetItemCount(); i++)
	{
		DWORD dwView;
		m_wndTab.GetItemData(i,dwView);
		if(GetActiveView()!=(CView*)dwView)
			((CView*)dwView)->DestroyWindow();
	}
	m_wndTab.DeleteAllItems();
}

BOOL CTabSDIFrameWnd::SetCurView(int nNdx)
{
	if(nNdx>=0 && nNdx<m_wndTab.GetItemCount())
	{
		if(nNdx!=m_wndTab.GetCurSel())
			m_wndTab.SetCurSel(nNdx);
		DWORD dwData;
		m_wndTab.GetItemData(nNdx,dwData);
		
		CDocument* pDoc = GetActiveDocument();
		CString sTitle = pDoc->GetTitle();
		CView* pView = (CView*)dwData;
		CView* pViewRemove = GetActiveView();

		if(pView==pViewRemove)
		{
			pView->ShowWindow(SW_HIDE);
			pView->ShowWindow(SW_SHOW);
			return TRUE;
		}
		pView->SetDlgCtrlID(AFX_IDW_PANE_FIRST);
		pViewRemove->SetDlgCtrlID(AFX_IDW_PANE_FIRST+1);

		pView->ShowWindow(SW_SHOW);
		pViewRemove->ShowWindow(SW_HIDE);

		pDoc->AddView(pView);
		pDoc->RemoveView(pViewRemove);

		SetActiveView(pView);
		RecalcLayout();
		pDoc->SetTitle(sTitle);
		return TRUE;
	}
	return FALSE;
}
