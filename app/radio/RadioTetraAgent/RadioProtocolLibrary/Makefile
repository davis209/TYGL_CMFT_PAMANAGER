# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/Makefile $
# @author:  Ripple
# @version: $Revision: #2 $
#
# Last modification: $DateTime: 2009/05/18 10:25:59 $
# Last modified by:  $Author: builder $
#
# This file builds the Radio Agent Protocol Library
#

PROJ_ROOT_DIR	= ../../../../../..
PROJ_CODE_DIR	= transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary
PROJ_NAME	= TA_RadioProtocolLibrary
PROJ_SOURCE	= DeleteReferenceEventReader.cpp \
		GetGroupDetailsEventReader.cpp \
		IncomingCallEventReader.cpp \
		RadioEvent.cpp \
		IncomingSDSEventReader.cpp \
		NewReferenceEventReader.cpp \
		RadioMethodSelect.cpp \
		ChangeReferenceEventReader.cpp \
		RadioMethodTextToReference.cpp \
		RadioMethodDemandTX.cpp \
		RadioEventReader.cpp \
		RadioMethod.cpp \
		RadioMethodAttachSession.cpp \
		RadioMethodDetachSession.cpp \
		RadioMethodCallReference.cpp \
		RadioMethodDeleteSubscriber.cpp \
		RadioMethodCeaseTX.cpp \
		RadioMethodDeleteReference.cpp \
		RadioMethodDisconnect.cpp \
		RadioMethodGetGroupDetails.cpp \
		RadioMethodInitialise.cpp \
		RadioMethodNewReference.cpp \
		RadioMethodQueryReference.cpp \
		RadioMethodSearchSubscribers.cpp \
		DemandTXEventReader.cpp \
		RadioMethodSendSDS.cpp \
		RadioMethodSetupCall.cpp \
		CeaseTXEventReader.cpp \
		RadioMethodText2SubscriberReference.cpp \
		RadioMethodDeselect.cpp \
		RadioMethodChangeReference.cpp \
		SearchSubscribersEventReader.cpp \
		Text2SubscriberReferenceEventReader.cpp \
		QueryReferenceEventReader.cpp \
		DisconnectEventReader.cpp \
		RadioMethodDetachAudio.cpp \
		TextToReferenceEventReader.cpp \
		RadioMethodAttachAudio.cpp \
		ConvertToDBTimeEventReader.cpp \
		RadioMethodConvertToDBTime.cpp \
		RadioMethodInclude.cpp \
		RadioMethodAuthoriseCall.cpp \
		SelectEventReader.cpp \
		RadioMethodLogin.cpp \
		LoginEventReader.cpp \
		SubscriberActivityEventReader.cpp \
		RequestAuthoriseEventReader.cpp \
		RadioMethodLogout.cpp \
		RadioMethodAnswerCall.cpp \
		RadioMethodMonitorSubscriber.cpp \
		MonitorSubscriberEventReader.cpp \
		RadioMethodAttachMonitorAudio.cpp \
		RadioMethodDetachMonitorAudio.cpp \
		RadioMethodMonitorCall.cpp \
		RadioMethodJoin.cpp \
		RadioMethodForceCallTermination.cpp \
		MonitorCallEventReader.cpp \
		ForceCallTerminationEventReader.cpp \
		AttachMonitorAudioEventReader.cpp \
		DetachMonitorAudioEventReader.cpp \
		IncludeEventReader.cpp \
		JoinEventReader.cpp \
		RadioMethodAttachToGroup.cpp \
		AttachToGroupEventReader.cpp \
		CallStatusEventReader.cpp \
		CircuitDataCapacityEventReader.cpp \
		IncomingCircuitDataEventReader.cpp \
		SendCircuitDataEventReader.cpp \
		RadioMethodSendCircuitData.cpp \
		GroupCallAckEventReader.cpp \
		ITcpEvent.cpp \
		ITcpMethod.cpp \
		SystemErrorEventReader.cpp \
		DetachSessionEventReader.cpp \
		RadioMethodCheckConnection.cpp \
		KeepAliveEventReader.cpp \
		CheckConnectionEventReader.cpp \
		AttachSessionExtEventReader.cpp \
		GetLastErrorEventReader.cpp \
		IncomingSDSTLAckEventReader.cpp \
		IncomingSDSTLReportEventReader.cpp \
		IncomingSDSTLTransferEventReader.cpp \
		IncomingSDSTLTransferExtendedEventReader.cpp \
		IncomingSimpleTextSDSEventReader.cpp \
		IncomingUserDefinedSDSEventReader.cpp \
		RadioMethodAttachSessionExt.cpp \
		RadioMethodGetLastError.cpp \
		RadioMethodSimpleTextSDS.cpp \
		RadioMethodTLSDSAck.cpp \
		RadioMethodTLSDSReport.cpp \
		RadioMethodTLSDSTransfer.cpp \
		RadioMethodUserDefinedSDS.cpp \
		RadioProtocolException.cpp \
		SDSTLAckEventReader.cpp \
		SDSTLReportEventReader.cpp \
		SDSTLTransferEventReader.cpp \
		SimpleTextSDSEventReader.cpp \
		UserDefinedSDSEventReader.cpp \
		PttStatusEventReader.cpp \
		RadioMethodChangePassword.cpp \
		ChangePasswordEventReader.cpp
		
PROJ_TYPE	= lib
PROJ_INC_DIR    = ./src $(PROJ_ROOT_DIR)/code/transactive $(PROJ_ROOT_DIR)/sdk/include/transactive
PROJ_OUT_DIR    = $(PROJ_ROOT_DIR)/build/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

#
# DEPENDENT_LIBRARIES = 
#				$(PROJ_ROOT_DIR)/core/message/IDL \

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing
