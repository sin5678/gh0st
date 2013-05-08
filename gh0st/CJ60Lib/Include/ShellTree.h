#ifndef __SHELLTREE_H__
#define __SHELLTREE_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ShellTree.h : header file
//

#include <shlobj.h>
#include <Shlwapi.h>
#pragma comment(lib,"Shlwapi.lib") 

#include "ShellPidl.h"

/////////////////////////////////////////////////////////////////////////////
// CShellTree window
//
// This source is part of CShellTree - Selom Ofori
// 
// Version: 1.02 (any previously unversioned copies are older/inferior
//
// This code is free for all to use. Mutatilate it as much as you want
// See MFCENUM sample from microsoft
// ==========================================================================  
// HISTORY:	  
// ==========================================================================  
//			1.01	24 Feb 1999 - Overloaded PopulateTree(LPCTSTR lpPath) in the 
//								  class CShellTree to fill the tree based upon 
//								  path. Takehiko Mizoguti [mizoguti@m2.sys.to.casio.co.jp]
// ==========================================================================  
/////////////////////////////////////////////////////////////////////////////

class AFX_EXT_CLASS CShellTree : public CTreeCtrl, public CShellPidl
{
public:
	enum FindAttribs{type_drive,type_folder};

// Construction
public:
	CShellTree();

// Attributes
public:
    BOOL m_bOutaHere;

// Operations
public:
	void	PopulateTree(LPCTSTR lpPath);
	void	PopulateTree();
	void	PopulateTree(int nFolder);
	void	OnFolderExpanding(NMHDR* pNMHDR, LRESULT* pResult);
	void	GetContextMenu(NMHDR* pNMHDR, LRESULT* pResult);
	BOOL	OnFolderSelected(NMHDR* pNMHDR, LRESULT* pResult, CString &szFolderPath);
	void	OnDeleteShellItem(NMHDR* pNMHDR, LRESULT* pResult);
	void	EnableImages();
	BOOL	GetSelectedFolderPath(CString &szFolderPath);
	bool	SearchTree(HTREEITEM treeNode, CString szSearchName, FindAttribs attr);
	void	TunnelTree(CString szFindPath);
	LPSHELLFOLDER	GetParentShellFolder(HTREEITEM folderNode);
	LPITEMIDLIST	GetRelativeIDLIST(HTREEITEM folderNode);
	LPITEMIDLIST	GetFullyQualifiedID(HTREEITEM folderNode);
	void	FindTreePidl(	HTREEITEM nextNode,
							HTREEITEM& folderNode, 
							LPLVITEMDATA lplvid, 
							bool& valid);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShellTree)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CShellTree();

	// Generated message map functions
protected:
	void	FillTreeView(LPSHELLFOLDER lpsf, LPITEMIDLIST  lpifq, HTREEITEM     hParent);
	void	GetNormalAndSelectedIcons(LPITEMIDLIST lpifq, LPTV_ITEM lptvitem);
 	static int CALLBACK TreeViewCompareProc(LPARAM, LPARAM, LPARAM);
	
	//{{AFX_MSG(CShellTree)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __SHELLTREE_H__
