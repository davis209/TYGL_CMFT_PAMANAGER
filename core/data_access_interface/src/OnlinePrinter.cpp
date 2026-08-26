/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source: $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/OnlinePrinter.cpp $
 * @author Nick Jardine
 * @version $Revision: #3 $
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by: $Author: CM $
 *
 * OnlinePrinter is an implementation of IOnlinePrinter. It holds the data specific to an OnlinePrinter entry
 * in the database, and allows read-only access to that data.
 */

#if defined(_MSC_VER)
    #pragma warning(disable:4290)   // C++ Exception Specification ignored
#endif // defined _MSC_VER

#include "core/types/src/ta_types.h"
#include "core/data_access_interface/src/OnlinePrinter.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
    OnlinePrinter::OnlinePrinter(const ta_uint32 key)
    {
        m_OnlinePrinterHelper = new OnlinePrinterHelper(key);
    }

    OnlinePrinter::OnlinePrinter(ta_uint32 row, TA_Base_Core::IData& data)
        : m_OnlinePrinterHelper(new OnlinePrinterHelper(row, data))
    {
    }

    OnlinePrinter::~OnlinePrinter()
    {
        delete m_OnlinePrinterHelper;
        m_OnlinePrinterHelper = NULL;
    }

    std::string OnlinePrinter::getPrinter()
    {
        TA_ASSERT(m_OnlinePrinterHelper != NULL, "The OnlinePrinterHelper pointer is null.");
        return m_OnlinePrinterHelper->getPrinter();
    }

    ta_uint32 OnlinePrinter::getLocation()
    {
        TA_ASSERT(m_OnlinePrinterHelper != NULL, "The OnlinePrinterHelper pointer is null.");
        return m_OnlinePrinterHelper->getLocation();
    }

    ta_uint32 OnlinePrinter::getSubsystem()
    {
        TA_ASSERT(m_OnlinePrinterHelper != NULL, "The OnlinePrinterHelper pointer is null.");
        return m_OnlinePrinterHelper->getSubsystem();
    }

    bool OnlinePrinter::getIsAlarm()
    {
        TA_ASSERT(m_OnlinePrinterHelper != NULL, "The OnlinePrinterHelper pointer is null.");
        return m_OnlinePrinterHelper->getIsAlarm();
    }

    ta_uint32 OnlinePrinter::getKey()
    {
        TA_ASSERT(m_OnlinePrinterHelper != NULL, "The OnlinePrinterHelper pointer is null.");
        return m_OnlinePrinterHelper->getKey();
    }

    std::string OnlinePrinter::getName()
    {
        TA_ASSERT(m_OnlinePrinterHelper != NULL, "The OnlinePrinterHelper pointer is null.");

        std::ostringstream name;
        name << m_OnlinePrinterHelper->getPrinter();
        name << " - ";
        name << m_OnlinePrinterHelper->getSubsystem();

        return name.str();
    }

    void OnlinePrinter::invalidate()
    {
        TA_ASSERT(m_OnlinePrinterHelper != NULL, "The OnlinePrinterHelper pointer is null.");
        m_OnlinePrinterHelper->invalidate();
    }

} // closes TA_Core
