# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/TYGL_CMFT_TIP/J155/transactive/app/radio/RadioTetraAgent/Makefile $
# @author:  Ripple
# @version: $Revision: #3 $
#
# Last modification: $DateTime: 2023/10/16 18:31:43 $
# Last modified by:  $Author: CM $
#
# Make file for the Radio Tetra Agent
#

PROJ_ROOT_DIR   = ../../../../..
PROJ_CODE_DIR   = transactive/app/radio/RadioTetraAgent
PROJ_NAME       = RadioTetraGateway
PROJ_SOURCE     = main.cpp \
                Radio.cpp \
                RadioResourceManager.cpp \
                RadioDirectory.cpp \
                RadioDirectoryServant.cpp \
                RadioDirectorySynchronisation.cpp \
                RadioConnectionManager.cpp \
                RadioCorbaTypeConverter.cpp \
                RadioServant.cpp \
                RadioSession.cpp \
                RadioSessionInitialisationThread.cpp \
                RadioSessionServant.cpp \
                RadioTCPConnection.cpp \
                RadioTetraAgent.cpp \
                RadioTransactionManager.cpp \
                RadioSessionException.cpp \
                RadioSessionNotInitialisedException.cpp \
                RadioMethodTimeoutException.cpp \
                RadioAuthorisationManager.cpp \
                RadioEventSubscriber.cpp \
                RadioBackgroundProcessor.cpp \
                RadioTaskFactory.cpp \
                RadioTask.cpp \
                RadioSubscriberActivityTask.cpp \
                RadioCallStackUpdateTask.cpp \
                RadioSessionTask.cpp \
                RadioHelper.cpp \
                RadioSessionHelper.cpp \
                RadioSubscriberMonitor.cpp \
                RadioGroupCallAckTask.cpp \
                RadioResetSessionTask.cpp \
                RadioSDSDetailsUpdateTask.cpp \
                RadioCircuitModeManager.cpp \
                RadioCircuitTransfer.cpp \
                RadioFullStateReceiver.cpp \
                RadioStateUpdateReceiver.cpp \
                RadioStateSynchronisationServant.cpp \
                RadioStateMessageConverter.cpp \
                RadioStateUpdateBroadcaster.cpp \
                RadioFallbackException.cpp \
                RadioResetConnectionTask.cpp \
                RadioDeleteReferenceTask.cpp \
                RadioAgentConstants.cpp \
                RadioConfigurationException.cpp \
                RadioConnectionException.cpp \
                RadioException.cpp \
                ObservedSocket.cpp \
                RadioTcpServer.cpp \
                RadioTimedEvent.cpp \
                RadioServerLinkStatusUpdateThread.cpp \
                RadioKeepAliveHandlerThread.cpp \
                RadioResetSocketTask.cpp \
                RadioStatusSummary.cpp \
				RadioSystemErrorTask.cpp \
				IMTLogHelper.cpp \
				VersionGen.cpp

PROJ_TYPE       = exe
PROJ_INC_DIR    = ./src $(PROJ_ROOT_DIR)/code/transactive $(PROJ_ROOT_DIR)/sdk/include/transactive
PROJ_OUT_DIR    = $(PROJ_ROOT_DIR)/bin/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

PROJ_LIBS   = \
        TA_TrainCorbaProxy \
        TA_TrainCommonLibrary \
        TA_TrainAgentObjectNames \
        TA_TrainAgentCorba \
        TA_ScadaGatewaySync \
        TA_ScadaAgent \
        TA_CorbaUtil \
        TA_DataPointAgentCorbaDef_Impl \
        TA_DataNodeAgentCorbaDef_Impl \
        TA_DataPointLibrary \
        TA_DerivedDataPointDAI \
        TA_DataNodeLibrary \
        TA_ScadaCommonCorbaDef \
        TA_ScadaCommonLib \
        TA_MathEvaluation \
        TA_CorbaUtil \
        TA_DataRecording \
        TA_DataRecordingDAI \
        TA_Utilities \
        TA_Version \
        TA_Timers \
        TA_Thread \
        TA_GenericAgent \
        TA_Bus_Event \
        TA_EventTriggeredPlans \
        TA_PlanAgentLibrary \
        TA_PlanAgentCorbaDef \
        TA_PlanDAI \
        TA_PlanAgentDataCorbaDef \
        TA_PlanManagerCorbaDef \
        TA_ScadaProxyLibrary \
        TA_ScadaBusinessLibrary \
        TA_ModbusHelperLib \
        TA_DataNodeLibrary \
        TA_DataPointLibrary \
        TA_DerivedDataPointDAI \
        TA_ScadaCommonLib \
        TA_ScadaCommonCorbaDef \
        TA_RTULib \
        TA_DataRecording \
        TA_DataRecordingDAI \
        TA_MmsCorbaDef \
        TA_ItaScada \
        TA_AtsTCPCommunication \
        TA_AtsMethod \
        TA_AtsDai \
        TA_AtsAgentCorbaDef \
        TA_Alarm \
        TA_RightsLibrary \
        TA_SecurityUtilities \
        TA_AuthenticationLibrary \
        TA_AuthenticationCorbaDef \
        TA_RightsAgentCorbaDef \
        TA_Naming \
        TA_AlarmAgentCorbaDef \
        TA_EntityStatusUpdateCorbaDef \
        TA_Message \
        TA_EventProcessing \
        TA_Event \
        TA_EventAgentCorbaDef \
        TA_Alarm \
        TA_Naming \
        TA_Sockets \
        TA_Synchronisation \
        TA_CorbaDef \
        TA_AlarmAgentCorbaDef \
        TA_EntityStatusUpdateCorbaDef \
        TA_NamingCorbaDef \
        TA_MessageCorbaDef \
        TA_ManagedProcess \
        TA_ProcessManagementCorbaDef \
        TA_AuthenticationLibrary \
        TA_AuthenticationCorbaDef \
        TA_RadioAgentCorbaDef_4669 \
        TA_RadioSDSUtilities \
        TA_RightsLibrary \
        TA_AuthenticationLibrary \
        TA_DutyAgentCorbaDef \
        TA_SecurityUtilities \
        TA_AuthenticationCorbaDef \
        TA_RightsAgentCorbaDef \
        TA_EntityAccess \
        TA_RadioDAI \
        TA_DAI \
        TA_AlarmRuleDAI \
        TA_DataAccess \
        TA_DatabaseCorbaDef \
        TA_ConfigurationUpdates \
        TA_Version \
        TA_ModbusComms \
        TA_Sockets \
        TA_Exceptions \
        TA_CommonExceptionsCorbaDef \
        TA_Utilities \
        TA_Uuid \
        TA_RadioProtocolLibrary \
        TA_EntityAccess \
        TA_MathEvaluation \
        TA_Synchronisation \
        TA_UtilityCorbaDef \
        TA_Utility \
        TA_ocl \
        mysqlcppconn \
        clntsh \
        ACE \
        ACEXML

DEPENDENT_LIBRARIES = $(PROJ_ROOT_DIR)/code/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary \
                      $(PROJ_ROOT_DIR)/code/transactive/bus/trains/TrainCorbaProxy \
                      $(PROJ_ROOT_DIR)/code/transactive/bus/radio/RadioSDSUtilities \
                      $(PROJ_ROOT_DIR)/code/transactive/core/utility/IDL \
                      $(PROJ_ROOT_DIR)/code/transactive/core/utility

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
    #Do Nothing
