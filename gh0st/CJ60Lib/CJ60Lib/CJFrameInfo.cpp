////////////////////////////////////////////////////////////////
// Copyright 1999 Takehiko Mizoguti
// If this code works, it was written by Takehiko Mizoguti.
// If not, I don't know who wrote it.
//
// CCJFrameInfo implements FrameWindow Info for CCJFrameWnd Families.
//

#include "StdAfx.h"
#include "CJFrameInfo.h"

CCJFrameInfo::CCJFrameInfo()
{
	m_pFrameWnd = NULL;
	m_strSection.Empty();
	m_strEntry.Empty();

	length = sizeof( WINDOWPLACEMENT );
	flags = NULL;
	showCmd = NULL;
	ptMinPosition.x = 0;
	ptMinPosition.y = 0;
	ptMaxPosition.x = 0;
	ptMaxPosition.y = 0;
	rcNormalPosition.left = 0;
	rcNormalPosition.top = 0;
	rcNormalPosition.right = 0;
	rcNormalPosition.bottom = 0;
}

CCJFrameInfo::~CCJFrameInfo()
{
}

void CCJFrameInfo::EnableRestorePlacement( CFrameWnd* pFrame, CString strSection, CString strEntry )
{
	m_pFrameWnd = pFrame;
	m_strSection = strSection;
	m_strEntry = strEntry;
}

void CCJFrameInfo::LoadFramePlacement()
{
	if( !m_pFrameWnd )	return;

	ASSERT( !m_strSection.IsEmpty() );
	ASSERT( !m_strEntry.IsEmpty() );
	
	CString strValue;
	strValue = AfxGetApp()->GetProfileString( m_strSection, m_strEntry, "" );

	int nField = _stscanf( strValue, _T("%i,%i,%i,%i,%i,%i,%i,%i,%i,%i"),
							&flags,
							&showCmd,
							&ptMinPosition.x,
							&ptMinPosition.y,
							&ptMaxPosition.x,
							&ptMaxPosition.y,
							&rcNormalPosition.left,
							&rcNormalPosition.top,
							&rcNormalPosition.right,
							&rcNormalPosition.bottom
						);

	if( nField == 10 )	m_pFrameWnd->SetWindowPlacement( this );
}

void CCJFrameInfo::SaveFramePlacement()
{
	if( !m_pFrameWnd )	return;

	ASSERT( !m_strSection.IsEmpty() );
	ASSERT( !m_strEntry.IsEmpty() );
	
	m_pFrameWnd->GetWindowPlacement( this );

	CString strValue;
	strValue.Format( _T("%i,%i,%i,%i,%i,%i,%i,%i,%i,%i"),
						flags,
						showCmd,
						ptMinPosition.x,
						ptMinPosition.y,
						ptMaxPosition.x,
						ptMaxPosition.y,
						rcNormalPosition.left,
						rcNormalPosition.top,
						rcNormalPosition.right,
						rcNormalPosition.bottom
					);

	AfxGetApp()->WriteProfileString( m_strSection, m_strEntry, strValue );
}
