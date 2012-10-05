// SHFileInfo.h : header file
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
//  <>  Thanks to Girish Bharadwaj (Girish_Bharadwaj@Pictel.com) for his article
//      'Class to select directory' which is where the 'BrowseForFolder()' method
//      came from.
//  <>  Thanks to by Matt Esterly (matt_esterly@vds.com) for his article 
//      'Attaching System ImageList to ListControl' which is where the
//		idea for 'GetSystemImageList() came from.
//  <>  Thanks to Selom Ofori (sofori@chat.carleton.ca) for his article 
//		'Class for Browsing shell namespace with your own dialog', which is
//		where the inspiration for the rest of this class came from, as well as
//		the CShellTree() and CShellPidl() classes came from.
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
//			1.01	22 Jan 1999 - Added include file <shlobj.h> 
//			1.02	24 Feb 1999 - Added GetFileExt() to return the file extension
//								  per Fredfrik Mattsson's [fremat@algonet.se] request.
// ==========================================================================  
//  
/////////////////////////////////////////////////////////////////////////////

#ifndef __SHFILEINFO_H__
#define __SHFILEINFO_H__

/////////////////////////////////////////////////////////////////////////////
// CSHFileInfo class

class AFX_EXT_CLASS CSHFileInfo
{
// Construction
public:
	CSHFileInfo(LPCTSTR lpszFileName = NULL);
	CSHFileInfo(CFileFind *pFoundFile);

// Attributes
public:
    int		m_iImageIndex;
    CString m_strPath;
    CString m_strInitDir;
    CString m_strSelDir;
    CString m_strTitle;
protected:
	CFileFind*	m_pFoundFile;
	CString		m_strFileName;
	char		m_szDrive[_MAX_DRIVE];
	char		m_szDir[_MAX_DIR];
	char		m_szFname[_MAX_FNAME];
	char		m_szExt[_MAX_EXT];

// Operations
public:
	CString GetFileSize();
	CString GetLastWriteTime();
	CString	GetDisplayName();
	int 	GetIconIndex();
	void	SetFileName(LPCTSTR lpszFileName=NULL);
	CString	GetFileName();
	CString	GetRoot();
	CString	GetFileTitle();
	CString GetFileExt();
	CString	GetDescription();
	bool	Exist();
	void	GetSystemImageList(CImageList * pSmallList, CImageList * pLargeList);
	BOOL	BrowseForFolder(CWnd* pParentWnd);

// Implementation
public:
	virtual ~CSHFileInfo();
};

/////////////////////////////////////////////////////////////////////////////

#endif // __SHFILEINFO_H__

