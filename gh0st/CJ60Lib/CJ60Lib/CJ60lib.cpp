// CJ60Lib.cpp : Defines the initialization routines for the DLL.
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
//			6.00	17 Oct 1998	- Initial re-write and release.
//			6.02	17 Jan 1999	- Added validation check for ComCtl32.dll.
//			6.04	21 Jan 1999 - New release posted at codeguru.com.
//			6.05	22 Jan 1999 - Fixed the following problems with release 6.04:
//			                    - Added included file <shlobj.h> to SHFileInfo.cpp.
//								- Fixed build problem with Explorer and Outlook
//								  examples.
//								- Fixed static build problems.
// ==========================================================================
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <afxdllx.h>
#include "ModulVer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

bool GetVerComCtl32();

static AFX_EXTENSION_MODULE CJ60LibDLL = { NULL, NULL };

extern "C" int APIENTRY

DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0(_T("CJ60LIB.DLL Initializing!\n"));

		if (!GetVerComCtl32())
			return 0;
		
		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(CJ60LibDLL, hInstance))
			return 0;

		// Insert this DLL into the resource chain
		// NOTE: If this Extension DLL is being implicitly linked to by
		//  an MFC Regular DLL (such as an ActiveX Control)
		//  instead of an MFC application, then you will want to
		//  remove this line from DllMain and put it in a separate
		//  function exported from this Extension DLL.  The Regular DLL
		//  that uses this Extension DLL should then explicitly call that
		//  function to initialize this Extension DLL.  Otherwise,
		//  the CDynLinkLibrary object will not be attached to the
		//  Regular DLL's resource chain, and serious problems will
		//  result.

		new CDynLinkLibrary(CJ60LibDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0(_T("CJ60LIB.DLL Terminating!\n"));
		// Terminate the library before destructors are called
		AfxTermExtensionModule(CJ60LibDLL);
	}
	return 1;   // ok
}

bool GetVerComCtl32()
{
	CModuleVersion ver;
	DLLVERSIONINFO dvi;
	VERIFY(ver.DllGetVersion(_T("comctl32.dll"), dvi));
	int iVerComCtl32 = dvi.dwMajorVersion*100 + dvi.dwMinorVersion;

	if (iVerComCtl32 < 471)
	{
		CString str;
		str.Format(_T("Copyright © 1998-99 Kirk Stowell\nmailto:kstowell@codejockeys.com - http://www.codejockeys.com/kstowell/\n\nCode Jockey's CJ60 extension library requires ComCtl32.dll, version 4.71 or later. The curent version found on this system is %d.%d.%d. To get the latest version of ComCtl32.dll visit Microsoft's site at:\n\nhttp://www.microsoft.com/msdn/downloads/files/40comupd.htm"), dvi.dwMajorVersion, dvi.dwMinorVersion, dvi.dwBuildNumber);
		AfxMessageBox(str, MB_ICONSTOP);
		return false;
	}
	
	return true;
}
