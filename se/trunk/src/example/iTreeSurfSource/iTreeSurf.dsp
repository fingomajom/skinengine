# Microsoft Developer Studio Project File - Name="iTreeSurf" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=iTreeSurf - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "iTreeSurf.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "iTreeSurf.mak" CFG="iTreeSurf - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "iTreeSurf - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "iTreeSurf - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "iTreeSurf - Win32 Dynamic Link Release" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "iTreeSurf - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\iTreeSurfResult\Intermediate\Release\iTreeSurf"
# PROP Intermediate_Dir "..\iTreeSurfResult\Intermediate\Release\iTreeSurf"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 Wsock32.lib pdh.lib Delayimp.lib /nologo /subsystem:windows /pdb:none /machine:I386 /DELAYLOAD:pdh.dll
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy "..\iTreeSurfResult\Intermediate\Release\iTreeSurf\iTreeSurf.exe" "..\iTreeSurfResult\iTreeSurf"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "iTreeSurf - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\iTreeSurfResult\Intermediate\Debug\iTreeSurf"
# PROP Intermediate_Dir "..\iTreeSurfResult\Intermediate\Debug\iTreeSurf"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ws2_32.lib pdh.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "iTreeSurf - Win32 Dynamic Link Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "iTreeSurf___Win32_Dynamic_Link_Release"
# PROP BASE Intermediate_Dir "iTreeSurf___Win32_Dynamic_Link_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\iTreeSurfResult\Intermediate\Release\iTreeSurfDynamicLink"
# PROP Intermediate_Dir "..\iTreeSurfResult\Intermediate\Release\iTreeSurfDynamicLink"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_ITREE_SURF_DYNAMIC" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 Wsock32.lib pdh.lib Delayimp.lib /nologo /subsystem:windows /pdb:none /machine:I386 /DELAYLOAD:pdh.dll
# ADD LINK32 Wsock32.lib wininet.lib pdh.lib Delayimp.lib /nologo /subsystem:windows /pdb:none /machine:I386 /DELAYLOAD:pdh.dll
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy "..\iTreeSurfResult\Intermediate\Release\iTreeSurfDynamicLink\iTreeSurf.exe" "..\iTreeSurfResult\DynamicLinkRelease"
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "iTreeSurf - Win32 Release"
# Name "iTreeSurf - Win32 Debug"
# Name "iTreeSurf - Win32 Dynamic Link Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AddressBarEx.cpp
# End Source File
# Begin Source File

SOURCE=.\AddToThisFavDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AdvancedProp.cpp
# End Source File
# Begin Source File

SOURCE=.\AdvTabCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\AnimateIcon.cpp
# End Source File
# Begin Source File

SOURCE=.\BaseExpObj.cpp
# End Source File
# Begin Source File

SOURCE=.\ChevBar.cpp
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\CollectorDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ConfigTable.cpp
# End Source File
# Begin Source File

SOURCE=.\Crypt.cpp
# End Source File
# Begin Source File

SOURCE=.\CustomizeToolbar.cpp
# End Source File
# Begin Source File

SOURCE=.\custsite.cpp
# End Source File
# Begin Source File

SOURCE=.\ExternalUtilItem.cpp
# End Source File
# Begin Source File

SOURCE=.\ExternalUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\FavTree.cpp
# End Source File
# Begin Source File

SOURCE=.\FavTreeCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\FavWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\FileName.cpp
# End Source File
# Begin Source File

SOURCE=.\FixedHtmlView.cpp
# End Source File
# Begin Source File

SOURCE=.\GlobalSpace.cpp
# End Source File
# Begin Source File

SOURCE=.\GroupUrlDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\HistoryTree.cpp
# End Source File
# Begin Source File

SOURCE=.\HisTreeCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\HtmlWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\idispimp.cpp
# End Source File
# Begin Source File

SOURCE=.\InputAlias.cpp
# End Source File
# Begin Source File

SOURCE=.\InputPassword.cpp
# End Source File
# Begin Source File

SOURCE=.\iTreeSurf.cpp
# End Source File
# Begin Source File

SOURCE=.\iTreeSurf.idl
# ADD MTL /h "iTreeSurf_i.h" /iid "iTreeSurf_i.c" /Oicf
# End Source File
# Begin Source File

SOURCE=.\iTreeSurf.rc
# End Source File
# Begin Source File

SOURCE=.\iTreeSurfHelper.cpp
# End Source File
# Begin Source File

SOURCE=.\iTreeSurfNSHandle.cpp
# End Source File
# Begin Source File

SOURCE=.\iTreeSurfOptions.cpp
# End Source File
# Begin Source File

SOURCE=.\iTreeSurfView.cpp
# End Source File
# Begin Source File

SOURCE=.\LanguageTable.cpp
# End Source File
# Begin Source File

SOURCE=.\LinkListDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\MenuBar.cpp
# End Source File
# Begin Source File

SOURCE=.\MenuData.cpp
# End Source File
# Begin Source File

SOURCE=.\Menuorder.cpp
# End Source File
# Begin Source File

SOURCE=.\OrgGroupDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\perfmon.cpp
# End Source File
# Begin Source File

SOURCE=.\PopupDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ProxyInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\ProxyList.cpp
# End Source File
# Begin Source File

SOURCE=.\QuickSearchDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\rasspdm.cpp
# End Source File
# Begin Source File

SOURCE=.\Resourcebar.cpp
# End Source File
# Begin Source File

SOURCE=.\scbarcf.cpp
# End Source File
# Begin Source File

SOURCE=.\scbarg.cpp
# End Source File
# Begin Source File

SOURCE=.\searchbar.cpp
# End Source File
# Begin Source File

SOURCE=.\SelProxyCatDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SetAliasDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SetProxy.cpp
# End Source File
# Begin Source File

SOURCE=.\SetRefIntervalDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SetSCDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ShellPidl.cpp
# End Source File
# Begin Source File

SOURCE=.\ShellTree.cpp
# End Source File
# Begin Source File

SOURCE=.\ShellTreeCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\ShellWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\SimpleSAH.cpp
# End Source File
# Begin Source File

SOURCE=.\sizecbar.cpp
# End Source File
# Begin Source File

SOURCE=.\StatusBarWithProgress.Cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\StickName.cpp
# End Source File
# Begin Source File

SOURCE=.\TabDropTarget.cpp
# End Source File
# Begin Source File

SOURCE=.\TextProgressCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\UrlFilterProp.cpp
# End Source File
# Begin Source File

SOURCE=.\webbrowser2.cpp
# End Source File
# Begin Source File

SOURCE=.\WebFileOpenDlg.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AddressBarEx.h
# End Source File
# Begin Source File

SOURCE=.\AddToThisFavDlg.h
# End Source File
# Begin Source File

SOURCE=.\AdvancedProp.h
# End Source File
# Begin Source File

SOURCE=.\AdvTabCtrl.h
# End Source File
# Begin Source File

SOURCE=.\AnimateIcon.h
# End Source File
# Begin Source File

SOURCE=.\BaseExpObj.h
# End Source File
# Begin Source File

SOURCE=.\ChevBar.h
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.h
# End Source File
# Begin Source File

SOURCE=.\CollectorDlg.h
# End Source File
# Begin Source File

SOURCE=.\ConfigTable.h
# End Source File
# Begin Source File

SOURCE=.\Crypt.h
# End Source File
# Begin Source File

SOURCE=.\CustomizeToolbar.h
# End Source File
# Begin Source File

SOURCE=.\custsite.h
# End Source File
# Begin Source File

SOURCE=.\ExternalUtilItem.h
# End Source File
# Begin Source File

SOURCE=.\ExternalUtils.h
# End Source File
# Begin Source File

SOURCE=.\FavTree.h
# End Source File
# Begin Source File

SOURCE=.\FavTreeCtrl.h
# End Source File
# Begin Source File

SOURCE=.\FavWnd.h
# End Source File
# Begin Source File

SOURCE=.\FileName.h
# End Source File
# Begin Source File

SOURCE=.\FixedHtmlView.h
# End Source File
# Begin Source File

SOURCE=.\GlobalSpace.h
# End Source File
# Begin Source File

SOURCE=.\GroupUrlDlg.h
# End Source File
# Begin Source File

SOURCE=.\HistoryTree.h
# End Source File
# Begin Source File

SOURCE=.\HisTreeCtrl.h
# End Source File
# Begin Source File

SOURCE=.\HtmlWnd.h
# End Source File
# Begin Source File

SOURCE=.\idispimp.h
# End Source File
# Begin Source File

SOURCE=.\InputAlias.h
# End Source File
# Begin Source File

SOURCE=.\InputPassword.h
# End Source File
# Begin Source File

SOURCE=.\iTreeSurf.h
# End Source File
# Begin Source File

SOURCE=.\iTreeSurfHelper.h
# End Source File
# Begin Source File

SOURCE=.\iTreeSurfNSHandle.h
# End Source File
# Begin Source File

SOURCE=.\iTreeSurfOptions.h
# End Source File
# Begin Source File

SOURCE=.\iTreeSurfView.h
# End Source File
# Begin Source File

SOURCE=.\LanguageTable.h
# End Source File
# Begin Source File

SOURCE=.\LinkListDlg.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\MenuBar.h
# End Source File
# Begin Source File

SOURCE=.\MenuData.h
# End Source File
# Begin Source File

SOURCE=.\Menuorder.h
# End Source File
# Begin Source File

SOURCE=.\OrgGroupDlg.h
# End Source File
# Begin Source File

SOURCE=.\perfmon.h
# End Source File
# Begin Source File

SOURCE=.\PopupDlg.h
# End Source File
# Begin Source File

SOURCE=.\ProxyInfo.h
# End Source File
# Begin Source File

SOURCE=.\ProxyList.h
# End Source File
# Begin Source File

SOURCE=.\QuickSearchDlg.h
# End Source File
# Begin Source File

SOURCE=.\rasspdm.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\Resourcebar.h
# End Source File
# Begin Source File

SOURCE=.\scbarcf.h
# End Source File
# Begin Source File

SOURCE=.\scbarg.h
# End Source File
# Begin Source File

SOURCE=.\searchbar.h
# End Source File
# Begin Source File

SOURCE=.\SelProxyCatDlg.h
# End Source File
# Begin Source File

SOURCE=.\SetAliasDlg.h
# End Source File
# Begin Source File

SOURCE=.\SetProxy.h
# End Source File
# Begin Source File

SOURCE=.\SetRefIntervalDlg.h
# End Source File
# Begin Source File

SOURCE=.\SetSCDlg.h
# End Source File
# Begin Source File

SOURCE=.\ShellPidl.h
# End Source File
# Begin Source File

SOURCE=.\ShellTree.h
# End Source File
# Begin Source File

SOURCE=.\ShellTreeCtrl.h
# End Source File
# Begin Source File

SOURCE=.\ShellWnd.h
# End Source File
# Begin Source File

SOURCE=.\SimpleSAH.h
# End Source File
# Begin Source File

SOURCE=.\sizecbar.h
# End Source File
# Begin Source File

SOURCE=.\StatusBarWithProgress.H
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\StickName.h
# End Source File
# Begin Source File

SOURCE=.\TabDropTarget.h
# End Source File
# Begin Source File

SOURCE=.\TextProgressCtrl.h
# End Source File
# Begin Source File

SOURCE=.\ToolMenu.h
# End Source File
# Begin Source File

SOURCE=.\TxtDropTarget.h
# End Source File
# Begin Source File

SOURCE=.\UrlFilterProp.h
# End Source File
# Begin Source File

SOURCE=.\webbrowser2.h
# End Source File
# Begin Source File

SOURCE=.\WebFileOpenDlg.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\coldtool.bmp
# End Source File
# Begin Source File

SOURCE=.\res\coldtoolsm.bmp
# End Source File
# Begin Source File

SOURCE=.\res\coolfavtools.bmp
# End Source File
# Begin Source File

SOURCE=.\res\coolgo.bmp
# End Source File
# Begin Source File

SOURCE=.\res\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\res\hotfavtools.bmp
# End Source File
# Begin Source File

SOURCE=.\res\hotgo.bmp
# End Source File
# Begin Source File

SOURCE=.\res\hottoolb.bmp
# End Source File
# Begin Source File

SOURCE=.\res\hottoolbsm.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon2.ico
# End Source File
# Begin Source File

SOURCE=.\res\icons.bmp
# End Source File
# Begin Source File

SOURCE=.\res\idr_itreesurf.ico
# End Source File
# Begin Source File

SOURCE=.\res\iTreeSurf.ico
# End Source File
# Begin Source File

SOURCE=.\iTreeSurf.rgs
# End Source File
# Begin Source File

SOURCE=.\res\iTreeSurfDoc.ico
# End Source File
# Begin Source File

SOURCE=.\iTreeSurfNSHandle.rgs
# End Source File
# Begin Source File

SOURCE=.\res\iTreeSurfNSHandle.rgs
# End Source File
# Begin Source File

SOURCE=.\res\minitoolhot.bmp
# End Source File
# Begin Source File

SOURCE=.\res\minitools.bmp
# End Source File
# Begin Source File

SOURCE=.\res\placehold.gif
# End Source File
# Begin Source File

SOURCE=.\res\proxy.bmp
# End Source File
# Begin Source File

SOURCE=.\res\refresh.bmp
# End Source File
# Begin Source File

SOURCE=.\res\secure.ico
# End Source File
# Begin Source File

SOURCE=.\res\status.bmp
# End Source File
# Begin Source File

SOURCE=.\res\sysmenu.bmp
# End Source File
# Begin Source File

SOURCE=.\res\titleicons.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\treelist.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\_Bug.txt
# End Source File
# Begin Source File

SOURCE=.\_History.txt
# End Source File
# Begin Source File

SOURCE=.\_iTreeSays.txt
# End Source File
# Begin Source File

SOURCE=.\_Structure.txt
# End Source File
# Begin Source File

SOURCE=.\_ToDo.txt
# End Source File
# Begin Source File

SOURCE=.\iTreeSurfDefinition.h
# End Source File
# End Target
# End Project
# Section iTreeSurf : {01710002-7E02-0173-02BA-580102187D01}
# 	1:8:IDR_ITREESURF:102
# End Section
