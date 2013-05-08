//
//	Class:		CWinXPButtonST
//
//	Compiler:	Visual C++
//				eMbedded Visual C++
//	Tested on:	Visual C++ 6.0
//				Windows CE 3.0
//
//	Created:	03/September/2001
//	Updated:	25/November/2002
//
//	Author:		Davide Calabro'		davide_calabro@yahoo.com
//	Modified by jingzhou xu.
//
//	Disclaimer
//	----------
//	THIS SOFTWARE AND THE ACCOMPANYING FILES ARE DISTRIBUTED "AS IS" AND WITHOUT
//	ANY WARRANTIES WHETHER EXPRESSED OR IMPLIED. NO REPONSIBILITIES FOR POSSIBLE
//	DAMAGES OR EVEN FUNCTIONALITY CAN BE TAKEN. THE USER MUST ASSUME THE ENTIRE
//	RISK OF USING THIS SOFTWARE.
//
//	Terms of use
//	------------
//	THIS SOFTWARE IS FREE FOR PERSONAL USE OR FREEWARE APPLICATIONS.
//	IF YOU USE THIS SOFTWARE IN COMMERCIAL OR SHAREWARE APPLICATIONS YOU
//	ARE GENTLY ASKED TO DONATE 5$ (FIVE U.S. DOLLARS) TO THE AUTHOR:
//
//		Davide Calabro'
//		P.O. Box 65
//		21019 Somma Lombardo (VA)
//		Italy
// 
#ifndef _WINXPBUTTONST_H_
#define _WINXPBUTTONST_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef UNDER_CE
#include "CeBtnST.h"
#define	BASE_BUTTONST	CCeButtonST
#else
#include "BtnST.h"
#define	BASE_BUTTONST	CButtonST
#endif

class CWinXPButtonST : public BASE_BUTTONST
{
public:
	CWinXPButtonST();
	virtual ~CWinXPButtonST();
	
	DWORD SetRounded(BOOL bRounded, BOOL bRepaint = TRUE);
protected:
	virtual DWORD OnDrawBackground(CDC* pDC, CRect* pRect);
	virtual DWORD OnDrawBorder(CDC* pDC, CRect* pRect);
	
private:
	BOOL	m_bIsRounded;		// Borders must be rounded?
};

#endif 
