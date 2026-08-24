# Microsoft Developer Studio Project File - Name="core.data_access_interface.pa.TA_PADAI" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=core.data_access_interface.pa.TA_PADAI - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "core.data_access_interface.pa.TA_PADAI.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "core.data_access_interface.pa.TA_PADAI.mak" CFG="core.data_access_interface.pa.TA_PADAI - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "core.data_access_interface.pa.TA_PADAI - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "core.data_access_interface.pa.TA_PADAI - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "core.data_access_interface.pa.TA_PADAI - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../../../build/win32_nr/"
# PROP Intermediate_Dir "../../../../../build/win32_nr/transactive/core/data_access_interface/pa"
# PROP Target_Dir ""
LINK32=link.exe -link
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "..\..\.." /I "..\..\..\..\..\sdk\include\transactive" /I "..\..\..\..\..\sdk\omniORB\omniORB_4.0.5\include" /I "../../../../../sdk/include/cots/boost/boost_1_31_0" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__WIN32__" /D "__x86__" /D "__NT__" /D __OSVERSION__=4 /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../../../build/win32_nr/TA_PADAI.lib" /ignore:4006

!ELSEIF  "$(CFG)" == "core.data_access_interface.pa.TA_PADAI - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../../../build/win32_nd/"
# PROP Intermediate_Dir "../../../../../build/win32_nd/transactive/core/data_access_interface/pa"
# PROP Target_Dir ""
LINK32=link.exe -link
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\..\.." /I "..\..\..\..\..\sdk\include\transactive" /I "..\..\..\..\..\sdk\omniORB\omniORB_4.0.5\include" /I "../../../../../sdk/include/cots/boost/boost_1_31_0" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__WIN32__" /D "__x86__" /D "__NT__" /D __OSVERSION__=4 /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../../../build/win32_nd/TA_PADAI_d.lib" /ignore:4006

!ENDIF 

# Begin Target

# Name "core.data_access_interface.pa.TA_PADAI - Win32 Release"
# Name "core.data_access_interface.pa.TA_PADAI - Win32 Debug"
# Begin Group "PA_ZONE"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\ConfigPaZone.cpp
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Src\ConfigPaZone.h
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Src\IConfigPaZone.h
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Src\IPaZone.h
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Src\PaZone.cpp
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Src\PaZone.h
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Src\PaZoneAccessFactory.cpp
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Src\PaZoneAccessFactory.h
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Src\PaZoneHelper.cpp
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Src\PaZoneHelper.h
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "PA_ZONE_GROUP"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\ConfigPaZoneGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\ConfigPaZoneGroup.h
# End Source File
# Begin Source File

SOURCE=.\Src\IConfigPaZoneGroup.h
# End Source File
# Begin Source File

SOURCE=.\Src\IPaZoneGroup.h
# End Source File
# Begin Source File

SOURCE=.\Src\PaZoneGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\PaZoneGroup.h
# End Source File
# Begin Source File

SOURCE=.\Src\PaZoneGroupAccessFactory.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\PaZoneGroupAccessFactory.h
# End Source File
# Begin Source File

SOURCE=.\Src\PaZoneGroupHelper.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\PaZoneGroupHelper.h
# End Source File
# End Group
# Begin Group "PA_DVA_MESSAGE"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\ConfigPaDvaMessage.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\ConfigPaDvaMessage.h
# End Source File
# Begin Source File

SOURCE=.\Src\IConfigPaDvaMessage.h
# End Source File
# Begin Source File

SOURCE=.\Src\IPaDvaMessage.h
# End Source File
# Begin Source File

SOURCE=.\Src\PaDvaMessage.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\PaDvaMessage.h
# End Source File
# Begin Source File

SOURCE=.\Src\PaDvaMessageAccessFactory.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\PaDvaMessageAccessFactory.h
# End Source File
# Begin Source File

SOURCE=.\Src\PaDvaMessageHelper.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\PaDvaMessageHelper.h
# End Source File
# End Group
# Begin Group "HelperFiles"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\DatabaseQueryHelper.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\DatabaseQueryHelper.h
# End Source File
# Begin Source File

SOURCE=.\src\GenericDaiHelper.h
# End Source File
# Begin Source File

SOURCE=.\src\GenericDaiHelper.inl
# End Source File
# End Group
# Begin Group "PA_TRAIN_DVA_MESSAGE"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\ConfigPaTrainDvaMessage.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\ConfigPaTrainDvaMessage.h
# End Source File
# Begin Source File

SOURCE=.\Src\IConfigPaTrainDvaMessage.h
# End Source File
# Begin Source File

SOURCE=.\Src\IPaTrainDvaMessage.h
# End Source File
# Begin Source File

SOURCE=.\Src\PaTrainDvaMessage.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\PaTrainDvaMessage.h
# End Source File
# Begin Source File

SOURCE=.\Src\PaTrainDvaMessageAccessFactory.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\PaTrainDvaMessageAccessFactory.h
# End Source File
# Begin Source File

SOURCE=.\Src\PaTrainDvaMessageHelper.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\PaTrainDvaMessageHelper.h
# End Source File
# End Group
# Begin Group "PA_ATS_TRIGGERED_BROADCAST"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\ConfigPaAtsTriggeredBroadcast.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\ConfigPaAtsTriggeredBroadcast.h
# End Source File
# Begin Source File

SOURCE=.\Src\IConfigPaAtsTriggeredBroadcast.h
# End Source File
# Begin Source File

SOURCE=.\Src\IPaAtsTriggeredBroadcast.h
# End Source File
# Begin Source File

SOURCE=.\Src\PaAtsTriggeredBroadcast.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\PaAtsTriggeredBroadcast.h
# End Source File
# Begin Source File

SOURCE=.\Src\PaAtsTriggeredBroadcastAccessFactory.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\PaAtsTriggeredBroadcastAccessFactory.h
# End Source File
# Begin Source File

SOURCE=.\Src\PaAtsTriggeredBroadcastHelper.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\PaAtsTriggeredBroadcastHelper.h
# End Source File
# End Group
# Begin Group "PA_DVA_MESSAGE_VERSION"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\ConfigPaDvaMessageVersion.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\ConfigPaDvaMessageVersion.h
# End Source File
# Begin Source File

SOURCE=.\Src\IConfigPaDvaMessageVersion.h
# End Source File
# Begin Source File

SOURCE=.\Src\IPaDvaMessageVersion.h
# End Source File
# Begin Source File

SOURCE=.\Src\PaDvaMessageVersion.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\PaDvaMessageVersion.h
# End Source File
# Begin Source File

SOURCE=.\Src\PaDvaMessageVersionAccessFactory.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\PaDvaMessageVersionAccessFactory.h
# End Source File
# Begin Source File

SOURCE=.\Src\PaDvaMessageVersionHelper.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\PaDvaMessageVersionHelper.h
# End Source File
# End Group
# Begin Group "PA_STATION"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\ConfigPaStation.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ConfigPaStation.h
# End Source File
# Begin Source File

SOURCE=.\src\IConfigPaStation.h
# End Source File
# Begin Source File

SOURCE=.\src\IPaStation.h
# End Source File
# Begin Source File

SOURCE=.\src\PaStation.cpp
# End Source File
# Begin Source File

SOURCE=.\src\PaStation.h
# End Source File
# Begin Source File

SOURCE=.\src\PaStationAccessFactory.cpp
# End Source File
# Begin Source File

SOURCE=.\src\PaStationAccessFactory.h
# End Source File
# Begin Source File

SOURCE=.\src\PaStationHelper.cpp
# End Source File
# Begin Source File

SOURCE=.\src\PaStationHelper.h
# End Source File
# End Group
# End Target
# End Project
