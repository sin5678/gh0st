# Microsoft Developer Studio Project File - Name="CJ60Lib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=CJ60Lib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CJ60Lib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CJ60Lib.mak" CFG="CJ60Lib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CJ60Lib - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "CJ60Lib - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Src/CJ60Lib", KCAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CJ60Lib - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Od /I "../Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386 /out:"../Lib/CJ60Lib.dll" /implib:"../Lib/CJ60Lib.lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "CJ60Lib - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /GX /ZI /Od /I "../Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../Lib/CJ60Libd.dll" /implib:"../Lib/CJ60Libd.lib" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "CJ60Lib - Win32 Release"
# Name "CJ60Lib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CJ60Lib.cpp
# End Source File
# Begin Source File

SOURCE=.\CJ60Lib.def
# End Source File
# Begin Source File

SOURCE=.\CJ60Lib.rc
# End Source File
# Begin Source File

SOURCE=.\CJCaption.cpp
# End Source File
# Begin Source File

SOURCE=.\CJControlBar.cpp
# End Source File
# Begin Source File

SOURCE=.\CJDockBar.cpp
# End Source File
# Begin Source File

SOURCE=.\CJDockContext.cpp
# End Source File
# Begin Source File

SOURCE=.\CJExplorerBar.cpp
# End Source File
# Begin Source File

SOURCE=.\CJFlatButton.cpp
# End Source File
# Begin Source File

SOURCE=.\CJFlatComboBox.cpp
# End Source File
# Begin Source File

SOURCE=.\CJFlatHeaderCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\CJFrameInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\CJFrameWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\CJListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\CJListView.cpp
# End Source File
# Begin Source File

SOURCE=.\CJMDIFrameWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\CJMiniDockFrameWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\CJOutlookBar.cpp
# End Source File
# Begin Source File

SOURCE=.\CJPagerCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\CJSearchEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\CJSizeDockBar.cpp
# End Source File
# Begin Source File

SOURCE=.\CJTabCtrlBar.cpp
# End Source File
# Begin Source File

SOURCE=.\CJTabView.cpp
# End Source File
# Begin Source File

SOURCE=.\CJToolBar.cpp
# End Source File
# Begin Source File

SOURCE=.\CoolBar.cpp
# End Source File
# Begin Source File

SOURCE=.\CoolMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\FixTB.cpp
# End Source File
# Begin Source File

SOURCE=.\FlatBar.cpp
# End Source File
# Begin Source File

SOURCE=.\Globals.cpp
# End Source File
# Begin Source File

SOURCE=.\HyperLink.cpp
# End Source File
# Begin Source File

SOURCE=.\MenuBar.cpp
# End Source File
# Begin Source File

SOURCE=.\ModulVer.cpp
# End Source File
# Begin Source File

SOURCE=.\ShellPidl.cpp
# End Source File
# Begin Source File

SOURCE=.\ShellTree.cpp
# End Source File
# Begin Source File

SOURCE=.\SHFileInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Subclass.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\Include\CJ60Lib.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJCaption.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJControlBar.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJDockBar.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJDockContext.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJExplorerBar.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJFlatButton.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJFlatComboBox.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJFlatHeaderCtrl.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJFrameInfo.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJFrameWnd.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJListCtrl.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJListView.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJMDIFrameWnd.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJMiniDockFrameWnd.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJOutlookBar.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJPagerCtrl.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJSearchEdit.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJSizeDockBar.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJTabCtrlBar.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJTabView.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJToolBar.h
# End Source File
# Begin Source File

SOURCE=..\Include\CoolBar.h
# End Source File
# Begin Source File

SOURCE=..\Include\CoolMenu.h
# End Source File
# Begin Source File

SOURCE=..\Include\FixTB.h
# End Source File
# Begin Source File

SOURCE=..\Include\FlatBar.h
# End Source File
# Begin Source File

SOURCE=.\Globals.h
# End Source File
# Begin Source File

SOURCE=..\Include\HyperLink.h
# End Source File
# Begin Source File

SOURCE=..\Include\MenuBar.h
# End Source File
# Begin Source File

SOURCE=..\Include\ModulVer.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=..\Include\ShellPidl.h
# End Source File
# Begin Source File

SOURCE=..\Include\ShellTree.h
# End Source File
# Begin Source File

SOURCE=..\Include\SHFileInfo.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=..\Include\Subclass.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\btn_arro.bmp
# End Source File
# Begin Source File

SOURCE=.\res\btn_explorer.bmp
# End Source File
# Begin Source File

SOURCE=.\res\button_images.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CJ60Lib.rc2
# End Source File
# Begin Source File

SOURCE=.\res\cj_logo.bmp
# End Source File
# Begin Source File

SOURCE=.\res\hsplitba.cur
# End Source File
# Begin Source File

SOURCE=.\res\vsplitba.cur
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
