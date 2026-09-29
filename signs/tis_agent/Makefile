# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/TYGL_CMFT_TIP/J155/transactive/app/signs/tis_agent/Makefile $
# @author:  Ripple
# @version: $Revision: #5 $
#
# Last modification: $DateTime: 2026/05/21 16:17:09 $
# Last modified by:  $Author: anusuya $

PROJ_ROOT_DIR       = ../../../../..
PROJ_CODE_DIR       = transactive/app/signs/tis_agent
PROJ_NAME           = TisAgent
PROJ_TYPE           = exe
PROJ_SOURCE         = $(filter-out $(PROJ_SOURCE_IGNORE), $(subst src/,,$(wildcard src/*.cpp src/stis_protocol/*.cpp)))
PROJ_SOURCE_IGNORE  = pch.cpp
ADDITIONAL_OBJ_DIRECTORIES = stis_protocol
PROJ_INC_DIR        = ./src $(PROJ_ROOT_DIR)/code/transactive $(PROJ_ROOT_DIR)/sdk/include/transactive
PROJ_OUT_DIR        = $(PROJ_ROOT_DIR)/bin/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

PROJ_LIBS           = \
                    TA_CorbaUtil \
                    TA_TISManagerCorbaDef_4669 \
                    TA_DataPointAgentCorbaDef_Impl \
                    TA_DataNodeAgentCorbaDef_Impl \
                    TA_DataPointLibrary \
                    TA_ScadaProxyLibrary \
                    TA_DataNodeLibrary \
                    TA_ScadaCommonLib \
                    TA_ScadaCommonCorbaDef \
                    TA_ScadaGatewaySync \
                    TA_DataRecording \
                    TA_DataRecordingDAI \
                    TA_MmsCorbaDef \
                    TA_RightsLibrary \
                    TA_AuthenticationLibrary \
                    TA_DutyAgentCorbaDef \
                    TA_SecurityUtilities \
                    TA_AuthenticationCorbaDef \
                    TA_RightsAgentCorbaDef \
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
                    TA_Alarm \
                    TA_Naming \
                    TA_Message \
                    TA_EventProcessing \
                    TA_Event \
                    TA_EventAgentCorbaDef \
                    TA_Uuid \
                    TA_AlarmAgentCorbaDef \
                    TA_EntityStatusUpdateCorbaDef \
                    TA_TISAgentAccess \
                    TA_TISAgentDAI_4669 \
                    TA_TISAgentCorbaDef_4669 \
                    TA_TISAgentAccess_4669 \
                    TA_TrainSelectionComponent \
                    TA_TrainCorbaProxy \
                    TA_TrainCommonLibrary \
                    TA_TrainAgentObjectNames \
                    TA_TrainAgentCorba \
                    TA_EntityAccess \
                    TA_DAI \
                    TA_AlarmRuleDAI \
                    TA_DerivedDataPointDAI \
                    TA_AtsAgentCorbaDef \
                    TA_CorbaDef \
                    TA_NamingCorbaDef \
                    TA_MessageCorbaDef \
                    TA_ManagedProcess \
                    TA_ProcessManagementCorbaDef \
                    TA_DataAccess \
                    TA_DatabaseCorbaDef \
                    TA_ConfigurationUpdates \
                    TA_Utilities \
                    TA_Exceptions \
                    TA_CommonExceptionsCorbaDef \
                    TA_Version \
                    TA_Sockets \
                    TA_FTP \
                    TA_MathEvaluation \
                    TA_Synchronisation \
                    TA_SignsCommonLibrary \
                    TA_UtilityCorbaDef \
                    TA_Utility \
                    TA_ocl \
                    curl \
                    ssh2 \
                    clntsh

DEPENDENT_LIBRARIES = \
                    $(PROJ_ROOT_DIR)/code/transactive/app/signs/common_library \
                    $(PROJ_ROOT_DIR)/code/transactive/core/utility/IDL \
                    $(PROJ_ROOT_DIR)/code/transactive/core/utility \
                    $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/tis_agent \
                    $(PROJ_ROOT_DIR)/code/transactive/bus/ats/ats_agent/IDL \
                    $(PROJ_ROOT_DIR)/code/transactive/bus/signs/tis_agent_access \
                    $(PROJ_ROOT_DIR)/code/transactive/bus/trains/TrainSelectionComponent

PROJ_CFLAGS         = -DTA_ASSERT_LOG=1

# COS CORBA IDL skeleton sources (required by StisServerServant)
CORBALIB_SRC_DIR    = $(PROJ_ROOT_DIR)/code/transactive/app/common/lib/corbalib/vc10/src
CORBALIB_INC_DIR    = $(PROJ_ROOT_DIR)/code/transactive/app/common/lib/corbalib/vc10/include
CORBALIB_SK_SOURCES = CosDataDefinitionSK.cc ICosMonitoredThreadSK.cc ICosDmdServerSK.cc
CORBALIB_SK_OBJECTS = $(CORBALIB_SK_SOURCES:%.cc=$(PROJ_OBJ_DIR)/%.o)

PROJ_INC_DIR       += $(CORBALIB_INC_DIR)

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

# Append corbalib SK objects to the link command.
# NOTE: The link rule in CommonMakefile.mak ($(PROJ_TARGET): $(OBJECTS) ...)
# has its prerequisites expanded at parse time, so appending to OBJECTS here
# only affects the recipe's link line, not the prerequisite list. We therefore
# also add the corbalib SK objects as an explicit prerequisite of PROJ_TARGET
# below so make actually builds them before linking.
OBJECTS += $(CORBALIB_SK_OBJECTS)

$(PROJ_TARGET): $(CORBALIB_SK_OBJECTS)

# Compile rule for corbalib .cc skeleton files
$(PROJ_OBJ_DIR)/%.o: $(CORBALIB_SRC_DIR)/%.cc
	@echo "==> COMPILING (corbalib)" $@
	@if [ ! -d $(PROJ_OBJ_DIR) ]; then mkdir -p $(PROJ_OBJ_DIR); fi
	$(CXX) -o $@ $(CFLAGS) -I$(CORBALIB_INC_DIR) $<
	@echo "	DONE"

install:
        #Do Nothing
