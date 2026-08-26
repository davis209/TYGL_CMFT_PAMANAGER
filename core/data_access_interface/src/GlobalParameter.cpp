/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/GlobalParameter.cpp $
 * @author:  Karen Graham
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * GlobalParameter is an implementation of IGlobalParameter. It holds the data specific to an GlobalParameter entry
 * in the database, and allows read-only access to that data.
 *
 */

#ifdef __WIN32__
    #pragma warning(disable:4786)
    #pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include "core/types/src/ta_types.h"
#include "core/data_access_interface/src/GlobalParameter.h"
#include "core/data_access_interface/src/GlobalParameterHelper.h"
#include "core/data_access_interface/src/GlobalParameterAccessFactory.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
    GlobalParameter::GlobalParameter(const ta_uint32 key)
        : m_GlobalParameterHelper(new GlobalParameterHelper(key))
    {
    }

    GlobalParameter::GlobalParameter(const ta_uint32 row, TA_Base_Core::IData& data)
        : m_GlobalParameterHelper(new GlobalParameterHelper(row, data))
    {
    }

    GlobalParameter::~GlobalParameter()
    {
        try
        {
            if (m_GlobalParameterHelper != NULL)
            {
                delete m_GlobalParameterHelper;
                m_GlobalParameterHelper = NULL;
            }
        }
        catch (...)
        {
            LOG_EXCEPTION("Unknown", "Caught in destructor");
        }
    }

    ta_uint32 GlobalParameter::getKey()
    {
        TA_ASSERT(m_GlobalParameterHelper != NULL, "The GlobalParameter helper is NULL");

        return m_GlobalParameterHelper->getKey();
    }

    std::string GlobalParameter::getName()
    {
        TA_ASSERT(m_GlobalParameterHelper != NULL, "The GlobalParameter helper is NULL");

        return m_GlobalParameterHelper->getName();
    }

    std::string GlobalParameter::getValue()
    {
        TA_ASSERT(m_GlobalParameterHelper != NULL, "The GlobalParameter helper is NULL");

        return m_GlobalParameterHelper->getValue();
    }

    ta_uint32 GlobalParameter::getSubsystemKey()
    {
        TA_ASSERT(m_GlobalParameterHelper != NULL, "The GlobalParameter helper is NULL");

        return m_GlobalParameterHelper->getSubsystemKey();
    }

    void GlobalParameter::invalidate()
    {
        TA_ASSERT(m_GlobalParameterHelper != NULL, "The GlobalParameter helper is NULL");

        m_GlobalParameterHelper->invalidate();
    }

} // closes TA_Base_Core
