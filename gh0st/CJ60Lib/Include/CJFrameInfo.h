////////////////////////////////////////////////////////////////
// Copyright 1999 Takehiko Mizoguti
// If this code works, it was written by Takehiko Mizoguti.
// If not, I don't know who wrote it.
//
// CCJFrameInfo implements FrameWindow Info for CCJFrameWnd Families.
//

#ifndef __CJFRAMEINFO_H__
#define __CJFRAMEINFO_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class AFX_EXT_CLASS CCJFrameInfo : public WINDOWPLACEMENT
{
// Construction
public:
	CCJFrameInfo();

// Attributes
protected:
	CFrameWnd*	m_pFrameWnd;
	CString		m_strSection;
	CString		m_strEntry;

// Operations
public:
	void EnableRestorePlacement( CFrameWnd* pFrame, CString strSection, CString strEntry );
	void LoadFramePlacement();
	void SaveFramePlacement();

// Implementation
public:
	virtual ~CCJFrameInfo();

};

#endif // __CLFRAMEINFO_H__