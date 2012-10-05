// CJPagerCtrl.cpp : implementation file
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
//			1.01	02 Jan 1999 - Cleaned up class source, wrapped all pager
//								  messages.
// ==========================================================================
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CJPagerCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCJPagerCtrl

CCJPagerCtrl::CCJPagerCtrl()
{
	static BOOL bInit = FALSE;

    if (!bInit) {
        INITCOMMONCONTROLSEX sex;
        sex.dwSize = sizeof(INITCOMMONCONTROLSEX);
		sex.dwICC = ICC_PAGESCROLLER_CLASS|ICC_BAR_CLASSES;
        ::InitCommonControlsEx(&sex);
        bInit = TRUE;
    }
}

CCJPagerCtrl::~CCJPagerCtrl()
{
}

BEGIN_MESSAGE_MAP(CCJPagerCtrl, CWnd)
	//{{AFX_MSG_MAP(CCJPagerCtrl)
	ON_NOTIFY_REFLECT_EX(PGN_SCROLL, OnPagerScroll)
	ON_NOTIFY_REFLECT_EX(PGN_CALCSIZE, OnPagerCalcSize)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CCJPagerCtrl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) 
{
    return CWnd::Create(WC_PAGESCROLLER, NULL,
		dwStyle, CRect(0,0,0,0), pParentWnd, nID);
}

/////////////////////////////////////////////////////////////////////////////
// CCJPagerCtrl message handlers

BOOL CCJPagerCtrl::OnPagerCalcSize(NMPGCALCSIZE* pNMPGCalcSize, LRESULT* pResult)
{
	switch(pNMPGCalcSize->dwFlag)
    {
	case PGF_CALCWIDTH:
		pNMPGCalcSize->iWidth = m_nWidth;
		break;
		
	case PGF_CALCHEIGHT:
		pNMPGCalcSize->iHeight = m_nHeight;
        break;
	}
	
    *pResult = 0;
	return FALSE;	// Let parent handle message
}

BOOL CCJPagerCtrl::OnPagerScroll(NMPGSCROLL* pNMPGScroll, LRESULT* pResult)
{
    *pResult = 0;
	return FALSE;	// Let parent handle message
}
