# Microsoft Developer Studio Project File - Name="app.radio.RadioTetraAgent.RadioTetraAgent" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=app.radio.RadioTetraAgent.RadioTetraAgent - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "app.radio.RadioTetraAgent.RadioTetraAgent.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "app.radio.RadioTetraAgent.RadioTetraAgent.mak" CFG="app.radio.RadioTetraAgent.RadioTetraAgent - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "app.radio.RadioTetraAgent.RadioTetraAgent - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "app.radio.RadioTetraAgent.RadioTetraAgent - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "app.radio.RadioTetraAgent.RadioTetraAgent - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../../../bin/win32_nr/"
# PROP Intermediate_Dir "../../../../../build/win32_nr/transactive/app/radio/RadioTetraAgent"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "./src" /I "../../../" /I "../../../../../build/win32_n/transactive" /I "../../../../../sdk/include/transactive" /I "../../../../../sdk/include/transactive/core/exceptions/IDL/src" /I "../../../../../sdk/include/transactive/core/message/IDL/src" /I "../../../../../sdk\include\transactive\core\process_management\idl\src" /I "../../../../../sdk/include/transactive\bus/security/duty_agent/IDL/src/" /I "../../../../../sdk/include/transactive\bus/security/authentication_agent/IDL/src/" /I "../../../../../sdk/omniORB/omniORB_4.0.5/include" /I "../../../../../sdk/include/cots/boost/boost_1_31_0" /I "..\..\..\..\..\sdk\include\cots\ACE\5_3\ACE_wrappers" /I "..\..\..\..\..\sdk\include\transactive\bus\radio\radioagentcorbadef_4669\IDL\src" /I "../../../../../sdk/include/transactive/bus/trains/trainagentcorbadef/IDL/src" /I "../../../../../sdk\include\transactive\bus\scada\common_library\IDL\src" /D "NDEBUG" /D "__WIN32__" /D "__x86__" /D "_NT_" /D __OSVERSION__=4 /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D _WIN32_WINNT=0x0400 /FD /Zm500 /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib WS2_32.lib winmm.lib ace.lib TA_Base_Bus.lib TA_IRS_Core.lib TA_IRS_Bus.lib omniORB405_rt.lib omniDynamic405_rt.lib omnithread30_rt.lib COS405_rt.lib TA_Base_Core.lib oci.lib Rpcrt4.lib stracelib.lib /nologo /subsystem:windows /pdb:none /machine:I386 /out:"../../../../../bin/win32_nr/RadioTetraAgent.exe" /libpath:"../../../../../build/win32_nr" /libpath:"..\..\..\..\..\sdk\win32_nr" /libpath:"..\..\..\..\..\sdk\lib" /libpath:"../../../../../sdk/omniORB/omniORB_4.0.5/lib/x86_win32" /libpath:"../../../../../sdk/include/cots/WinStackTrace\build\Release"

!ELSEIF  "$(CFG)" == "app.radio.RadioTetraAgent.RadioTetraAgent - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../../../build/win32_nd/"
# PROP Intermediate_Dir "../../../../../build/win32_nd/transactive/app/radio/RadioTetraAgent"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /Zi /Od /I "./src" /I "../../../" /I "../../../../../build/win32_n/transactive" /I "../../../../../sdk/include/transactive" /I "../../../../../sdk/include/transactive/bus/scada/common_library/IDL/src/" /I "../../../../../sdk/include/transactive/core/exceptions/IDL/src" /I "../../../../../sdk/include/transactive/core/message/IDL/src" /I "../../../../../sdk\include\transactive\core\process_management\idl\src" /I "../../../../../sdk/include/transactive\bus/security/duty_agent/IDL/src/" /I "../../../../../sdk/include/transactive\bus/security/authentication_agent/IDL/src/" /I "../../../../../sdk/omniORB/omniORB_4.0.5/include" /I "../../../../../sdk/include/cots/boost/boost_1_31_0" /I "..\..\..\..\..\sdk\include\cots\ACE\5_3\ACE_wrappers" /I "..\..\..\..\..\sdk\include\transactive\bus\radio\radioagentcorbadef_4669\IDL\src" /I "../../../../../sdk/include/transactive/bus/scada/DataPointCorbaDef/src/" /I "../../../../../sdk/include/transactive/bus/trains/trainagentcorbadef/IDL/src" /I "../../../../../sdk/include/transactive/core/alarm/IDL/src" /D "_DEBUG" /D "__WIN32__" /D "__x86__" /D "_NT_" /D __OSVERSION__=4 /D "WIN32" /D "_CONSOLE" /D "_MBCS" /FR /FD /GZ /Zm200 /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 omniORB405_rtd.lib omniDynamic405_rtd.lib COS405_rtd.lib omnithread30_rtd.lib oci.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Rpcrt4.lib WS2_32.lib winmm.lib aced.lib TA_Base_Core_d.lib TA_Base_Bus_d.lib TA_IRS_Core_d.lib TA_IRS_Bus_d.lib RadioProtocolLibrary_d.lib stracelib.lib /nologo /subsystem:console /incremental:no /pdb:"../../../../../bin/win32_nd/RadioTetraAgent.pdb" /map /debug /debugtype:both /machine:I386 /out:"../../../../../bin/win32_nd/RadioTetraAgent.exe" /pdbtype:sept /libpath:"../../../../../build/win32_nd" /libpath:"../../../../../sdk/include/transactive" /libpath:"../../../../../sdk/omniORB/omniORB_4.0.5/lib/x86_win32" /libpath:"..\..\..\..\..\sdk\win32_nd" /libpath:"..\..\..\..\..\sdk\lib" /libpath:"../../../../../sdk/include/cots/WinStackTrace/build/Debug" /libpath:"../../../../../bin/win32_nd"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "app.radio.RadioTetraAgent.RadioTetraAgent - Win32 Release"
# Name "app.radio.RadioTetraAgent.RadioTetraAgent - Win32 Debug"
# Begin Group "src"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\main.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ObservedSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Radio.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioAgentConstants.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioAuthorisationManager.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioBackgroundProcessor.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioCallStackUpdateTask.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioCircuitModeManager.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioCircuitTransfer.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioConfigurationException.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioConnectionException.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioConnectionManager.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioCorbaTypeConverter.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioDeleteReferenceTask.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioDirectory.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioDirectoryServant.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioDirectorySynchronisation.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioEventSubscriber.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioException.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioFallbackException.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioFullStateReceiver.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioGroupCallAckTask.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioHelper.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioKeepAliveHandlerThread.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodTimeoutException.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioResetConnectionTask.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioResetSessionTask.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioResetSocketTask.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioResourceManager.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioSDSDetailsUpdateTask.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioServant.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioServerLinkStatusUpdateThread.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioSession.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioSessionException.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioSessionHelper.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioSessionInitialisationThread.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioSessionNotInitialisedException.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioSessionServant.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioSessionTask.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioStateMessageConverter.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioStateSynchronisationServant.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioStateUpdateBroadcaster.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioStateUpdateReceiver.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioSubscriberActivityTask.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioSubscriberMonitor.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioTask.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioTaskFactory.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioTCPConnection.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioTcpServer.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioTetraAgent.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioTimedEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioTransactionManager.cpp
# End Source File
# Begin Source File

SOURCE=.\src\VersionGen.cpp
# End Source File
# End Group
# Begin Group "inc"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\src\CallForwardingDetails.h
# End Source File
# Begin Source File

SOURCE=.\src\IRadioCallback.h
# End Source File
# Begin Source File

SOURCE=.\src\IRadioConnectionObserver.h
# End Source File
# Begin Source File

SOURCE=.\src\IRadioDirectoryCallback.h
# End Source File
# Begin Source File

SOURCE=.\src\IRadioTetraAgentPeerState.h
# End Source File
# Begin Source File

SOURCE=.\src\IRadioTetraAgentState.h
# End Source File
# Begin Source File

SOURCE=.\src\ISocketObserver.h
# End Source File
# Begin Source File

SOURCE=.\src\ObservedSocket.h
# End Source File
# Begin Source File

SOURCE=.\src\Radio.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioAgentConstants.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioAuthorisationManager.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioBackgroundProcessor.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioCallStackUpdateTask.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioCircuitModeManager.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioCircuitTransfer.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioConfigurationException.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioConnectionException.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioConnectionManager.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioCorbaTypeConverter.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioDeleteReferenceTask.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioDirectory.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioDirectoryServant.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioDirectorySynchronisation.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioEventSubscriber.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioException.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioFallbackException.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioFullStateReceiver.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioGroupCallAckTask.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioHelper.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioKeepAliveHandlerThread.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodTimeoutException.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioResetConnectionTask.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioResetSessionTask.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioResetSocketTask.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioResourceManager.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioSDSDetailsUpdateTask.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioServant.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioServerLinkStatusUpdateThread.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioSession.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioSessionException.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioSessionHelper.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioSessionInitialisationThread.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioSessionNotInitialisedException.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioSessionServant.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioSessionTask.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioStateMessageConverter.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioStateSynchronisationServant.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioStateUpdateBroadcaster.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioStateUpdateReceiver.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioSubscriberActivityTask.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioSubscriberMonitor.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioSynchronisationTypes.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioTask.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioTaskFactory.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioTCPConnection.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioTcpServer.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioTetraAgent.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioTimedEvent.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioTransactionManager.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioTypes.h
# End Source File
# End Group
# End Target
# End Project
