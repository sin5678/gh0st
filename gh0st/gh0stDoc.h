// gh0stDoc.h : interface of the CGh0stDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_GH0STDOC_H__57FB1788_3C2A_40A9_BAB7_3F192505E38E__INCLUDED_)
#define AFX_GH0STDOC_H__57FB1788_3C2A_40A9_BAB7_3F192505E38E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CGh0stDoc : public CDocument
{
protected: // create from serialization only
	CGh0stDoc();
	DECLARE_DYNCREATE(CGh0stDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGh0stDoc)
	public:
	virtual void Serialize(CArchive& ar);
	virtual void DeleteContents();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGh0stDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CGh0stDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GH0STDOC_H__57FB1788_3C2A_40A9_BAB7_3F192505E38E__INCLUDED_)
