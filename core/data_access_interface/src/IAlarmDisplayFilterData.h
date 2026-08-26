/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/IAlarmDisplayFilterData.h $
 * @author:  Noel R. Tajanlangit
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * IAlarmSeverityData is an interface to a Alarm severity object. It allows the Alarm severity object implementation
 * to be changed without changing the code that uses this interface.
 */

#ifndef __IALARM_DISPLAY_FILTER_DATA_H_INCLUDED__
#define __IALARM_DISPLAY_FILTER_DATA_H_INCLUDED__

#include "core/types/src/ta_types.h"
namespace TA_Base_Core
{
    class IAlarmDisplayFilterData
    {
    public:

        typedef enum
        {
            EXCLUDE = 0,
            INLCUDE
        } DISPLAY_CONDITION;

        /**
         * getKey
         *
         * Returns the key for this display filter.
         *
         * @return The key for this display filter for as an ta_uint32 .
         */
        virtual ta_uint32 getKey() = 0;

        // LocationKey
        virtual ta_uint32 getLocationKey() = 0;
        virtual void setLocationKey(const int& _nLocationKey) = 0;

        // AlarmType
        virtual ta_uint32 getAlarmType() = 0;
        virtual void setAlarmType(const ta_uint32& _nAlarmType) = 0;

        // alarm location
        virtual ta_uint32 getAlarmLocationKey() = 0;
        virtual void setAlarmLocationKey(const ta_uint32& _nLocationKey) = 0;

        // Sub System
        virtual ta_uint32 getSubSystemKey() = 0;
        virtual void setSubSystemKey(const ta_uint32& _nSubSystemKey) = 0;

        // Display Condition
        virtual DISPLAY_CONDITION getDisplayCondition() = 0;
        virtual void setDisplayCondition(const DISPLAY_CONDITION& _nDisplayCondition) = 0;

        // Deleted
        virtual ta_uint32 getDeleted() = 0;
        virtual void setDeleted(const ta_uint32& _isDeleted) = 0;
        /*
        * virtual destructor
        */
        virtual ~IAlarmDisplayFilterData() {}
    };

}//close namespace TA_Base_Core

#endif // __IALARM_DISPLAY_FILTER_DATA_H_INCLUDED__
