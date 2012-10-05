// CJ60Lib.h : header file
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
//
//////////////////////////////////////////////////////////////////////

#ifndef __CJ60LIB_H__
#define __CJ60LIB_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXTEMPL_H__
#include <afxtempl.h>
#endif

#ifndef __AFXPRIV_H__
#include <afxpriv.h>
#endif

#ifdef _AFXDLL
#ifdef _DEBUG
#pragma comment(lib,"CJ60Libd.lib") 
#pragma message("Automatically linking with CJ60Libd.dll")
#else
#pragma comment(lib,"CJ60Lib.lib") 
#pragma message("Automatically linking with CJ60Lib.dll") 
#endif
#else
#ifdef _DEBUG
#pragma comment(lib,"CJ60StaticLibd.lib") 
#pragma message("Automatically linking with CJ60StaticLibd.lib") 
#else
#pragma comment(lib,"CJ60StaticLib.lib") 
#pragma message("Automatically linking with CJ60StaticLib.lib") 
#endif
#endif

#ifndef __SEARCHEDIT_H__
#include "CJSearchEdit.h"
#endif

#ifndef __CJCAPTION_H__
#include "CJCaption.h"
#endif

#ifndef __CJCONTROLBAR_H__
#include "CJControlBar.h"
#endif

#ifndef __CJDOCKBAR_H__
#include "CJDockBar.h"
#endif

#ifndef __CJDOCKCONTEXT_H__
#include "CJDockContext.h"
#endif

#ifndef __CJEXPLORERBAR_H__
#include "CJExplorerBar.h"
#endif

#ifndef __CJFLATBUTTON_H__
#include "CJFlatButton.h"
#endif

#ifndef __CJCOMBOBOX_H__
#include "CJFlatComboBox.h"
#endif

#ifndef __CJFLATHEADERCTRL_H__
#include "CJFlatHeaderCtrl.h"
#endif

#ifndef __CJFRAMEWND_H__
#include "CJFrameWnd.h"
#endif

#ifndef __CJLISTCTRL_H__
#include "CJListCtrl.h"
#endif

#ifndef __CJLISTVIEW_H__
#include "CJListView.h"
#endif

#ifndef __CJMDIFRAMEWND_H__
#include "CJMDIFrameWnd.h"
#endif

#ifndef __CJMINIDOCKFRAMEWND_H__
#include "CJMiniDockFrameWnd.h"
#endif

#ifndef __CJOUTLOOKBAR_H__
#include "CJOutlookBar.h"
#endif

#ifndef __CJPAGERCTRL_H__
#include "CJPagerCtrl.h"
#endif

#ifndef __CJSIZEDOCKBAR_H__
#include "CJSizeDockBar.h"
#endif

#ifndef __CJTABCTRLBAR_H__
#include "CJTabCtrlBar.h"
#endif

#ifndef __CJTOOLBAR_H__
#include "CJToolBar.h"
#endif

#ifndef __COOLBAR_H
#include "CoolBar.h"
#endif

#ifndef __COOLMENU_H__
#include "CoolMenu.h"
#endif

#ifndef __FIXTB_H
#include "FixTB.h"
#endif

#ifndef __FLATBAR_H
#include "FlatBar.h"
#endif

#ifndef __HYPERLINK_H__
#include "HyperLink.h"
#endif

#ifndef __MENUBAR_H
#include "MenuBar.h"
#endif

#ifndef __MODULEVER_H
#include "ModulVer.h"
#endif

#ifndef __SHELLPIDL_H__
#include "ShellPidl.h"
#endif

#ifndef __SHELLTREE_H__
#include "ShellTree.h"
#endif

#ifndef __SHFILEINFO_H__
#include "SHFileInfo.h"
#endif

#ifndef _SUBCLASSW_H
#include "Subclass.h"
#endif

#ifndef __CJTABVIEW_H__
#include "CJTabView.h"
#endif

#endif // __CJ60LIB_H__