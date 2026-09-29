# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/TYGL_CMFT_TIP/J155/transactive/app/signs/common_library/Makefile $
# @author:  Ripple
# @version: $Revision: #3 $
#
# Last modification: $DateTime: 2023/10/16 18:31:43 $
# Last modified by:  $Author: CM $

PROJ_ROOT_DIR               = ../../../../..
PROJ_CODE_DIR               = transactive/app/signs/common_library
PROJ_NAME                   = TA_SignsCommonLibrary
PROJ_TYPE                   = lib
PROJ_SOURCE                 = $(filter-out $(PROJ_SOURCE_IGNORE), $(subst src/,,$(wildcard src/*.cpp src/stis_protocol/*.cpp src/stis_protocol/message_types/*.cpp)))
PROJ_SOURCE_IGNORE          = pch.cpp
ADDITIONAL_OBJ_DIRECTORIES  = stis_protocol stis_protocol/message_types
PROJ_INC_DIR                = src
PROJ_OUT_DIR                = $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)
DEPENDENT_LIBRARIES         = $(PROJ_ROOT_DIR)/code/transactive/core/utility/IDL
PROJ_CFLAGS                 = -DTA_ASSERT_LOG=1

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
        #Do Nothing
