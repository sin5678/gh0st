// CJListView.h : header file
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
//      he has done with codeguru, enough can not be said, and for his articles
//		in the ListView section of codeguru, which is where most of this code
//		comes from.
//	<>  Many thanks to Microsoft for making the source code availiable for MFC. 
//		Since most of this work is a modification from existing classes and 
//		methods, this library would not have been possible.
//
// ==========================================================================  
// HISTORY:	  
// ==========================================================================  
//			1.00	16 Jan 1999	- Initial release.  
//			1.01	03 Mar 1999 - Added InitializeFlatHeader() in order to initialize
//								  the list control manually.
// ==========================================================================  
//  
/////////////////////////////////////////////////////////////////////////////

#ifndef __CJLISTVIEW_H__
#define __CJLISTVIEW_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afxcview.h>
#include "CJFlatHeaderCtrl.h"

#define MINCOLWIDTH	       70
#define MAXCOLWIDTH	       70

/////////////////////////////////////////////////////////////////////////////
// CCJListView window

class AFX_EXT_CLASS CCJListView : public CListView
{
	DECLARE_DYNCREATE(CCJListView)

// Construction
public:
	CCJListView();

// Attributes
public:

protected:
	int					m_nSortedCol; 
	BOOL				m_bSortAscending; 
	BOOL				m_bSaveColumnState;
	CString				m_strSection;
	CString				m_strControl;
	CCJFlatHeaderCtrl	m_headerCtrl;
	CUIntArray			m_NumericColumns;
// Operations
public:
	int InitializeFlatHeader();
	BOOL SortIntItems( int nCol, BOOL bAscending, int low = 0, int high = -1 );
	BOOL SortTextItems( int nCol, BOOL bAscending, int low = 0, int high = -1 );
	void SetExtendedStyle(DWORD dwNewStyle);
	void SaveColumnState(CString strSection, CString strControl);
	BOOL BuildColumns(int nCols, int* nWidth, int* nColString);
	int GetRegColumnWidth( int iColumn );
	void AutoSizeColumn( int iColumn );
	void SetColumnWidth( int nCol );
	void LoadColumnWidths();
	void SaveColumnWidths();

	void SetColumnNumeric( int iCol );
	void UnsetColumnNumeric(int iCol);
	int FindNumericColumnIndex( int iCol );
	const bool IsColumnNumeric( int ) const;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJListView)
	public:
	virtual void OnInitialUpdate();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCJListView();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCJListView)
	afx_msg void OnItemclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __CJLISTVIEW_H__
