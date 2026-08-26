/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/SystemData.cpp $
 * @author:  xiangmei.lu
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * IAlarmData is an interface to the Alarm table. It provides both read and write access
 * for agents and user applicaitons.
 */

#ifdef __WIN32__
    #pragma warning(disable:4786)
#endif

#include "core/types/src/ta_types.h"
#include "SystemData.h"

namespace TA_Base_Core
{
    ta_uint32 SystemData::getPkey()
    {
        return m_pkey;
    }

    std::string SystemData::getName()
    {
        return m_name;
    }

    bool SystemData::getIsPhysical()
    {
        return m_isPhysical;
    }

    bool SystemData::getIsExclusive()
    {
        return m_isExclusive;
    }

    bool SystemData::getIsLocationExclusive()
    {
        return m_isLocationExclusive;
    }

} // closes TA_Base_Core
