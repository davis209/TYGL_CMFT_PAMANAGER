/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioFields.h $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This header file includes the text representations of the available keys in the radio subsystem.  *
  */

#if !defined(RADIO_FILEDS_H)
#define RADIO_FILEDS_H

namespace TA_IRS_App
{
    static const std::string FORWARD_UNCONDITIONAL     = "K_FORWARD_UNCONDITIONAL";
    static const std::string FORWARD_NOT_REACHABLE     = "K_FORWARD_NOT_REACHABLE";
    static const std::string NAME                      = "K_NAME";
    static const std::string TYPE                      = "K_TYPE";
    static const std::string ITSI                      = "K_TSI";
    static const std::string DELETED                   = "K_DELETED_FLAG";
    static const std::string REGION                    = "K_ORGANIZATION";
    static const std::string PATCH_ID                  = "K_PATCH_ID";
    static const std::string TEMPORARY_TSI             = "K_TEMPORARY_TSI";
    static const std::string SCN_ID                    = "K_SCN_ID";
    static const std::string LOCATION_AREA_ID          = "K_LOCATION_AREA_ID";
    static const std::string LOCATION_AREA_LABEL       = "K_LOCATION_AREA_LABEL";
    static const std::string LOCATION_AREA_DN          = "K_LOCATION_AREA_DN";
    static const std::string OWNER_DISPATCHER          = "K_OWNER_DISPATCHER";
}
#endif // !defined(RADIO_FILEDS_H)
