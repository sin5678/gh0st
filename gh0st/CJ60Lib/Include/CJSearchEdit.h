// CJCJSearchEdit.h : header file
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

#ifndef __SEARCHEDIT_H__
#define __SEARCHEDIT_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CCJSearchEdit window
/////////////////////////////////////////////////////////////////////////////

typedef enum SEARCH_TYPE {
	SEARCH_DIRECTORY = 0,
	SEARCH_FILE,
	SEARCH_POPUP,
};

class CEditButton : public CButton {
public:
	CEditButton();
protected:
	UINT	m_nID;
	CWnd*	m_pWnd;
	CFont	m_Font;
	SEARCH_TYPE m_eType;
	UINT m_nMenu;
public:
	UINT GetNextID(CWnd* pWnd) const;
	virtual BOOL Create(CWnd* pWnd, SEARCH_TYPE eType, UINT nMenu);
	virtual ~CEditButton();
protected:
	afx_msg void OnClicked();
	DECLARE_MESSAGE_MAP()
};

class AFX_EXT_CLASS CCJSearchEdit : public CEdit
{
// Construction
public:
	CCJSearchEdit();

// Attributes
public:
	SEARCH_TYPE m_eType;
	UINT m_nMenu;

// Operations
public:
	void SetSearchType(SEARCH_TYPE eType, UINT nMenu=-1);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJSearchEdit)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCJSearchEdit();

	// Generated message map functions
protected:
	CEditButton m_button;

	//{{AFX_MSG(CCJSearchEdit)
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __SEARCHEDIT_H__
