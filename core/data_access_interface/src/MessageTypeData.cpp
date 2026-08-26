/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/MessageTypeData.cpp $
 * @author:  Nick Jardine
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * MessageTypeData is an implementation of IMessageTypeData. It holds the data specific to a message
 * type entry in the database.
 */

#include "core/types/src/ta_types.h"
#include "core/data_access_interface/src/MessageTypeData.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
    MessageTypeData::MessageTypeData(const std::string& key)
        : m_strKey(key)
    {
        m_messageHelper = new MessageTypeHelper(key);
    }

    MessageTypeData::MessageTypeData(const std::string& key,
                                     const std::string& name,
                                     const std::string& description,
                                     int priority,
                                     bool toBePrinted,
                                     bool toBeLogged,
                                     bool toBePersisted,
                                     unsigned int viewLevel,
                                     bool isRealTimeEvent,
                                     bool isSystemEvent,
                                     ta_uint32 actionID)
        : m_messageHelper(new MessageTypeHelper(key, name, description, priority, toBePrinted, toBeLogged, toBePersisted, viewLevel, isRealTimeEvent, isSystemEvent, actionID))
    {
    }

    MessageTypeData::MessageTypeData(const ta_uint32 row, TA_Base_Core::IData& data)
        : m_messageHelper(new MessageTypeHelper(row, data))
    {
    }

    ta_uint32 MessageTypeData::getKey()
    {
        TA_ASSERT(m_messageHelper != NULL, "The MessageHelper pointer is null.");

        return m_messageHelper->getKey();
    }

    std::string MessageTypeData::getName()
    {
        TA_ASSERT(m_messageHelper != NULL, "The MessageHelper pointer is null.");
        return m_messageHelper->getName();
    }

    std::string MessageTypeData::getDisplayName()
    {
        TA_ASSERT(m_messageHelper != NULL, "The MessageHelper pointer is null.");
        return m_messageHelper->getDisplayName();
    }

    std::string MessageTypeData::getDescription()
    {
        TA_ASSERT(m_messageHelper != NULL, "The MessageHelper pointer is null.");
        return m_messageHelper->getDescription();
    }

    unsigned int MessageTypeData::getPriority()
    {
        TA_ASSERT(m_messageHelper != NULL, "The MessageHelper pointer is null.");
        return m_messageHelper->getPriority();
    }

    bool MessageTypeData::toBePrinted()
    {
        TA_ASSERT(m_messageHelper != NULL, "The MessageHelper pointer is null.");
        return m_messageHelper->toBePrinted();
    }

    bool MessageTypeData::toBeLogged()
    {
        TA_ASSERT(m_messageHelper != NULL, "The MessageHelper pointer is null.");
        return m_messageHelper->toBeLogged();
    }

    bool MessageTypeData::toBePersisted()
    {
        TA_ASSERT(m_messageHelper != NULL, "The MessageHelper pointer is null.");
        return m_messageHelper->toBePersisted();
    }

    time_t MessageTypeData::getDateCreated()
    {
        TA_ASSERT(m_messageHelper != NULL, "The MessageHelper pointer is null.");
        return m_messageHelper->getDateCreated();
    }

    time_t MessageTypeData::getDateModified()
    {
        TA_ASSERT(m_messageHelper != NULL, "The MessageHelper pointer is null.");
        return m_messageHelper->getDateModified();
    }

    unsigned int MessageTypeData::getViewLevel()
    {
        TA_ASSERT(m_messageHelper != NULL, "The MessageHelper pointer is null.");
        return m_messageHelper->getViewLevel();
    }

    ta_uint32 MessageTypeData::getActionId()
    {
        TA_ASSERT(m_messageHelper != NULL, "The MessageHelper pointer is null.");
        return m_messageHelper->getActionId();
    }

    bool MessageTypeData::getIsEnableRealTimeEvent()
    {
        TA_ASSERT(m_messageHelper != NULL, "The MessageHelper pointer is null.");
        return m_messageHelper->getIsEnableRealTimeEvent();
    }

    void MessageTypeData::invalidate()
    {
        TA_ASSERT(m_messageHelper != NULL, "The MessageHelper pointer is null.");
        m_messageHelper->invalidate();
    }

    bool MessageTypeData::isSystemEvent()
    {
        TA_ASSERT(m_messageHelper != NULL, "The MessageHelper pointer is null.");
        return m_messageHelper->isSystemEvent();
    }
} // closes TA_Base_Core
