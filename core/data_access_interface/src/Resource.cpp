/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/Resource.cpp $
 * @author:  Julian Chevalley
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * Implementation of the Resource class
 *
 */

#ifdef _MSC_VER
    #pragma warning(disable:4786)
#endif // _MSC_VER

#include "core/types/src/ta_types.h"
#include "core/data_access_interface/src/Resource.h"

namespace TA_Base_Core
{
    Resource::Resource(const ta_uint32 key)
        : m_helper(new ResourceHelper(key))
    {
    }

    Resource::Resource(const ta_uint32 row, TA_Base_Core::IData& data)
        : m_helper(new ResourceHelper(row, data))
    {
    }

    Resource::~Resource()
    {
        if (NULL !=  m_helper)
        {
            delete m_helper;
        }
    }

    ta_uint32 Resource::getKey()
    {
        return m_helper->getKey();
    }

    ta_uint32 Resource::getSubsystem()
    {
        return m_helper->getSubsystem();
    }

} // TA_Base_Core
