/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/SystemData.h $
 * @author:  Lu Xiangmei
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * This is a data structure for "System" data, which also correspond with the table "System"
 *
 * All data is populated when the object is constructed and is read-only.
 */

#if !defined(_SYSTEM_DATA_H_)
#define _SYSTEM_DATA_H_

#include "core/types/src/ta_types.h"
#include <string>

namespace TA_Base_Core
{
    class SystemData
    {
    public:

        /**
         * Constructor
         *
         * This constructs a DatalogConfigSettingsData object that has an initial state.
         * If data is requested it will NOT be loaded from the database.
         *
         */
        SystemData
        (const ta_uint32 pkey,
         std::string name,
         const bool is_physical,
         const bool is_exclusive,
         const bool is_locationexclusive) :
            m_pkey(pkey), m_name(name), m_isPhysical(is_physical), m_isExclusive(is_exclusive), m_isLocationExclusive(is_locationexclusive)
        {
        }

        ta_uint32       getPkey();
        std::string        getName();
        bool               getIsPhysical();
        bool               getIsExclusive();
        bool               getIsLocationExclusive();

    private:

        // Variables used to store event data.
        ta_uint32       m_pkey;
        std::string        m_name;
        bool               m_isPhysical;
        bool               m_isExclusive;     //operator exclusive 
        bool               m_isLocationExclusive;  //location exclusive
    };
};

#endif // !defined(_SYSTEM_DATA_H_)
