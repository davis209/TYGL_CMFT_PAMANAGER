# Microsoft Developer Studio Project File - Name="RadioTetraAgentUnitTest" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=RadioTetraAgentUnitTest - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "RadioTetraAgentUnitTest.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "RadioTetraAgentUnitTest.mak" CFG="RadioTetraAgentUnitTest - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "RadioTetraAgentUnitTest - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "RadioTetraAgentUnitTest - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "RadioTetraAgentUnitTest - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Rpcrt4.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "RadioTetraAgentUnitTest - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "./stub/" /I "../src/" /I "./src" /I "../../../../" /I "../../../../../../build/win32_n/transactive" /I "../../../../../../sdk/include/transactive" /I "../../../../../../sdk/include/transactive/core/exceptions/IDL/src" /I "../../../../../../sdk/include/transactive/core/message/IDL/src" /I "../../../../../../sdk\include\transactive\core\process_management\idl\src" /I "../../../../../../sdk/include/transactive\bus/security/duty_agent/IDL/src/" /I "../../../../../../sdk/include/transactive\bus/security/authentication_agent/IDL/src/" /I "../../../../../../sdk/include/cots/omniORB/omniORB_4.0.5/include" /I "../../../../../../sdk/include/cots/boost/boost_1_31_0" /I "../..\..\..\..\cots\ACE\5_3\ACE_wrappers" /I "../../../../../" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "__WIN32__" /D "__x86__" /D "__NT__" /D __OSVERSION__=4 /D TA_ASSERT_FAIL=EXCEPTION /D TA_ASSERT_LOG=1 /YX /FD /GZ /c
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 omniORB405_rtd.lib omniDynamic405_rtd.lib COS405_rtd.lib omnithread30_rtd.lib oci.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Rpcrt4.lib WS2_32.lib aced.lib TA_Base_Core_d.lib TA_Base_Bus_d.lib TA_IRS_Core_d.lib TA_IRS_Bus_d.lib RadioProtocolLibrary_d.lib  TA_Message_d.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /libpath:"../../../../../../build/win32_nd" /libpath:"../../../../../cots/omniORB/omniORB_4.0.5/lib/x86_win32" /libpath:"../../../../../cots/oracle/920/winnt/lib" /libpath:"../..\..\..\..\..\sdk\win32_nd" /libpath:"../..\..\..\..\..\sdk\lib" /libpath:"../../../../../build/win32_nd/"

!ENDIF 

# Begin Target

# Name "RadioTetraAgentUnitTest - Win32 Release"
# Name "RadioTetraAgentUnitTest - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\DataManager.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioCircuitModeManagerUnitTest.cpp
# End Source File
# Begin Source File

SOURCE=.\src\UnitTestMain.cpp
# End Source File
# Begin Source File

SOURCE=.\src\VersionGen.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\src\DataManager.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioCircuitModeManagerUnitTest.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Config"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cfg\UnitConfig.ini
# End Source File
# End Group
# Begin Group "stub"

# PROP Default_Filter "*.h *.cpp"
# Begin Source File

SOURCE=.\stub\Radio.cpp
# End Source File
# Begin Source File

SOURCE=.\stub\RadioCallback.cpp
# End Source File
# Begin Source File

SOURCE=.\stub\RadioCallback.h
# End Source File
# End Group
# Begin Group "External"

# PROP Default_Filter "*.h *.cpp"
# Begin Source File

SOURCE=..\src\RadioCircuitModeManager.cpp
# End Source File
# End Group
# End Target
# End Project
