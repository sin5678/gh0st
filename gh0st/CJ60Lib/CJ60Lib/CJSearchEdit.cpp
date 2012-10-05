// CJSearchEdit.cpp : implementation file
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
// ==========================================================================  
//  
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CJSearchEdit.h"
#include "SHFileInfo.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditButton class
/////////////////////////////////////////////////////////////////////////////

CEditButton::CEditButton()
{
	NONCLIENTMETRICS ncm;
	ncm.cbSize = sizeof(NONCLIENTMETRICS);
	VERIFY(::SystemParametersInfo(SPI_GETNONCLIENTMETRICS,
		sizeof(NONCLIENTMETRICS), &ncm, 0));
	m_Font.CreateFontIndirect(&ncm.lfMessageFont);
}

CEditButton::~CEditButton()
{
}

BEGIN_MESSAGE_MAP(CEditButton, CButton)
	ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
END_MESSAGE_MAP()

BOOL CEditButton::Create(CWnd* pWnd, SEARCH_TYPE eType, UINT nMenu)
{
	ASSERT(eType != -1);
	ASSERT_VALID(pWnd);
	m_eType = eType;
	m_nMenu = nMenu;
	m_pWnd = pWnd;
	CWnd* pWndParent = m_pWnd->GetParent();
	ASSERT_VALID(pWndParent);
	
	CRect rc;
	m_pWnd->GetWindowRect(&rc);
	m_pWnd->SetWindowPos(NULL, 0, 0, rc.Width()-23,
		rc.Height(), SWP_NOZORDER|SWP_NOMOVE);
	pWndParent->ScreenToClient(&rc);
	rc.left = rc.right-18;

	DWORD dwStyle = WS_VISIBLE|WS_CHILD|WS_TABSTOP|BS_CENTER|BS_VCENTER;
	switch (m_eType)
	{
	case SEARCH_DIRECTORY:
	case SEARCH_FILE:
		{
			if (CButton::Create(_T("..."), dwStyle, rc,
				pWndParent, GetNextID(pWndParent)))
			{
				SetWindowPos(m_pWnd, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE);
				EnableWindow(m_pWnd->IsWindowEnabled());
				SetFont(&m_Font);
				return TRUE;
			}
		}

	case SEARCH_POPUP:
		{
			if (CButton::Create(_T("..."), dwStyle|BS_ICON, rc,
				pWndParent, GetNextID(pWndParent)))
			{
				SetWindowPos(m_pWnd, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE);
				EnableWindow(m_pWnd->IsWindowEnabled());
				SetFont(&m_Font);

				CImageList imageList;
				CBitmap    bitmap;

				bitmap.LoadBitmap(IDB_BTN_ARROW);
				imageList.Create(15, 17, ILC_COLORDDB|ILC_MASK, 1, 1);
				imageList.Add(&bitmap, RGB(255,0,255));

				SetIcon(imageList.ExtractIcon(0));

				imageList.Detach();
				bitmap.Detach();
				return TRUE;
			}
		}
	}

	return FALSE;
}

void CEditButton::OnClicked()
{
	switch (m_eType)
	{
	case SEARCH_DIRECTORY:
		{
			CSHFileInfo sfi;
			sfi.m_strTitle = _T("Select Folder Location:");
			if (sfi.BrowseForFolder(GetParent()) == IDOK) {
				m_pWnd->SetWindowText(sfi.m_strPath);
			}
		}
		break;

	case SEARCH_FILE:
		{
			static char BASED_CODE szFilter[] = _T("All Files (*.*)|*.*||");
			CFileDialog dlg(TRUE, _T("*.*"), NULL, OFN_HIDEREADONLY, szFilter, GetParent());
			if (dlg.DoModal() == IDOK) {
				m_pWnd->SetWindowText(dlg.GetPathName());
			}
		}
		break;

	case SEARCH_POPUP:
		{
			ASSERT(m_nMenu!=-1);
			SetState(TRUE);

			CRect rc;
			GetWindowRect(&rc);
			CWnd* pParentWnd = m_pWnd->GetParent();

			CMenu menu;
			VERIFY(menu.LoadMenu(m_nMenu));

			CMenu* pPopup = menu.GetSubMenu(0);
			ASSERT(pPopup != NULL);
			CWnd* pWndPopupOwner = this;

			while (pWndPopupOwner->GetStyle() & WS_CHILD)
				pWndPopupOwner = pWndPopupOwner->GetParent();

			pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL,
				rc.right, rc.top, pParentWnd, &rc);

			// Return the button state to normal.
			SetState(FALSE);
		}
		break;
	}
}

UINT CEditButton::GetNextID(CWnd* pWnd) const
{
	for (UINT nID = 32767; nID != 1; --nID)
		if(!pWnd->GetDlgItem(nID))
			return nID;
		return -1;
}
/////////////////////////////////////////////////////////////////////////////
// CCJSearchEdit

CCJSearchEdit::CCJSearchEdit()
{
	m_eType = SEARCH_DIRECTORY;
	m_nMenu = -1;
}

CCJSearchEdit::~CCJSearchEdit()
{
}

BEGIN_MESSAGE_MAP(CCJSearchEdit, CEdit)
	//{{AFX_MSG_MAP(CCJSearchEdit)
	ON_WM_ENABLE()
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCJSearchEdit message handlers

void CCJSearchEdit::OnEnable(BOOL bEnable) 
{
	CEdit::OnEnable(bEnable);
	m_button.EnableWindow(bEnable);
}

void CCJSearchEdit::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CEdit::OnShowWindow(bShow, nStatus);
	m_button.ShowWindow(bShow?SW_SHOW:SW_HIDE);
}

void CCJSearchEdit::PreSubclassWindow() 
{
	CEdit::PreSubclassWindow();
	if (!m_button.Create(this, m_eType, m_nMenu)) {
		TRACE0(_T("Failed to create browse button.\n"));
		return;
	}
}

void CCJSearchEdit::SetSearchType(SEARCH_TYPE eType, UINT nMenu)
{
	m_eType = eType;
	m_nMenu = nMenu;
}

