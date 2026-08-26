/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source: $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/Operator.cpp $
 * @author Nick Jardine
 * @version $Revision: #3 $
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by: $Author: CM $
 *
 * Operator is an implementation of IOperator. It holds the data specific to an operator entry
 * in the database, and allows read-only access to that data.
 */

#include "core/types/src/ta_types.h"
#include "core/data_access_interface/src/Operator.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
    Operator::Operator(const ta_uint32 key)
        : m_OperatorHelper(new OperatorHelper(key))
    {
    }

    Operator::Operator(const ta_uint32 key,
                       const std::string& name, bool isSystemOperator)
    {
        m_OperatorHelper = new OperatorHelper(key, name, isSystemOperator);
    }

    Operator::Operator(const ta_uint32 key,
                       const std::string& name,
                       const std::string& description,
                       const std::string& password,
                       bool isSystemOperator,
                       time_t dateCreated,
                       time_t dateModified,
                       bool loadProfiles, /*=false*/
                       bool loadRegions /*=false*/)
    {
        m_OperatorHelper = new OperatorHelper(key, name, description, password, isSystemOperator,
                                              dateCreated, dateModified, loadProfiles, loadRegions);
    }

    Operator::Operator(const ta_uint32 row, TA_Base_Core::IData& data, std::vector<ta_uint32>& profiles, std::vector<ta_uint32>& regions)
        : m_OperatorHelper(new OperatorHelper(row, data, profiles, regions))
    {
    }

    Operator::~Operator()
    {
        delete m_OperatorHelper;
        m_OperatorHelper = NULL;
    }

    ta_uint32 Operator::getKey()
    {
        TA_ASSERT(m_OperatorHelper != NULL, "The OperatorHelper pointer is null.");
        return m_OperatorHelper->getKey();
    }

    std::string Operator::getName()
    {
        TA_ASSERT(m_OperatorHelper != NULL, "The OperatorHelper pointer is null.");
        return m_OperatorHelper->getName();
    }

    std::string Operator::getDescription()
    {
        TA_ASSERT(m_OperatorHelper != NULL, "The OperatorHelper pointer is null.");
        return m_OperatorHelper->getDescription();
    }

    std::string Operator::getPassword(const std::string& options)
    {
        TA_ASSERT(m_OperatorHelper != NULL, "The OperatorHelper pointer is null.");
        return m_OperatorHelper->getPassword(options);
    }

    void Operator::setPassword(const std::string password)
    {
        TA_ASSERT(m_OperatorHelper != NULL, "The OperatorHelper pointer is null.");
        m_OperatorHelper->setPassword(password);
        m_OperatorHelper->writeOperatorData();
    }

    bool Operator::isSystemOperator()
    {
        TA_ASSERT(m_OperatorHelper != NULL, "The OperatorHelper pointer is null.");
        return m_OperatorHelper->isSystemOperator();
    }

    std::vector<IProfile*> Operator::getAssociatedProfiles()
    {
        TA_ASSERT(m_OperatorHelper != NULL, "The OperatorHelper pointer is null.");
        return m_OperatorHelper->getAssociatedProfiles();
    }

    std::vector<ta_uint32> Operator::getAssociatedProfileKeys()
    {
        TA_ASSERT(m_OperatorHelper != NULL, "The OperatorHelper pointer is null.");
        return m_OperatorHelper->getAssociatedProfileKeys();
    }

    std::vector<ta_uint32> Operator::getAssociatedRegionKeys()
    {
        TA_ASSERT(m_OperatorHelper != NULL, "The OperatorHelper pointer is null.");
        return m_OperatorHelper->getAssociatedRegionKeys();
    }

    void Operator::invalidate()
    {
        TA_ASSERT(m_OperatorHelper != NULL, "The OperatorHelper pointer is null.");
        m_OperatorHelper->invalidate();
    }

} // closes TA_Base_Core
