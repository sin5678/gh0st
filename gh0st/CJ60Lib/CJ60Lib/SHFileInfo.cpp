// SHFileInfo.cpp : implementation file
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

#include "StdAfx.h"
#include "SHFileInfo.h"
#include <shlobj.h>

/////////////////////////////////////////////////////////////////////////////
// CSHFileInfo

CSHFileInfo::CSHFileInfo(LPCTSTR lpszFileName)
{
	m_strFileName = lpszFileName;
	_splitpath(m_strFileName, m_szDrive, m_szDir, m_szFname, m_szExt);
}

CSHFileInfo::CSHFileInfo(CFileFind *pFoundFile)
{
	m_pFoundFile = pFoundFile;
	m_strFileName = m_pFoundFile->GetFilePath();
	_splitpath(m_strFileName, m_szDrive, m_szDir, m_szFname, m_szExt);
}

CSHFileInfo::~CSHFileInfo()
{
	// TODO: add destruction code here.
}

///////////////////////////////////////////////////////////////////
//	Function:	GetFileName()
//	Purpose:	Retrieves current filename minus the path
//	Remarks:	if the filename is "c:\incoming\hello.txt", this
//				function returns "hello.txt".
///////////////////////////////////////////////////////////////////
CString CSHFileInfo::GetFileName()
{
	CString str;
	str.Format(_T("%s%s"), m_szFname, m_szExt);
	return str;
}

///////////////////////////////////////////////////////////////////
//	Function:	GetRoot()
//	Purpose:	Retrieves the path only of the current filename.
//	Remarks:	if the filename is "c:\incoming\hello.txt", this
//				function returns "c:\incoming\".
///////////////////////////////////////////////////////////////////
CString CSHFileInfo::GetRoot()
{
	CString str;
	str.Format(_T("%s%s"), m_szDrive, m_szDir);
	return str;
}

///////////////////////////////////////////////////////////////////
//	Function:	GetFileTitle()
//	Purpose:	Retrieves the title of the filename excluding
//				the path and extension.
//	Remarks:	if the filename is "c:\incoming\hello.txt", this
//				function returns "hello".
///////////////////////////////////////////////////////////////////
CString CSHFileInfo::GetFileTitle()
{
	return m_szFname;
}

///////////////////////////////////////////////////////////////////
//	Function:	GetFileExt()
//	Purpose:	Retrieves the file extension.
//	Remarks:	if the filename is "c:\incoming\hello.txt", this
//				function returns "txt".
///////////////////////////////////////////////////////////////////
CString CSHFileInfo::GetFileExt()
{
    return m_szExt;
}

///////////////////////////////////////////////////////////////////
//	Function:	GetDescription()
//	Purpose:	Returns the description of the file
///////////////////////////////////////////////////////////////////
CString CSHFileInfo::GetDescription()
{
	SHFILEINFO	sfi;

	::SHGetFileInfo(m_strFileName, 0, &sfi,
		sizeof(SHFILEINFO), SHGFI_TYPENAME);

   return sfi.szTypeName;
}

///////////////////////////////////////////////////////////////////
//	Function:	Exists()
//	Purpose:	Determines whether a file or directory exists.
///////////////////////////////////////////////////////////////////
bool CSHFileInfo::Exist()
{
	WIN32_FIND_DATA fd;

	CString	szFindPath=m_strFileName;
	int nSlash = szFindPath.ReverseFind(_T('\\'));
	int nLength = szFindPath.GetLength();
	
	if (nSlash == nLength-1)
	{
		if (nLength == 3)
			if (szFindPath.GetAt(1) == _T(':'))
				return true;
		else
			szFindPath = szFindPath.Left(nSlash);
	}

	HANDLE hFind = FindFirstFile( szFindPath, &fd );

	if ( hFind != INVALID_HANDLE_VALUE )
		FindClose( hFind );

	return hFind != INVALID_HANDLE_VALUE;
}

///////////////////////////////////////////////////////////////////
//	Function:	GetIconIndex()
//	Purpose:	Returns the icon index of the file.
///////////////////////////////////////////////////////////////////
int CSHFileInfo::GetIconIndex()
{
	SHFILEINFO    sfi;
	
	::SHGetFileInfo(m_strFileName, 0, &sfi,	sizeof(SHFILEINFO), 
		SHGFI_SYSICONINDEX | SHGFI_SMALLICON );
	
	return sfi.iIcon;
}

///////////////////////////////////////////////////////////////////
//	Function:	GetDisplayName()
//	Purpose:	Returns the display name for the file.
///////////////////////////////////////////////////////////////////
CString CSHFileInfo::GetDisplayName()
{
	SHFILEINFO sfi;

	::SHGetFileInfo(m_strFileName, 0, &sfi,
		sizeof(SHFILEINFO),	SHGFI_DISPLAYNAME);

	return sfi.szDisplayName;
}

///////////////////////////////////////////////////////////////////
//	Function:	GetLastWriteTime()
//	Purpose:	Returns the last time file was written to.
///////////////////////////////////////////////////////////////////
CString CSHFileInfo::GetLastWriteTime()
{
	CString strTime;
	CTime time;
	m_pFoundFile->GetLastWriteTime(time);

	// Format the date time string.
	strTime.Format( _T("%.2d/%.2d/%d %.2d:%.2d"), time.GetMonth(), time.GetDay(),
		time.GetYear(), time.GetHour(), time.GetSecond());

	return strTime;
}

///////////////////////////////////////////////////////////////////
//	Function:	GetFileSize()
//	Purpose:	Returns the comma seperated size of the file.
///////////////////////////////////////////////////////////////////
CString CSHFileInfo::GetFileSize()
{
	CString strFileSize;
	CString strTemp;
	
	strTemp.Format(_T("%d"), m_pFoundFile->GetLength());
	int nSize = strTemp.GetLength()-1;
	int nCount = 1;

	while (nSize >= 0)
	{
		strFileSize += strTemp.GetAt(nSize);

		if (nSize  == 0)
			break;
		if (nCount != 3)
			++nCount;
	
		else {
			strFileSize += _T(",");
			nCount = 1;
		}
		nSize--;
	}

	strFileSize.MakeReverse();
	return strFileSize;
}

///////////////////////////////////////////////////////////////////
//	Function:	GetSystemImageList()
//	Purpose:	Returns the system image list for small and large icons.
///////////////////////////////////////////////////////////////////
void CSHFileInfo::GetSystemImageList(CImageList * pSmallList, CImageList * pLargeList)
{
	//image list setup
	SHFILEINFO  ssfi, lsfi;
    
	// Get a handle to the system small icon list 
	HIMAGELIST hSystemSmallImageList = (HIMAGELIST)
		::SHGetFileInfo((LPCTSTR)_T("C:\\"), 0, &ssfi,
		sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON); 

	// Attach it to the small image list 
	// DON'T FORGET TO PUT pSmallList->Detach(); in your destructor
 	pSmallList->Attach(hSystemSmallImageList);

	// Get a handle to the system large icon list 
	HIMAGELIST hSystemLargeImageList =	(HIMAGELIST)
		::SHGetFileInfo((LPCTSTR)_T("C:\\"), 0,	&lsfi,
		sizeof(SHFILEINFO),	SHGFI_SYSICONINDEX | SHGFI_ICON); 

	// Attach it to the large image list 
	// DON'T FORGET TO PUT pLargeList->Detach(); in your destructor
 	pLargeList->Attach(hSystemLargeImageList); 
}

//////////////////////////////////////////////////////////////////////
// SHBrowseForFolder callback...
//////////////////////////////////////////////////////////////////////
static int __stdcall BrowseCtrlCallback(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	CSHFileInfo* pShellFileInfo = (CSHFileInfo*)lpData;
	
	if (uMsg == BFFM_INITIALIZED && !pShellFileInfo->m_strSelDir.IsEmpty())
	{
		::SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)(LPCTSTR)(pShellFileInfo->m_strSelDir));
	}
	else // uMsg == BFFM_SELCHANGED
	{
	}
	
	return FALSE;
}

///////////////////////////////////////////////////////////////////
//	Function:	BrowseForFolder()
//	Purpose:	Displays browse for folder dialog.
///////////////////////////////////////////////////////////////////
BOOL CSHFileInfo::BrowseForFolder(CWnd* pParentWnd)
{
	LPMALLOC pMalloc;
	if (::SHGetMalloc(&pMalloc)!= NOERROR)
		return FALSE;
	
	BROWSEINFO bInfo;
	LPITEMIDLIST pidl;
	ZeroMemory ( (PVOID) &bInfo,sizeof (BROWSEINFO));
	
	if (!m_strInitDir.IsEmpty ())
	{
		OLECHAR       olePath[MAX_PATH];
		ULONG         chEaten;
		ULONG         dwAttributes;
		HRESULT       hr;
		LPSHELLFOLDER pDesktopFolder;
		// 
		// Get a pointer to the Desktop's IShellFolder interface. 
		//
		if (SUCCEEDED(SHGetDesktopFolder(&pDesktopFolder)))
		{
			//
			// IShellFolder::ParseDisplayName requires the file name be in Unicode.
			//
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, m_strInitDir.GetBuffer(MAX_PATH), -1,
				olePath, MAX_PATH);
			
			m_strInitDir.ReleaseBuffer (-1);
			//
			// Convert the path to an ITEMIDLIST.
			//
			hr = pDesktopFolder->ParseDisplayName(NULL,
				NULL,
				olePath,
				&chEaten,
				&pidl,
				&dwAttributes);
			if (FAILED(hr))
			{
				pMalloc ->Free (pidl);
				pMalloc ->Release ();
				return FALSE;
			}
			bInfo.pidlRoot = pidl;
			
		}
	}

	bInfo.hwndOwner		 = pParentWnd->GetSafeHwnd();
	bInfo.pszDisplayName = m_strPath.GetBuffer (MAX_PATH);
	bInfo.lpszTitle		 = (m_strTitle.IsEmpty()) ? _T("Open"):m_strTitle;
	bInfo.ulFlags		 = BIF_RETURNFSANCESTORS|BIF_RETURNONLYFSDIRS;
	bInfo.lpfn			 = BrowseCtrlCallback;  // address of callback function
	bInfo.lParam		 = (LPARAM)this;		// pass address of object to callback function
	
	if ((pidl = ::SHBrowseForFolder(&bInfo)) == NULL)
	{
		return FALSE;
	}
	m_strPath.ReleaseBuffer();
	m_iImageIndex = bInfo.iImage;
	
	if (::SHGetPathFromIDList(pidl,m_strPath.GetBuffer(MAX_PATH)) == FALSE)
	{
		pMalloc ->Free(pidl);
		pMalloc ->Release();
		return FALSE;
	}
	
	m_strPath.ReleaseBuffer();
	
	pMalloc ->Free(pidl);
	pMalloc ->Release();

	return TRUE;
}
