// CJListCtrl.cpp : implementation file
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

#include "stdafx.h"
#include "CJListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCJListCtrl

CCJListCtrl::CCJListCtrl()
{
	m_nSortedCol = -1;
	m_bSortAscending = TRUE;
	m_bSaveColumnState = FALSE;
	m_strSection.Empty();
	m_strControl.Empty();
}

CCJListCtrl::~CCJListCtrl()
{
}

BEGIN_MESSAGE_MAP(CCJListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CCJListCtrl)
	ON_NOTIFY(HDN_ITEMCLICKA, 0, OnItemclick) 
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_NOTIFY(HDN_ITEMCLICKW, 0, OnItemclick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCJListCtrl message handlers

void CCJListCtrl::OnItemclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HD_NOTIFY *phdn = (HD_NOTIFY *) pNMHDR;

	if( phdn->iButton == 0 )
	{
		// User clicked on header using left mouse button
		if( phdn->iItem == m_nSortedCol )
			m_bSortAscending = !m_bSortAscending;
		else
			m_bSortAscending = TRUE;

		m_nSortedCol = phdn->iItem;

		if (IsColumnNumeric(m_nSortedCol))
			SortIntItems( m_nSortedCol, m_bSortAscending );
		else
			SortTextItems( m_nSortedCol, m_bSortAscending );
		m_headerCtrl.SetSortImage(m_nSortedCol, m_bSortAscending);
	}

	*pResult = 0;
}

// SortTextItems	- Sort the list based on column text
// Returns		- Returns true for success
// nCol			- column that contains the text to be sorted
// bAscending		- indicate sort order
// low			- row to start scanning from - default row is 0
// high			- row to end scan. -1 indicates last row

BOOL CCJListCtrl::SortTextItems( int nCol, BOOL bAscending, int low /*= 0*/, int high /*= -1*/ )
{
	if( nCol >= ((CHeaderCtrl*)GetDlgItem(0))->GetItemCount() )
		return FALSE;

	if( high == -1 ) high = GetItemCount() - 1;

	int lo = low;
	int hi = high;
	CString midItem;

	if( hi <= lo ) return FALSE;

	midItem = GetItemText( (lo+hi)/2, nCol );

	// loop through the list until indices cross
	while( lo <= hi )
	{
		// rowText will hold all column text for one row
		CStringArray rowText;

		// find the first element that is greater than or equal to
		// the partition element starting from the left Index.
		if( bAscending ) {
			while( ( lo < high ) && ( GetItemText(lo, nCol) < midItem ) )
				++lo;
		}
		else {
			while( ( lo < high ) && ( GetItemText(lo, nCol) > midItem ) )
				++lo;
		}

		// find an element that is smaller than or equal to
		// the partition element starting from the right Index.
		if( bAscending ) {
			while( ( hi > low ) && ( GetItemText(hi, nCol) > midItem ) )
				--hi;
		}

		else {
			while( ( hi > low ) && ( GetItemText(hi, nCol) < midItem ) )
				--hi;
		}

		// if the indexes have not crossed, swap
		// and if the items are not equal
		if( lo <= hi )
		{
			// swap only if the items are not equal
			if( GetItemText(lo, nCol) != GetItemText(hi, nCol))
			{
				// swap the rows
				LV_ITEM lvitemlo, lvitemhi;
				int nColCount =
					((CHeaderCtrl*)GetDlgItem(0))->GetItemCount();
				rowText.SetSize( nColCount );
				int i;
				for( i=0; i<nColCount; i++)
					rowText[i] = GetItemText(lo, i);
				lvitemlo.mask = LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
				lvitemlo.iItem = lo;
				lvitemlo.iSubItem = 0;
				lvitemlo.stateMask = LVIS_CUT | LVIS_DROPHILITED |
					LVIS_FOCUSED | LVIS_SELECTED |
					LVIS_OVERLAYMASK | LVIS_STATEIMAGEMASK;
				
				lvitemhi = lvitemlo;
				lvitemhi.iItem = hi;
				
				GetItem( &lvitemlo );
				GetItem( &lvitemhi );
				
				for( i=0; i<nColCount; i++)
					SetItemText(lo, i, GetItemText(hi, i));
				
				lvitemhi.iItem = lo;
				SetItem( &lvitemhi );
				
				for( i=0; i<nColCount; i++)
					SetItemText(hi, i, rowText[i]);
				
				lvitemlo.iItem = hi;
				SetItem( &lvitemlo );
			}
			
			++lo;
			--hi;
		}
	}
	
	// If the right index has not reached the left side of array
	// must now sort the left partition.
	if( low < hi )
		SortTextItems( nCol, bAscending , low, hi);
	
	// If the left index has not reached the right side of array
	// must now sort the right partition.
	if( lo < high )
		SortTextItems( nCol, bAscending , lo, high );
	
	return TRUE;
}

BOOL CCJListCtrl::SortIntItems( int nCol, BOOL bAscending, int low /*= 0*/, int high /*= -1*/ )
{
	if( nCol >= ((CHeaderCtrl*)GetDlgItem(0))->GetItemCount() )
		return FALSE;

	if( high == -1 ) high = GetItemCount() - 1;

	int lo = low;
	int hi = high;
	UINT midItem;

	if( hi <= lo ) return FALSE;

	midItem = atoi(GetItemText( (lo+hi)/2, nCol ).GetBuffer(0));

	// loop through the list until indices cross
	while( lo <= hi )
	{
		// rowText will hold all column text for one row
		CStringArray rowText;

		// find the first element that is greater than or equal to
		// the partition element starting from the left Index.
		if( bAscending ) {
			while( ( lo < high ) && ( atoi(GetItemText(lo, nCol).GetBuffer(0)) < midItem ) )
				++lo;
		}
		else {
			while( ( lo < high ) && ( atoi(GetItemText(lo, nCol).GetBuffer(0)) > midItem ) )
				++lo;
		}

		// find an element that is smaller than or equal to
		// the partition element starting from the right Index.
		if( bAscending ) {
			while( ( hi > low ) && ( atoi(GetItemText(hi, nCol).GetBuffer(0)) > midItem ) )
				--hi;
		}

		else {
			while( ( hi > low ) && ( atoi(GetItemText(hi, nCol).GetBuffer(0)) < midItem ) )
				--hi;
		}

		// if the indexes have not crossed, swap
		// and if the items are not equal
		if( lo <= hi )
		{
			// swap only if the items are not equal
			if( atoi(GetItemText(lo, nCol).GetBuffer(0)) != atoi(GetItemText(hi, nCol).GetBuffer(0)))
			{
				// swap the rows
				LV_ITEM lvitemlo, lvitemhi;
				int nColCount =
					((CHeaderCtrl*)GetDlgItem(0))->GetItemCount();
				rowText.SetSize( nColCount );
				int i;
				for( i=0; i<nColCount; i++)
					rowText[i] = GetItemText(lo, i);
				lvitemlo.mask = LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
				lvitemlo.iItem = lo;
				lvitemlo.iSubItem = 0;
				lvitemlo.stateMask = LVIS_CUT | LVIS_DROPHILITED |
					LVIS_FOCUSED | LVIS_SELECTED |
					LVIS_OVERLAYMASK | LVIS_STATEIMAGEMASK;
				
				lvitemhi = lvitemlo;
				lvitemhi.iItem = hi;
				
				GetItem( &lvitemlo );
				GetItem( &lvitemhi );
				
				for( i=0; i<nColCount; i++)
					SetItemText(lo, i, GetItemText(hi, i));
				
				lvitemhi.iItem = lo;
				SetItem( &lvitemhi );
				
				for( i=0; i<nColCount; i++)
					SetItemText(hi, i, rowText[i]);
				
				lvitemlo.iItem = hi;
				SetItem( &lvitemlo );
			}
			
			++lo;
			--hi;
		}
	}
	
	// If the right index has not reached the left side of array
	// must now sort the left partition.
	if( low < hi )
		SortIntItems( nCol, bAscending , low, hi);
	
	// If the left index has not reached the right side of array
	// must now sort the right partition.
	if( lo < high )
		SortIntItems( nCol, bAscending , lo, high );
	
	return TRUE;
}

void CCJListCtrl::SaveColumnWidths()
{
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);

	// You must set a unique name for every listctrl
	ASSERT( m_strSection.GetLength() );
	ASSERT( m_strControl.GetLength() );
	CString strValue;

	for( int nItem = 0; nItem < pHeader->GetItemCount(); nItem++ )
	{
		CString strTemp;
		strTemp.Format(_T("%d,"), GetColumnWidth(nItem) );
		strValue += strTemp;
	}

	AfxGetApp()->WriteProfileString( m_strSection, m_strControl, strValue );
}

const bool CCJListCtrl::IsColumnNumeric( int iCol ) const
{
	for( int i = 0; i < m_NumericColumns.GetSize(); i++ )
	{	
		if( m_NumericColumns.GetAt( i ) == (UINT)iCol )
			return true;
	}
	return false;
}

void CCJListCtrl::SetColumnNumeric( int iCol )
{
	m_NumericColumns.Add( iCol );
}

void CCJListCtrl::UnsetColumnNumeric(int iCol)
{
	int iIndex = FindNumericColumnIndex( iCol );
	if( iIndex >= 0 )
		m_NumericColumns.RemoveAt( iIndex );
}

int CCJListCtrl::FindNumericColumnIndex( int iCol )
{
	for( int i = 0; i < m_NumericColumns.GetSize(); i++ )
	{	
		if( m_NumericColumns.GetAt( i ) == (UINT)iCol )
			return i;
	}
	return -1;
}

void CCJListCtrl::LoadColumnWidths()
{
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);

	for( int nItem = 0; nItem < pHeader->GetItemCount(); nItem++ ) {
		SetColumnWidth(nItem);
	}
}

void CCJListCtrl::SetColumnWidth(int nCol)
{
	int iWidth = GetRegColumnWidth( nCol );
	if( iWidth < MINCOLWIDTH )
		AutoSizeColumn( nCol );
	else
		CListCtrl::SetColumnWidth( nCol, iWidth );
}

void CCJListCtrl::AutoSizeColumn(int iColumn)
{
	SetRedraw(TRUE);
	
	CListCtrl::SetColumnWidth(iColumn,LVSCW_AUTOSIZE);
	int wc1 = GetColumnWidth( iColumn );
	CListCtrl::SetColumnWidth(iColumn,LVSCW_AUTOSIZE_USEHEADER);
	int wc2 = GetColumnWidth( iColumn );
	int wc = max(MINCOLWIDTH,max( wc1,wc2 ));
	
	if( wc > MAXCOLWIDTH )
		wc = MAXCOLWIDTH;
	
	CListCtrl::SetColumnWidth( iColumn,wc );  
	SetRedraw(FALSE);
}

int CCJListCtrl::GetRegColumnWidth(int iColumn)
{
	ASSERT( m_strSection.GetLength() );
	ASSERT( m_strControl.GetLength() );
	
	CString strEntry( m_strControl );
	CString strValue, strSubString;

	strValue = AfxGetApp()->GetProfileString( m_strSection, strEntry, _T("") );
	AfxExtractSubString(strSubString, strValue, iColumn, _T(','));
	return atoi((char*)(LPCTSTR)strSubString);
}

void CCJListCtrl::OnDestroy() 
{
	if (m_bSaveColumnState)
		SaveColumnWidths();
	CListCtrl::OnDestroy();
}

BOOL CCJListCtrl::BuildColumns(int nCols, int * nWidth, int * nColString)
{
	//insert columns
	int i;
	LV_COLUMN lvc;

	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	CString	strTemp;
	for(i = 0; i < nCols; i++)
	{
		lvc.iSubItem = i;
		strTemp.LoadString(nColString[i]);
		lvc.pszText = strTemp.GetBuffer(0);
		strTemp.ReleaseBuffer(-1);
		lvc.cx = nWidth[i];
		lvc.fmt = LVCFMT_LEFT;
		InsertColumn(i,&lvc);
	}

	if (m_bSaveColumnState)
		LoadColumnWidths();
	return TRUE;
}

void CCJListCtrl::SaveColumnState(CString strSection, CString strControl)
{
	m_strSection = strSection;
	m_strControl = strControl;
	m_bSaveColumnState = TRUE;
}

// ie: LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP
void CCJListCtrl::SetExtendedStyle(DWORD dwNewStyle)
{
	// Returns the current extended style ( a DWORD ).
	DWORD dwStyle = ::SendMessage (m_hWnd, LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0);

	// Add the full row select and grid line style to the existing extended styles.
	dwStyle |= dwNewStyle;

	// Sets the current extended style ( a DWORD ).
	::SendMessage (m_hWnd, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, dwStyle);
}

int CCJListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return InitializeFlatHeader();
}

void CCJListCtrl::PreSubclassWindow() 
{
	CListCtrl::PreSubclassWindow();
	InitializeFlatHeader();
}

int CCJListCtrl::InitializeFlatHeader()
{
	if (!m_headerCtrl.SubclassWindow(GetDlgItem(0)->GetSafeHwnd()))
		return -1;

	m_headerCtrl.FlatHeader();
	SaveColumnState(_T("Settings"), _T("Control"));

	return 0;
}
