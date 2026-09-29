# Microsoft Developer Studio Project File - Name="app.signs.stis_manager.STISManager" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=app.signs.stis_manager.STISManager - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "app.signs.stis_manager.STISManager.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "app.signs.stis_manager.STISManager.mak" CFG="app.signs.stis_manager.STISManager - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "app.signs.stis_manager.STISManager - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "app.signs.stis_manager.STISManager - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "app.signs.stis_manager.STISManager - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../../../bin/win32_nr"
# PROP Intermediate_Dir "../../../../../build/win32_nr/transactive/app/signs/stis_manager"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "..\..\.." /I "..\..\..\.." /I "../../../../../sdk/include/transactive/bus/signs_4669/tis_agent_access/src" /I "..\..\..\..\..\sdk\include\transactive" /I "..\..\..\..\..\sdk\include\transactive\core\exceptions\IDL\src" /I "..\..\..\..\..\sdk\include\transactive\core\message\IDL\src" /I "..\..\..\..\..\build\win32_n\transactive" /I "..\..\..\..\..\sdk\include\transactive\core\data_access_interface\tis_agent_4669\IDL\src" /I "..\..\..\..\..\sdk\omniORB\omniORB_4.0.5\include" /I "..\..\..\..\cots\ssce\sdk\include" /I "../../../../../sdk/include/cots/boost/boost_1_31_0" /I "../../../../../sdk\include\transactive\core\data_access_interface\tis_agent_4669\IDL\src" /I "../../../../../sdk/include/transactive/bus/scada/datapointcorbadef/src" /I "..\..\..\..\..\sdk\include\cots\ACE\5_3\ACE_wrappers" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "__WIN32__" /D "__x86__" /D "__NT__" /D __OSVERSION__=4 /D _WIN32_WINNT=0x0500 /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc09 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0xc09 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 acemfc.lib TA_Base_Bus.lib TA_IRS_Core.lib TA_IRS_Bus.lib ssce5432.lib htmlhelp.lib winmm.lib omniORB405_rt.lib omniDynamic405_rt.lib omnithread30_rt.lib COS405_rt.lib TA_Base_Core.lib oci.lib Rpcrt4.lib stracelib.lib /nologo /subsystem:windows /pdb:none /machine:I386 /out:"../../../../../bin/win32_nr/STISManager.exe" /libpath:"..\..\..\..\cots\ssce\sdk\lib" /libpath:"..\..\..\..\..\sdk\lib" /libpath:"..\..\..\..\..\sdk\win32_nr" /libpath:"..\..\..\..\..\build\win32_nr" /libpath:"../../../../../sdk/omniORB/omniORB_4.0.5/lib/x86_win32" /libpath:"../../../../../sdk/include/cots/WinStackTrace\build\Release"

!ELSEIF  "$(CFG)" == "app.signs.stis_manager.STISManager - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../../../bin/win32_nd"
# PROP Intermediate_Dir "../../../../../build/win32_nd/transactive/app/signs/stis_manager"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\..\.." /I "..\..\..\.." /I "../../../../../sdk/include/transactive/bus/signs_4669/tis_agent_access/src" /I "..\..\..\..\..\sdk\include\transactive" /I "..\..\..\..\..\sdk\include\transactive\core\exceptions\IDL\src" /I "..\..\..\..\..\sdk\include\transactive\core\message\IDL\src" /I "..\..\..\..\..\build\win32_n\transactive" /I "..\..\..\..\..\sdk\include\transactive\core\data_access_interface\tis_agent_4669\IDL\src" /I "..\..\..\..\..\sdk\omniORB\omniORB_4.0.5\include" /I "..\..\..\..\cots\ssce\sdk\include" /I "../../../../../sdk/include/cots/boost/boost_1_31_0" /I "../../../../../sdk\include\transactive\core\data_access_interface\tis_agent_4669\IDL\src" /I "../../../../../sdk/include/transactive/bus/scada/datapointcorbadef/src" /I "..\..\..\..\..\sdk\include\cots\ACE\5_3\ACE_wrappers" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "__WIN32__" /D "__x86__" /D "__NT__" /D __OSVERSION__=4 /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc09 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0xc09 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 COS405_rtd.lib omniORB405_rtd.lib omnithread30_rtd.lib omniDynamic405_rtd.lib acemfcd.lib TA_Base_Core_d.lib TA_Base_Bus_d.lib TA_IRS_Core_d.lib TA_IRS_Bus_d.lib ssce5432.lib Rpcrt4.lib oci.lib htmlhelp.lib winmm.lib stracelib.lib /nologo /subsystem:windows /debug /debugtype:both /machine:I386 /out:"../../../../../bin/win32_nd/STISManager.exe" /pdbtype:sept /libpath:"..\..\..\..\cots\ssce\sdk\lib" /libpath:"../../../../../sdk/omniORB/omniORB_4.0.5/lib/x86_win32" /libpath:"..\..\..\..\..\sdk\lib" /libpath:"..\..\..\..\..\sdk\win32_nd" /libpath:"..\..\..\..\..\build\win32_nd" /libpath:"../../../../../sdk/include/cots/WinStackTrace\build\Debug"

!ENDIF 

# Begin Target

# Name "app.signs.stis_manager.STISManager - Win32 Release"
# Name "app.signs.stis_manager.STISManager - Win32 Debug"
# Begin Group "src"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\GraphworxComms.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GraphworxComms.h
# End Source File
# Begin Source File

SOURCE=.\src\InitProgressDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\LibraryVersionMonitor.cpp
# End Source File
# Begin Source File

SOURCE=.\src\LibraryVersionMonitor.h
# End Source File
# Begin Source File

SOURCE=.\src\MainTab.cpp
# End Source File
# Begin Source File

SOURCE=.\src\MainTab.h
# End Source File
# Begin Source File

SOURCE=.\src\PIDController.cpp
# End Source File
# Begin Source File

SOURCE=.\src\PIDController.h
# End Source File
# Begin Source File

SOURCE=.\src\REBProgressManager.cpp
# End Source File
# Begin Source File

SOURCE=.\src\REBProgressManager.h
# End Source File
# Begin Source File

SOURCE=.\src\Resource.h
# End Source File
# Begin Source File

SOURCE=.\src\RightsManager.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RightsManager.h
# End Source File
# Begin Source File

SOURCE=.\src\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=.\src\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\src\STISManager.cpp
# End Source File
# Begin Source File

SOURCE=.\src\STISManager.h
# End Source File
# Begin Source File

SOURCE=.\src\STISManager.rc
# End Source File
# Begin Source File

SOURCE=.\src\STISManagerDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\STISManagerDlg.h
# End Source File
# Begin Source File

SOURCE=.\src\STISManagerGUI.cpp
# End Source File
# Begin Source File

SOURCE=.\src\STISManagerGUI.h
# End Source File
# Begin Source File

SOURCE=.\src\STISPredefinedMessages.cpp
# End Source File
# Begin Source File

SOURCE=.\src\STISPredefinedMessages.h
# End Source File
# Begin Source File

SOURCE=.\src\UserMessages.cpp
# End Source File
# Begin Source File

SOURCE=.\src\UserMessages.h
# End Source File
# Begin Source File

SOURCE=.\src\VersionGen.cpp
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\STISManager.ico
# End Source File
# Begin Source File

SOURCE=.\res\STISManager.rc2
# End Source File
# End Group
# Begin Group "Display Page"

# PROP Default_Filter ""
# Begin Group "PID selection"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\CreateNewGroupDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Createnewgroupdlg.h
# End Source File
# Begin Source File

SOURCE=.\src\IPidSelectionListener.h
# End Source File
# Begin Source File

SOURCE=.\src\PidGroupCombo.cpp
# End Source File
# Begin Source File

SOURCE=.\src\PidGroupCombo.h
# End Source File
# Begin Source File

SOURCE=.\src\PidListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\src\PidListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\src\PidSelectionManager.cpp
# End Source File
# Begin Source File

SOURCE=.\src\PidSelectionManager.h
# End Source File
# End Group
# Begin Group "Message Selection"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\FreeTextPage.cpp
# End Source File
# Begin Source File

SOURCE=.\src\FreeTextPage.h
# End Source File
# Begin Source File

SOURCE=.\src\IMessageSelectionListener.h
# End Source File
# Begin Source File

SOURCE=.\src\MessageTypeTab.cpp
# End Source File
# Begin Source File

SOURCE=.\src\MessageTypeTab.h
# End Source File
# Begin Source File

SOURCE=.\src\PredefinedPage.cpp
# End Source File
# Begin Source File

SOURCE=.\src\PredefinedPage.h
# End Source File
# Begin Source File

SOURCE=.\src\RATISPage.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RATISPage.h
# End Source File
# End Group
# Begin Group "Time Controls"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\CallbackDateTimeCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CallbackDateTimeCtrl.h
# End Source File
# Begin Source File

SOURCE=.\src\IDateTimeListener.h
# End Source File
# Begin Source File

SOURCE=.\src\TimeControlManager.cpp
# End Source File
# Begin Source File

SOURCE=.\src\TimeControlManager.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\src\CallbackButton.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CallbackButton.h
# End Source File
# Begin Source File

SOURCE=.\src\DisplayPage.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DisplayPage.h
# End Source File
# Begin Source File

SOURCE=.\src\IButtonListener.h
# End Source File
# End Group
# Begin Group "Versions Page"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\LibraryVersionListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\src\LibraryVersionListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\src\LibraryVersionPage.cpp
# End Source File
# Begin Source File

SOURCE=.\src\LibraryVersionPage.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\src\InitProgressDlg.h
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
