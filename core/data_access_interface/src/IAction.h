/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/IAction.h $
 * @author:  Karen Graham
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * IAction is an interface to a action object. It allows the action object implementation
 * to be chagned (e.g. if necessary due to a database schema change) without changing code
 * that uses it.
 *
 */

#pragma once
#include "core/types/src/ta_types.h"
#include <boost/shared_ptr.hpp>
#include <string>
#include <vector>

namespace TA_Base_Core
{
    class IAction
    {
    public:

        virtual ~IAction() {}

        /**
         * getKey
         *
         * Returns the key for this Action.
         *
         * @return The key for this action as an ta_uint32 .
         */
        virtual ta_uint32 getKey() = 0;

        /**
         * getName
         *
         * Returns the name of this Action. If this is the first time data for this action
         * has been requested, all the data is loaded from the database.
         *
         * @return The name for this action as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. ACCESSLEVEL should be an
         *             ta_uint32 ), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getName() = 0;

        /**
         * getDescription
         *
         * Returns the description of this Action. If this is the first time data for this action
         * has been requested, all the data is loaded from the database.
         *
         * @return The description for this action as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. ACCESSLEVEL should be an
         *             ta_uint32 ), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getDescription() = 0;

        /**
         * getActionGroups
         *
         * This method retrieves the ActionGroup that this action has.
         *
         *
         * @return A std::vector<std::string> containing all the ActionGroup
         *         names. A std::auto_ptr<std::vector<std::string> > of size
         *         0 will be returned if no ActionGroups are configured for the resource/action
         *         combination.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. ACCESSLEVEL should be an
         *             ta_uint32 ), or if the wrong ammount of data is retrieved.
         */
        virtual std::vector<std::string> getActionGroups() = 0;

        /**
         * invalidate
         *
         * Mark the data contained by this action as invalid. The next call to get...()
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate() = 0;

        virtual bool isDisplayOnly() = 0;
    };

    using IActionPtr = boost::shared_ptr<IAction>;
    using IActionPtrList = std::vector<IActionPtr>;
}
