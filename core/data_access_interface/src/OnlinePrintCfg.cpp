/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source: $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/OnlinePrintCfg.cpp $
 * @author Nick Jardine
 * @version $Revision: #3 $
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by: $Author: CM $
 *
 * OnlinePrintCfg is an implementation of IOnlinePrintCfg. It holds the data specific to an OnlinePrintCfg entry
 * in the database, and allows read-only access to that data.
 */

#if defined(_MSC_VER)
    #pragma warning(disable:4290)   // C++ Exception Specification ignored
#endif // defined _MSC_VER

#include "core/types/src/ta_types.h"
#include "core/data_access_interface/src/OnlinePrintCfg.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
    OnlinePrintCfg::OnlinePrintCfg(const ta_uint32 idKey)
    {
        m_OnlinePrintCfgHelper = new OnlinePrintCfgHelper(idKey);
    }

    OnlinePrintCfg::OnlinePrintCfg(ta_uint32 row, TA_Base_Core::IData& data)
        : m_OnlinePrintCfgHelper(new OnlinePrintCfgHelper(row, data))
    {
    }

    OnlinePrintCfg::~OnlinePrintCfg()
    {
        delete m_OnlinePrintCfgHelper;
        m_OnlinePrintCfgHelper = NULL;
    }

    ta_uint32 OnlinePrintCfg::getKey()
    {
        return getLocation();
    }

    std::string OnlinePrintCfg::getDefaultPrinter()
    {
        TA_ASSERT(m_OnlinePrintCfgHelper != NULL, "The OnlinePrintCfgHelper pointer is null.");
        return m_OnlinePrintCfgHelper->getDefaultPrinter();
    }

    ta_uint32 OnlinePrintCfg::getLocation()
    {
        TA_ASSERT(m_OnlinePrintCfgHelper != NULL, "The OnlinePrintCfgHelper pointer is null.");
        return m_OnlinePrintCfgHelper->getLocation();
    }

    std::string OnlinePrintCfg::getLocationName()
    {
        TA_ASSERT(m_OnlinePrintCfgHelper != NULL, "The OnlinePrintCfgHelper pointer is null.");
        return m_OnlinePrintCfgHelper->getLocationName();
    }
    std::string OnlinePrintCfg::getLocationDisplayName()
    {
        TA_ASSERT(m_OnlinePrintCfgHelper != NULL, "The OnlinePrintCfgHelper pointer is null.");
        return m_OnlinePrintCfgHelper->getLocationDisplayName();
    }

    std::string OnlinePrintCfg::getName()
    {
        TA_ASSERT(m_OnlinePrintCfgHelper != NULL, "The OnlinePrintCfgHelper pointer is null.");

        std::ostringstream name;
        name << m_OnlinePrintCfgHelper->getDefaultPrinter();
        name << " - ";
        name << m_OnlinePrintCfgHelper->getLocation();

        return name.str();
    }

    void OnlinePrintCfg::invalidate()
    {
        TA_ASSERT(m_OnlinePrintCfgHelper != NULL, "The OnlinePrintCfgHelper pointer is null.");
        m_OnlinePrintCfgHelper->invalidate();
    }

} // closes TA_Core
