/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/AlarmStateData.cpp $
 * @author:  Xiangmei.Lu
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * AlarmStateData is an implementation of IAlarmTypeData. It holds the data specific to a Alarm
 * state entry in the database.
 */

#ifdef _MSC_VER
    #pragma warning(disable:4786)
#endif // _MSC_VER

#include "core/types/src/ta_types.h"
#include "core/data_access_interface/src/AlarmStateData.h"

namespace TA_Base_Core
{
    ta_uint32 AlarmStateData::getKey()
    {
        return m_key;
    }
    ta_uint32 AlarmStateData::getTypeKey()
    {
        return m_typeKey;
    }

    std::string AlarmStateData::getName()
    {
        return m_name;
    }

    std::string AlarmStateData::getDisplayName()
    {
        return m_displayName;
    }

} // closes TA_Base_Core
