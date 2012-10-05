// gh0stDoc.cpp : implementation of the CGh0stDoc class
//

#include "stdafx.h"
#include "gh0st.h"
#include "gh0stDoc.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGh0stDoc

IMPLEMENT_DYNCREATE(CGh0stDoc, CDocument)

BEGIN_MESSAGE_MAP(CGh0stDoc, CDocument)
	//{{AFX_MSG_MAP(CGh0stDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGh0stDoc construction/destruction

CGh0stDoc::CGh0stDoc()
{
	// TODO: add one-time construction code here

}

CGh0stDoc::~CGh0stDoc()
{
}

//DEL BOOL CGh0stDoc::OnNewDocument()
//DEL {
//DEL 	if (!CDocument::OnNewDocument())
//DEL 		return FALSE;
//DEL 
//DEL 	// TODO: add reinitialization code here
//DEL 	// (SDI documents will reuse this document)
//DEL 	return TRUE;
//DEL }



/////////////////////////////////////////////////////////////////////////////
// CGh0stDoc serialization

void CGh0stDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CGh0stDoc diagnostics

#ifdef _DEBUG
void CGh0stDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CGh0stDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGh0stDoc commands

void CGh0stDoc::DeleteContents() 
{
	// TODO: Add your specialized code here and/or call the base class
	if(AfxGetApp()->m_pMainWnd)
		((CMainFrame*)AfxGetApp()->m_pMainWnd)->DeleteContents();
	CDocument::DeleteContents();
}
