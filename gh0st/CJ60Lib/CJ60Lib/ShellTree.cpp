// ShellTree.cpp : implementation file
//

#include "stdafx.h"
#include "ShellTree.h"
#include "SHFileInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShellTree
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

CShellTree::CShellTree()
{
}

CShellTree::~CShellTree()
{
}


BEGIN_MESSAGE_MAP(CShellTree, CTreeCtrl)
	//{{AFX_MSG_MAP(CShellTree)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/****************************************************************************
*
*    FUNCTION: PopulateTree()
*
*    PURPOSE:  Processes the File.Fill/RefreshTree command
*
****************************************************************************/
void CShellTree::PopulateTree() 
{

    LPSHELLFOLDER lpsf=NULL;
    LPITEMIDLIST  lpi=NULL;
    HRESULT hr;
    TV_SORTCB      tvscb;
   
    // Get a pointer to the desktop folder.
    hr=SHGetDesktopFolder(&lpsf);

    if (SUCCEEDED(hr))
    {
       // Initialize the tree view to be empty.
       DeleteAllItems();

       // Fill in the tree view from the root.
       FillTreeView(lpsf, NULL, TVI_ROOT);
       //TunnelFillTree(lpsf, NULL, TVI_ROOT);
       // Release the folder pointer.
       lpsf->Release();
    }
    tvscb.hParent     = TVI_ROOT;
    tvscb.lParam      = 0;
    tvscb.lpfnCompare = TreeViewCompareProc;

    // Sort the items in the tree view
	SortChildrenCB(&tvscb/*, FALSE*/);
    
	HTREEITEM hItem;
	hItem = GetRootItem();
	Expand(hItem,TVE_EXPAND);
	Select(GetRootItem(),TVGN_CARET);
}

/****************************************************************************
*
*	FUNCTION:	PopulateTree()
*
*	PURPOSE:	Processes the File.Fill/RefreshTree command
*				This overload  has the ability to open from a
*				special folderlocation like SHBrowseForFolder()
*
*	WARNING:	TunnelTree() will not work if you use a special
*				folderlocation
*
****************************************************************************/
void CShellTree::PopulateTree(int nFolder) 
{

	LPSHELLFOLDER lpsf=NULL,lpsf2=NULL;
    LPITEMIDLIST  lpi=NULL;
    HRESULT hr;
    TV_SORTCB      tvscb;
   
    // Get a pointer to the desktop folder.
	hr=SHGetDesktopFolder(&lpsf);

    if (SUCCEEDED(hr))
    {
       // Initialize the tree view to be empty.
		DeleteAllItems();

		if (!SUCCEEDED(SHGetSpecialFolderLocation( 
				m_hWnd, nFolder, &lpi))) 
		{ 
			lpi=NULL;
			FillTreeView(lpsf,NULL,TVI_ROOT);
		}
		else
		{
			hr=lpsf->BindToObject(lpi,
                0, IID_IShellFolder,(LPVOID *)&lpsf2);

			if(SUCCEEDED(hr))
			{
				// Fill in the tree view from the root.
				FillTreeView(lpsf2, lpi, TVI_ROOT);
				lpsf2->Release();
			}
			else
				FillTreeView(lpsf,NULL,TVI_ROOT);
		}

		// Release the folder pointer.
		lpsf->Release();
	}
    tvscb.hParent     = TVI_ROOT;
    tvscb.lParam      = 0;
    tvscb.lpfnCompare = TreeViewCompareProc;

    // Sort the items in the tree view
	SortChildrenCB(&tvscb/*, FALSE*/);
    
	HTREEITEM hItem;
	hItem = GetRootItem();
	Expand(hItem,TVE_EXPAND);
	Select(GetRootItem(),TVGN_CARET);
}

/****************************************************************************
*
*  FUNCTION: FillTreeView( LPSHELLFOLDER lpsf,
*                          LPITEMIDLIST  lpifq,
*                          HTREEITEM     hParent)
*
*  PURPOSE: Fills a branch of the TreeView control.  Given the
*           shell folder, enumerate the subitems of this folder,
*           and add the appropriate items to the tree.
*
*  PARAMETERS:
*    lpsf         - Pointer to shell folder that we want to enumerate items 
*    lpifq        - Fully qualified item id list to the item that we are enumerating
*                   items for.  In other words, this is the PIDL to the item
*                   identified by the lpsf parameter.
*    hParent      - Parent node
*
*  COMMENTS:
*    This function enumerates the items in the folder identifed by lpsf.
*    Note that since we are filling the left hand pane, we will only add
*    items that are folders and/or have sub-folders.  We *could* put all
*    items in here if we wanted, but that's not the intent.
*
****************************************************************************/
void CShellTree::FillTreeView(LPSHELLFOLDER lpsf, LPITEMIDLIST  lpifq, HTREEITEM     hParent)
{
    TV_ITEM         tvi;                          // TreeView Item.
    TV_INSERTSTRUCT tvins;                        // TreeView Insert Struct.
    HTREEITEM       hPrev = NULL;                 // Previous Item Added.
    LPSHELLFOLDER   lpsf2=NULL;
    LPENUMIDLIST    lpe=NULL;
    LPITEMIDLIST    lpi=NULL, lpiTemp=NULL, lpifqThisItem=NULL;
    LPTVITEMDATA    lptvid=NULL;
    LPMALLOC        lpMalloc=NULL;
    ULONG           ulFetched;
    UINT            uCount=0;
    HRESULT         hr;
    char            szBuff[256];
    HWND            hwnd=::GetParent(m_hWnd);

    // Allocate a shell memory object. 
    hr=::SHGetMalloc(&lpMalloc);
    if (FAILED(hr))
       return;

    if (SUCCEEDED(hr))
    {
        // Get the IEnumIDList object for the given folder.
        hr=lpsf->EnumObjects(hwnd, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS, &lpe);

        if (SUCCEEDED(hr))
        {
            // Enumerate throught the list of folder and non-folder objects.
            while (S_OK==lpe->Next(1, &lpi, &ulFetched))
            {
                //Create a fully qualified path to the current item
                //The SH* shell api's take a fully qualified path pidl,
                //(see GetIcon above where I call SHGetFileInfo) whereas the
                //interface methods take a relative path pidl.
                ULONG ulAttrs = SFGAO_HASSUBFOLDER | SFGAO_FOLDER;

                // Determine what type of object we have.
                lpsf->GetAttributesOf(1, (const struct _ITEMIDLIST **)&lpi, &ulAttrs);

                if (ulAttrs & (SFGAO_HASSUBFOLDER | SFGAO_FOLDER))
                {
                   //We need this next if statement so that we don't add things like
                   //the MSN to our tree.  MSN is not a folder, but according to the
                   //shell it has subfolders.
                   if (ulAttrs & SFGAO_FOLDER)
                   {
                      tvi.mask= TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;

                      if (ulAttrs & SFGAO_HASSUBFOLDER)
                      {
                         //This item has sub-folders, so let's put the + in the TreeView.
                         //The first time the user clicks on the item, we'll populate the
                         //sub-folders.
                         tvi.cChildren=1;
                         tvi.mask |= TVIF_CHILDREN;
                      }
                        
                      //OK, let's get some memory for our ITEMDATA struct
                      lptvid = (LPTVITEMDATA)lpMalloc->Alloc(sizeof(TVITEMDATA));
                      if (!lptvid)
                         goto Done;  // Error - could not allocate memory.
   
                      //Now get the friendly name that we'll put in the treeview.
                      if (!GetName(lpsf, lpi, SHGDN_NORMAL, szBuff))
                         goto Done; // Error - could not get friendly name.

                      tvi.pszText    = szBuff;
                      tvi.cchTextMax = MAX_PATH;
    
                      lpifqThisItem=ConcatPidls(lpifq, lpi);
      
                      //Now, make a copy of the ITEMIDLIST
                      lptvid->lpi=CopyITEMID(lpMalloc, lpi);
   
                      GetNormalAndSelectedIcons(lpifqThisItem, &tvi);
   
                      lptvid->lpsfParent=lpsf;    //Store the parent folders SF
                      lpsf->AddRef();

                      lptvid->lpifq=ConcatPidls(lpifq, lpi);
   
                      tvi.lParam = (LPARAM)lptvid;
   
                      // Populate the TreeVeiw Insert Struct
                      // The item is the one filled above.
                      // Insert it after the last item inserted at this level.
                      // And indicate this is a root entry.
                      tvins.item         = tvi;
                      tvins.hInsertAfter = hPrev;
                      tvins.hParent      = hParent;
   
                      // Add the item to the tree
                      hPrev = InsertItem(&tvins);
                   }
                   // Free this items task allocator.
                   lpMalloc->Free(lpifqThisItem);  
                   lpifqThisItem=0;
                }

                lpMalloc->Free(lpi);  //Free the pidl that the shell gave us.
                lpi=0;
            }
        }

    }
    else
       return;

Done:
 
    if (lpe)  
        lpe->Release();

    //The following 2 if statements will only be TRUE if we got here on an
    //error condition from the "goto" statement.  Otherwise, we free this memory
    //at the end of the while loop above.
    if (lpi && lpMalloc)           
        lpMalloc->Free(lpi);
    if (lpifqThisItem && lpMalloc) 
        lpMalloc->Free(lpifqThisItem);  

    if (lpMalloc) 
        lpMalloc->Release();
}


/****************************************************************************
*
*    FUNCTION: GetNormalAndSelectedIcons(LPITEMIDLIST lpifq, LPTV_ITEM lptvitem)
*
*    PURPOSE:  Gets the index for the normal and selected icons for the current item.
*
*    PARAMETERS:
*    lpifq    - Fully qualified item id list for current item.
*    lptvitem - Pointer to treeview item we are about to add to the tree.
*
****************************************************************************/
void CShellTree::GetNormalAndSelectedIcons(LPITEMIDLIST lpifq,
                               LPTV_ITEM lptvitem)
{
   //Note that we don't check the return value here because if GetIcon()
   //fails, then we're in big trouble...

   lptvitem->iImage = GetItemIcon(lpifq, SHGFI_PIDL | 
                              SHGFI_SYSICONINDEX | 
                              SHGFI_SMALLICON);
   
   lptvitem->iSelectedImage = GetItemIcon(lpifq, SHGFI_PIDL | 
                                      SHGFI_SYSICONINDEX | 
                                      SHGFI_SMALLICON |
                                      SHGFI_OPENICON);
   
   return;
}



/****************************************************************************
*
*    FUNCTION: TreeViewCompareProc(LPARAM, LPARAM, LPARAM)
*
*    PURPOSE:  Callback routine for sorting the tree 
*
****************************************************************************/
int CALLBACK CShellTree::TreeViewCompareProc(LPARAM lparam1, 
    LPARAM lparam2, LPARAM lparamSort)
{
    LPTVITEMDATA lptvid1=(LPTVITEMDATA)lparam1;
    LPTVITEMDATA lptvid2=(LPTVITEMDATA)lparam2;
    HRESULT   hr;

    hr = lptvid1->lpsfParent->CompareIDs(0,lptvid1->lpi,lptvid2->lpi);

    if (FAILED(hr))
       return 0;

    return (short)SCODE_CODE(GetScode(hr));
}




/////////////////////////////////////////////////////////////////////////////
// CShellTree message handlers

/****************************************************************************
*
*	FUNCTION:	OnFolderExpanding(NMHDR* pNMHDR, LRESULT* pResult) 
*
*	PURPOSE:	Reponds to an TVN_ITEMEXPANDING message in order to fill up
*				subdirectories. Pass the parameters from OnItemExpanding() to 
*				this function. You need to do that or your folders won't
*				expand.
*
*	OTHER:		It can also be used to update a corresponding listview. Seem MFCENUM
*
*	MESSAGEMAP:	TVN_ITEMEXPANDING
*
****************************************************************************/
void CShellTree::OnFolderExpanding(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LPTVITEMDATA   lptvid;  //Long pointer to TreeView item data
	HRESULT        hr;
	LPSHELLFOLDER  lpsf2=NULL;
	static char    szBuff[MAX_PATH];
	TV_SORTCB      tvscb;

	NM_TREEVIEW* pnmtv = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
    if ((pnmtv->itemNew.state & TVIS_EXPANDEDONCE))
         return;
		
    lptvid=(LPTVITEMDATA)pnmtv->itemNew.lParam;
    if (lptvid)
       {
            hr=lptvid->lpsfParent->BindToObject(lptvid->lpi,
                0, IID_IShellFolder,(LPVOID *)&lpsf2);

            if (SUCCEEDED(hr))
            {
                FillTreeView(lpsf2,
                       lptvid->lpifq,
                       pnmtv->itemNew.hItem);
            }

            tvscb.hParent     = pnmtv->itemNew.hItem;
            tvscb.lParam      = 0;
            tvscb.lpfnCompare = TreeViewCompareProc;

            SortChildrenCB(&tvscb /*, FALSE*/);
    }	
	
	*pResult = 0;
}

/****************************************************************************
*
*	FUNCTION:	GetContextMenu(NMHDR* pNMHDR, LRESULT* pResult) 
*
*	PURPOSE:	Diplays a popup menu for the folder selected. Pass the
*				parameters from Rclick() to this function.
*
*	MESSAGEMAP:	NM_RCLICK;
*
****************************************************************************/
void CShellTree::GetContextMenu(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	POINT			pt;
	LPTVITEMDATA	lptvid;  //Long pointer to TreeView item data
	LPSHELLFOLDER	lpsf2=NULL;
	static char		szBuff[MAX_PATH];
	TV_HITTESTINFO	tvhti;
	TV_ITEM			tvi;

	// TODO: Add your control notification handler code here
	::GetCursorPos((LPPOINT)&pt);
	ScreenToClient(&pt);
	tvhti.pt=pt;
    HitTest(&tvhti);
    SelectItem(tvhti.hItem);
	if (tvhti.flags & (TVHT_ONITEMLABEL|TVHT_ONITEMICON))
	{
		ClientToScreen(&pt);
		tvi.mask=TVIF_PARAM;
		tvi.hItem=tvhti.hItem;
		
        if (!GetItem(&tvi)){
			return;
		}
		
		lptvid=(LPTVITEMDATA)tvi.lParam;
		
		DoTheMenuThing(::GetParent(m_hWnd),
		lptvid->lpsfParent, lptvid->lpi, &pt);
	}	
	
	*pResult = 0;
}

/****************************************************************************
*
*	FUNCTION:	OnFolderSelected(NMHDR* pNMHDR, LRESULT* pResult, CString &szFolderPath) 
*
*	PURPOSE:	Call this function if for example you want to put the path of the folder
*				selected inside a combobox or an edit window. You would pass the
*				parameters from OnSelChanged() to this function along with a CString object
*				that will hold the folder path. If the path is not
*				in the filesystem(eg MyComputer) it returns false.
*
*	MESSAGEMAP:	TVN_SELCHANGED
*
****************************************************************************/
BOOL CShellTree::OnFolderSelected(NMHDR* pNMHDR, LRESULT* pResult, CString &szFolderPath) 
{
	// TODO: Add your control notification handler code here
	LPTVITEMDATA	lptvid;  //Long pointer to TreeView item data
	LPSHELLFOLDER	lpsf2=NULL;
	static char		szBuff[MAX_PATH];
	HRESULT			hr;
	BOOL			bRet=false;
	TV_SORTCB		tvscb;
	HTREEITEM		hItem=NULL;

	if((hItem = GetSelectedItem()))
	{
		lptvid=(LPTVITEMDATA)GetItemData(hItem);

		if (lptvid && lptvid->lpsfParent && lptvid->lpi)
		{
			hr=lptvid->lpsfParent->BindToObject(lptvid->lpi,
					 0,IID_IShellFolder,(LPVOID *)&lpsf2);

			if (SUCCEEDED(hr))
				{
					ULONG ulAttrs = SFGAO_FILESYSTEM;

					// Determine what type of object we have.
					lptvid->lpsfParent->GetAttributesOf(1, (const struct _ITEMIDLIST **)&lptvid->lpi, &ulAttrs);

					if (ulAttrs & (SFGAO_FILESYSTEM))
					{
						if(SHGetPathFromIDList(lptvid->lpifq,szBuff)){
							szFolderPath = szBuff;
							bRet = true;
						}
					}
					//non standard from here(NEW CODE)
					NM_TREEVIEW* pnmtv = (NM_TREEVIEW*)pNMHDR;
					if ((pnmtv->itemNew.cChildren == 1) && !(pnmtv->itemNew.state & TVIS_EXPANDEDONCE)){
						FillTreeView(lpsf2,lptvid->lpifq,pnmtv->itemNew.hItem);

						tvscb.hParent     = pnmtv->itemNew.hItem;
						tvscb.lParam      = 0;
						tvscb.lpfnCompare = TreeViewCompareProc;
						SortChildrenCB(&tvscb);
						
						pnmtv->itemNew.state |= TVIS_EXPANDEDONCE;
						pnmtv->itemNew.stateMask |= TVIS_EXPANDEDONCE;
						pnmtv->itemNew.mask |= TVIF_STATE;
						SetItem(&pnmtv->itemNew);
					}
				}

		}
		if(lpsf2)
			lpsf2->Release();
		
	}	
	*pResult = 0;
	return bRet;
}

/****************************************************************************
*
*	FUNCTION:	OnDeleteShellItem(NMHDR* pNMHDR, LRESULT* pResult)
*
*	PURPOSE:	Releases the memory allocated by the shell folders
*
*	MESSAGEMAP:	TVN_DELETEITEM
*
*	MISC:		failure to call this function will result in a memory leak
*
****************************************************************************/
void CShellTree::OnDeleteShellItem(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPTVITEMDATA lptvid=NULL;
	HRESULT hr;
	LPMALLOC lpMalloc;

	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	//Let's free the memory for the TreeView item data...
	hr=SHGetMalloc(&lpMalloc);
	if (FAILED(hr))
		return;
		
	lptvid=(LPTVITEMDATA)pNMTreeView->itemOld.lParam;
	lptvid->lpsfParent->Release();
	lpMalloc->Free(lptvid->lpi);  
	lpMalloc->Free(lptvid->lpifq);  
	lpMalloc->Free(lptvid);  
	lpMalloc->Release();
}

/****************************************************************************
*
*	FUNCTION:	EnableImages()
*
*	PURPOSE:	Obtains a handle to the system image list and attaches it
*				to the tree control. DO NOT DELETE the imagelist
*
*	MESSAGEMAP:	NONE
*
****************************************************************************/
void CShellTree::EnableImages()
{
	// Get the handle to the system image list, for our icons
    HIMAGELIST  hImageList;
    SHFILEINFO    sfi;

    hImageList = (HIMAGELIST)SHGetFileInfo((LPCSTR)_T("C:\\"),
                                           0,
                                           &sfi, 
                                           sizeof(SHFILEINFO), 
                                           SHGFI_SYSICONINDEX | SHGFI_SMALLICON);

    // Attach ImageList to TreeView
    if (hImageList)
        ::SendMessage(m_hWnd, TVM_SETIMAGELIST, (WPARAM) TVSIL_NORMAL,
            (LPARAM)hImageList);
}

/****************************************************************************
*
*	FUNCTION:	GetSelectedFolderPath(CString &szFolderPath)
*
*	PURPOSE:	Retrieves the path of the currently selected string.
*				Pass a CString object that will hold the folder path. 
*				If the path is not in the filesystem(eg MyComputer) 
*				or none is selected it returns false.
*
*	MESSAGEMAP:	NONE
*
****************************************************************************/
BOOL CShellTree::GetSelectedFolderPath(CString &szFolderPath)
{
	LPTVITEMDATA	lptvid;  //Long pointer to TreeView item data
	LPSHELLFOLDER	lpsf2=NULL;
	static char		szBuff[MAX_PATH];
	HTREEITEM		hItem=NULL;
	HRESULT			hr;
	BOOL			bRet=false;

	if((hItem = GetSelectedItem()))
	{
		lptvid=(LPTVITEMDATA)GetItemData(hItem);

		if (lptvid && lptvid->lpsfParent && lptvid->lpi)
		{
			hr=lptvid->lpsfParent->BindToObject(lptvid->lpi,
					 0,IID_IShellFolder,(LPVOID *)&lpsf2);

			if (SUCCEEDED(hr))
				{
					ULONG ulAttrs = SFGAO_FILESYSTEM;

					// Determine what type of object we have.
					lptvid->lpsfParent->GetAttributesOf(1, (const struct _ITEMIDLIST **)&lptvid->lpi, &ulAttrs);

					if (ulAttrs & (SFGAO_FILESYSTEM))
					{
						if(SHGetPathFromIDList(lptvid->lpifq,szBuff)){
							szFolderPath = szBuff;
							bRet = true;
						}
					}
				}

		}
		if(lpsf2)
			lpsf2->Release();
	}
	return bRet;
}

/****************************************************************************
*
*	FUNCTION:	GetParentShellFolder(HTREEITEM folderNode)
*
*	PURPOSE:	Retrieves the pointer to the ISHELLFOLDER interface
*				of the tree node passed as the paramter.
*
*	MESSAGEMAP:	NONE
*
****************************************************************************/
LPSHELLFOLDER CShellTree::GetParentShellFolder(HTREEITEM folderNode)
{
	LPTVITEMDATA	lptvid;  //Long pointer to TreeView item data

	lptvid=(LPTVITEMDATA)GetItemData(folderNode);
	if(lptvid)
		return lptvid->lpsfParent;
	else
		return NULL;
}

/****************************************************************************
*
*	FUNCTION:	GetRelativeIDLIST(HTREEITEM folderNode)
*
*	PURPOSE:	Retrieves the Pointer to an ITEMIDLIST structure that
*				identifies the subfolder relative to its parent folder.
*				see GetParentShellFolder();
*
*	MESSAGEMAP:	NONE
*
****************************************************************************/
LPITEMIDLIST CShellTree::GetRelativeIDLIST(HTREEITEM folderNode)
{
	LPTVITEMDATA	lptvid;  //Long pointer to TreeView item data

	lptvid=(LPTVITEMDATA)GetItemData(folderNode);
	if(lptvid)
		return lptvid->lpifq;
	else
		return NULL;
}

/****************************************************************************
*
*	FUNCTION:	GetFullyQualifiedIDLIST(HTREEITEM folderNode)
*
*	PURPOSE:	Retrieves the Retrieves the Pointer to an ITEMIDLIST
*				structure that identifies the subfolder relative to the
*				desktop. This is a fully qualified Item Identifier
*
*	MESSAGEMAP:	NONE
*
****************************************************************************/
LPITEMIDLIST CShellTree::GetFullyQualifiedID(HTREEITEM folderNode)
{
	LPTVITEMDATA	lptvid;  //Long pointer to TreeView item data

	lptvid=(LPTVITEMDATA)GetItemData(folderNode);
	if(lptvid)
		return lptvid->lpifq;
	else
		return NULL;
}

/****************************************************************************
*
*	FUNCTION:	SearchTree(	HTREEITEM treeNode,
*							CString szSearchName )
*
*	PURPOSE:	Too crude to explain, just use it
*
*	WARNING:	Only works if you use the default PopulateTree()
*				Not guaranteed to work on any future or existing
*				version of windows. Use with caution. Pretty much
*				ok if you're using on local drives
*
****************************************************************************/
bool CShellTree::SearchTree(HTREEITEM treeNode,
							CString szSearchName,
							FindAttribs attr)
{
	LPTVITEMDATA	lptvid;  //Long pointer to TreeView item data
	LPSHELLFOLDER	lpsf2=NULL;
	char	drive[_MAX_DRIVE];
	char	dir[_MAX_DIR];
	char	fname[_MAX_FNAME];
	char	ext[_MAX_EXT];
	bool	bRet=false;
	HRESULT	hr;
	CString	szCompare;

	szSearchName.MakeUpper();
	while(treeNode && bRet==false)
	{
		lptvid=(LPTVITEMDATA)GetItemData(treeNode);
		if (lptvid && lptvid->lpsfParent && lptvid->lpi)
		{
			hr=lptvid->lpsfParent->BindToObject(lptvid->lpi,
					 0,IID_IShellFolder,(LPVOID *)&lpsf2);
			if (SUCCEEDED(hr))
			{
				ULONG ulAttrs = SFGAO_FILESYSTEM;
				lptvid->lpsfParent->GetAttributesOf(1, (const struct _ITEMIDLIST **)&lptvid->lpi, &ulAttrs);
				if (ulAttrs & (SFGAO_FILESYSTEM))
				{
					if(SHGetPathFromIDList(lptvid->lpifq,szCompare.GetBuffer(MAX_PATH)))
					{
						switch(attr)
						{
						case type_drive:
							_splitpath(szCompare,drive,dir,fname,ext);
							szCompare=drive;
							break;
						case type_folder:
							szCompare = GetItemText(treeNode);
							break;
						}
						szCompare.MakeUpper();
						if(szCompare == szSearchName)
						{
							EnsureVisible(treeNode);
							SelectItem(treeNode);
							bRet=true;
						}
					}
				}
				lpsf2->Release();
			}
		}
		treeNode = GetNextSiblingItem(treeNode);
	}
	return bRet;
}

/****************************************************************************
*
*	FUNCTION:	TunnelTree(CString szFindPath)
*
*	PURPOSE:	Too crude to explain, just use it
*
*	WARNING:	Only works if you use the default PopulateTree()
*				Not guaranteed to work on any future or existing
*				version of windows. Use with caution. Pretty much
*				ok if you're using on local drives
*
****************************************************************************/
void CShellTree::TunnelTree(CString szFindPath)
{
	HTREEITEM subNode = GetRootItem();
	CString szPathHop;
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	char delimiter[]=_T("\\");

	CSHFileInfo checkPath(szFindPath);
	if(!checkPath.Exist())
	{
		MessageBox(szFindPath,_T("Folder not found"),MB_ICONERROR);
		return;
	}
		
	if(szFindPath.ReverseFind(_T('\\')) != szFindPath.GetLength()-1)
	{
		szFindPath += _T("\\");
	}

	_splitpath(szFindPath,drive,dir,fname,ext);

	//search the drive first
	szPathHop=drive;
	subNode=GetChildItem(subNode);
	if(subNode)
	{
		if(SearchTree(subNode,szPathHop, CShellTree::type_drive))
		{
			//break down subfolders and search
			char *p=strtok(dir,delimiter);
			while(p)
			{
				subNode = GetSelectedItem();
				subNode = GetChildItem(subNode);
				if(SearchTree(subNode,p,CShellTree::type_folder))
					p=strtok(NULL,delimiter);
				else
					p=NULL;
			}
		}
	}
}

/****************************************************************************
*
*	FUNCTION:	PopulateTree(LPCTSTR lpPath)
*
*	PURPOSE:	Populates tree based upon path.
*
*	AUTHOR:		Takehiko Mizoguti [mizoguti@m2.sys.to.casio.co.jp]
*
****************************************************************************/
void CShellTree::PopulateTree(LPCTSTR lpPath)
{

	LPSHELLFOLDER lpsf=NULL,lpsf2=NULL;
    LPITEMIDLIST  lpi=NULL;
    HRESULT hr;
    TV_SORTCB      tvscb;
	LPTSTR			lpFolder = (LPTSTR)lpPath;
	LPTSTR			lpNextFolder;
	TCHAR			strPath[_MAX_PATH];

	LPMALLOC pMalloc;
	if (::SHGetMalloc(&pMalloc) == NOERROR)
	{

		// Get a pointer to the desktop folder.
		hr=SHGetDesktopFolder(&lpsf);

		if (SUCCEEDED(hr))
		{
			USES_CONVERSION;

		   // Initialize the tree view to be empty.
			DeleteAllItems();

			do{

				// Get the Next Component
				lpNextFolder = PathFindNextComponent( lpFolder );
				if( lpNextFolder && *lpNextFolder ){
					memcpy( strPath, lpFolder, ( lpNextFolder - lpFolder ) );
					strPath[lpNextFolder - lpFolder] = _T('\0');
				}
				else{
					_tcscpy( strPath, lpFolder );
					lpNextFolder = NULL;
				}

				// Get ShellFolder Pidl
				ULONG eaten;
				hr = lpsf->ParseDisplayName( NULL, NULL, T2OLE(strPath), &eaten, &lpi, NULL );
				if( FAILED( hr ) ){
					break;
				}
				hr=lpsf->BindToObject(lpi, 0, IID_IShellFolder,(LPVOID *)&lpsf2);
				if( FAILED( hr ) ){
					break;
				}

				pMalloc->Free( lpi );

				// Release the Parent Folder pointer.
				lpsf->Release();

				// Chenge Folder Info
				lpsf = lpsf2;
				lpFolder = lpNextFolder;
			}
			while( lpNextFolder );

			FillTreeView(lpsf,NULL,TVI_ROOT);

		}
	}
    tvscb.hParent     = TVI_ROOT;
    tvscb.lParam      = 0;
    tvscb.lpfnCompare = TreeViewCompareProc;

    // Sort the items in the tree view
	SortChildrenCB(&tvscb/*, FALSE*/);
    
	HTREEITEM hItem;
	hItem = GetRootItem();
	Expand(hItem,TVE_EXPAND);
	Select(GetRootItem(),TVGN_CARET);
}