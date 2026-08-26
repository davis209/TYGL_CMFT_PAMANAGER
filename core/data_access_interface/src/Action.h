/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/Action.h $
 * @author:  Karen Graham
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * Action is an implementation of IAction. It holds the data specific to an action entry
 * in the database, and allows read-only access to that data.
 *
 */

#pragma once
#include "core/types/src/ta_types.h"
#include "core/data_access_interface/src/IAction.h"
#include "core/data_access_interface/src/ActionHelper.h"
#include <memory>

namespace TA_Base_Core
{
    class ActionHelper;

    class Action : public IAction
    {
    public:

        /**
         * Constructor
         *
         * Construct an Action class based around a key.
         *
         * @param key The key of this Action in the database
         */
        Action(const ta_uint32 key);

        /**
         * Constructor
         *
         * Construct an Action class based around all data (except the duties).
         *
         * @param key The key of this Action in the database
         * @param name The name of this Action in the database
         * @param description The description of this Action in the database
         */
        Action(const ta_uint32 key, const std::string& name, const std::string& description);

        /**
         * Constructor
         *
         * Construct an Action class based around all data (except the duties).
         *
         * @param key The key of this Action in the database
         * @param name The name of this Action in the database
         * @param description The description of this Action in the database
         * @param isDisplayOnly the IS_DISPLAY_ONLY of thsi Action in the database
         */
        Action(const ta_uint32 key, const std::string& name, const std::string& description, const bool isDisplayOnly);
        /**
         * Constructor
         *
         * Construct a Action class based around action dataset
         */
        Action(const ta_uint32 row, TA_Base_Core::IData& data, std::vector<std::string>& actionGroups);

        /**
         * Refer to IAction.h for a description of this method.
         */
        virtual ta_uint32 getKey();

        /**
         * Refer to IAction.h for a description of this method.
         */
        virtual std::string getName();

        /**
         * Refer to IAction.h for a description of this method.
         */
        virtual std::string getDescription();

        /**
         * See IAction.h for a description of this method.
         */
        virtual std::vector<std::string> getActionGroups();

        /**
         * Refer to IAction.h for a description of this method.
         */
        virtual void invalidate();

        virtual bool isDisplayOnly();

    private:

        // Assignment operator not used so it is made private
        Action(const Action&);
        Action& operator=(const Action&);

        std::auto_ptr<ActionHelper> m_helper;
    };

    using ActionPtr = boost::shared_ptr<Action>;
    using ActionPtrList = std::vector<ActionPtr>;
}
