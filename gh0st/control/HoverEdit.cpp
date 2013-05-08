// HoverEdit.cpp : implementation file
//

#include "stdafx.h"
#include "HoverEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHoverEdit

CHoverEdit::CHoverEdit()
{
}

CHoverEdit::~CHoverEdit()
{
}

BEGIN_MESSAGE_MAP(CHoverEdit, CEdit)
//{{AFX_MSG_MAP(CHoverEdit)
ON_WM_NCPAINT()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHoverEdit message handlers

void CHoverEdit::OnHoverEnter()
{
	Redraw();
}

void CHoverEdit::OnHoverLeave()
{
	Redraw();
}

void CHoverEdit::Redraw()
{
	RedrawWindow(NULL,NULL,RDW_FRAME|RDW_INVALIDATE);
}

void CHoverEdit::OnNcPaint() 
{
	CWindowDC DC(this);
	CRect Rect;
	GetWindowRect(&Rect);
	if (IsHover())
	{
		DC.Rectangle(0,0,Rect.Width(),Rect.Height());
	}
	else
	{
		DC.DrawEdge(CRect(0,0,Rect.Width(),Rect.Height()),EDGE_SUNKEN,BF_FLAT|BF_RECT);
	}
}
