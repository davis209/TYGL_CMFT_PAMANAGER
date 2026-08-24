# Microsoft Developer Studio Project File - Name="app.radio.RadioTetraAgent.RadioProtocolLibrary.RadioProtocolLibrary" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=app.radio.RadioTetraAgent.RadioProtocolLibrary.RadioProtocolLibrary - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "app.radio.RadioTetraAgent.RadioProtocolLibrary.RadioProtocolLibrary.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "app.radio.RadioTetraAgent.RadioProtocolLibrary.RadioProtocolLibrary.mak" CFG="app.radio.RadioTetraAgent.RadioProtocolLibrary.RadioProtocolLibrary - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "app.radio.RadioTetraAgent.RadioProtocolLibrary.RadioProtocolLibrary - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "app.radio.RadioTetraAgent.RadioProtocolLibrary.RadioProtocolLibrary - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "app.radio.RadioTetraAgent.RadioProtocolLibrary.RadioProtocolLibrary - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../../../../build/win32_nr/"
# PROP Intermediate_Dir "../../../../../../build/win32_nr/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary"
# PROP Target_Dir ""
MTL=midl.exe
LINK32=link.exe -link
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "..\..\..\.." /I "../../../../../../sdk/include/transactive" /I "..\..\..\..\..\..\sdk\omniORB\omniORB_4.0.5\include" /I "..\..\..\..\..\..\sdk\include\cots\ACE\5_3\ACE_wrappers" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "__WIN32__" /D "__x86__" /D "__NT__" /D __OSVERSION__=4 /YX /FD /c
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../../../../build/win32_nr/RadioProtocolLibrary.lib" /ignore:4006

!ELSEIF  "$(CFG)" == "app.radio.RadioTetraAgent.RadioProtocolLibrary.RadioProtocolLibrary - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../../../../build/win32_nd/"
# PROP Intermediate_Dir "../../../../../../build/win32_nd/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary"
# PROP Target_Dir ""
MTL=midl.exe
LINK32=link.exe -link
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\..\..\.." /I "../../../../../../sdk/include/transactive" /I "..\..\..\..\..\..\sdk\omniORB\omniORB_4.0.5\include" /I "..\..\..\..\..\..\sdk\include\cots\ACE\5_3\ACE_wrappers" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "__WIN32__" /D "__x86__" /D "__NT__" /D __OSVERSION__=4 /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../../../../build/win32_nd/RadioProtocolLibrary_d.lib" /ignore:4006

!ENDIF 

# Begin Target

# Name "app.radio.RadioTetraAgent.RadioProtocolLibrary.RadioProtocolLibrary - Win32 Release"
# Name "app.radio.RadioTetraAgent.RadioProtocolLibrary.RadioProtocolLibrary - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\AttachMonitorAudioEventReader.cpp
# End Source File
# Begin Source File

SOURCE=.\src\AttachToGroupEventReader.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CallStatusEventReader.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CeaseTXEventReader.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ChangeReferenceEventReader.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CheckConnectionEventReader.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CircuitDataCapacityEventReader.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ConvertToDBTimeEventReader.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DeleteReferenceEventReader.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DemandTXEventReader.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DetachMonitorAudioEventReader.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DetachSessionEventReader.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DisconnectEventReader.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GetGroupDetailsEventReader.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GroupCallAckEventReader.cpp
# End Source File
# Begin Source File

SOURCE=.\src\IncludeEventReader.cpp
# End Source File
# Begin Source File

SOURCE=.\src\IncomingCallEventReader.cpp
# End Source File
# Begin Source File

SOURCE=.\src\IncomingCircuitDataEventReader.cpp
# End Source File
# Begin Source File

SOURCE=.\src\IncomingSDSEventReader.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ITcpEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ITcpMethod.cpp
# End Source File
# Begin Source File

SOURCE=.\src\JoinEventReader.cpp
# End Source File
# Begin Source File

SOURCE=.\src\KeepAliveEventReader.cpp
# End Source File
# Begin Source File

SOURCE=.\src\LoginEventReader.cpp
# End Source File
# Begin Source File

SOURCE=.\src\MonitorCallEventReader.cpp
# End Source File
# Begin Source File

SOURCE=.\src\MonitorSubscriberEventReader.cpp
# End Source File
# Begin Source File

SOURCE=.\src\NewReferenceEventReader.cpp
# End Source File
# Begin Source File

SOURCE=.\src\QueryReferenceEventReader.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioEventReader.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethod.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodAnswerCall.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodAttachAudio.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodAttachMonitorAudio.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodAttachSession.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodAttachToGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodAuthoriseCall.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodCallReference.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodCeaseTX.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodChangeReference.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodCheckConnection.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodConvertToDBTime.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodDeleteReference.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodDeleteSubscriber.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodDemandTX.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodDeselect.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodDetachAudio.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodDetachMonitorAudio.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodDetachSession.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodDisconnect.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodForceCallTermination.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodGetGroupDetails.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodInclude.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodInitialise.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodJoin.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodLogin.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodLogout.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodMonitorCall.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodMonitorSubscriber.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodNewReference.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodQueryReference.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodSearchSubscribers.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodSelect.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodSendCircuitData.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodSendSDS.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodSetupCall.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodText2SubscriberReference.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodTextToReference.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RequestAuthoriseEventReader.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SearchSubscribersEventReader.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SelectEventReader.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SendCircuitDataEventReader.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SubscriberActivityEventReader.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SystemErrorEventReader.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Text2SubscriberReferenceEventReader.cpp
# End Source File
# Begin Source File

SOURCE=.\src\TextToReferenceEventReader.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\src\AttachMonitorAudioEventReader.h
# End Source File
# Begin Source File

SOURCE=.\src\AttachToGroupEventReader.h
# End Source File
# Begin Source File

SOURCE=.\src\CallStatusEventReader.h
# End Source File
# Begin Source File

SOURCE=.\src\CeaseTXEventReader.h
# End Source File
# Begin Source File

SOURCE=.\src\ChangeReferenceEventReader.h
# End Source File
# Begin Source File

SOURCE=.\src\CheckConnectionEventReader.h
# End Source File
# Begin Source File

SOURCE=.\src\CircuitDataCapacityEventReader.h
# End Source File
# Begin Source File

SOURCE=.\src\ConvertToDBTimeEventReader.h
# End Source File
# Begin Source File

SOURCE=.\src\DeleteReferenceEventReader.h
# End Source File
# Begin Source File

SOURCE=.\src\DemandTXEventReader.h
# End Source File
# Begin Source File

SOURCE=.\src\DetachMonitorAudioEventReader.h
# End Source File
# Begin Source File

SOURCE=.\src\DetachSessionEventReader.h
# End Source File
# Begin Source File

SOURCE=.\src\DisconnectEventReader.h
# End Source File
# Begin Source File

SOURCE=.\src\GetGroupDetailsEventReader.h
# End Source File
# Begin Source File

SOURCE=.\src\GroupCallAckEventReader.h
# End Source File
# Begin Source File

SOURCE=.\src\IncludeEventReader.h
# End Source File
# Begin Source File

SOURCE=.\src\IncomingCallEventReader.h
# End Source File
# Begin Source File

SOURCE=.\src\IncomingCircuitDataEventReader.h
# End Source File
# Begin Source File

SOURCE=.\src\IncomingSDSEventReader.h
# End Source File
# Begin Source File

SOURCE=.\src\ITcpEvent.h
# End Source File
# Begin Source File

SOURCE=.\src\ITcpMethod.h
# End Source File
# Begin Source File

SOURCE=.\src\JoinEventReader.h
# End Source File
# Begin Source File

SOURCE=.\src\KeepAliveEventReader.h
# End Source File
# Begin Source File

SOURCE=.\src\LoginEventReader.h
# End Source File
# Begin Source File

SOURCE=.\src\MonitorCallEventReader.h
# End Source File
# Begin Source File

SOURCE=.\src\MonitorSubscriberEventReader.h
# End Source File
# Begin Source File

SOURCE=.\src\NewReferenceEventReader.h
# End Source File
# Begin Source File

SOURCE=.\src\QueryReferenceEventReader.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioEvent.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioEventReader.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioFields.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethod.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodAnswerCall.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodAttachAudio.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodAttachMonitorAudio.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodAttachSession.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodAttachToGroup.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodAuthoriseCall.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodCallReference.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodCeaseTX.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodChangeReference.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodCheckConnection.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodConvertToDBTime.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodDeleteReference.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodDeleteSubscriber.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodDemandTX.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodDeselect.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodDetachAudio.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodDetachMonitorAudio.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodDetachSession.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodDisconnect.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodForceCallTermination.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodGetGroupDetails.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodInclude.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodInitialise.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodJoin.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodLogin.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodLogout.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodMonitorCall.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodMonitorSubscriber.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodNewReference.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodQueryReference.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodSearchSubscribers.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodSelect.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodSendCircuitData.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodSendSDS.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodSetupCall.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodText2SubscriberReference.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioMethodTextToReference.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioProtocol.h
# End Source File
# Begin Source File

SOURCE=.\src\RadioProtocolLibrary.h
# End Source File
# Begin Source File

SOURCE=.\src\RequestAuthoriseEventReader.h
# End Source File
# Begin Source File

SOURCE=.\src\SearchSubscribersEventReader.h
# End Source File
# Begin Source File

SOURCE=.\src\SelectEventReader.h
# End Source File
# Begin Source File

SOURCE=.\src\SendCircuitDataEventReader.h
# End Source File
# Begin Source File

SOURCE=.\src\SubscriberActivityEventReader.h
# End Source File
# Begin Source File

SOURCE=.\src\SystemErrorEventReader.h
# End Source File
# Begin Source File

SOURCE=.\src\Text2SubscriberReferenceEventReader.h
# End Source File
# Begin Source File

SOURCE=.\src\TextToReferenceEventReader.h
# End Source File
# End Group
# End Target
# End Project
