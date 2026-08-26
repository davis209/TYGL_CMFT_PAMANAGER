/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/IActionGroup.h $
 * @author:  Karen Graham
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * IActionGroup is an interface to an ActionGroup object. It allows the ActionGroup object implementation
 * to be chagned (e.g. if necessary due to a database schema change) without changing code
 * that uses it.
 */

#if !defined(IActionGroup_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IActionGroup_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include "core/types/src/ta_types.h"
#include <string>

#include "core/data_access_interface/src/IItem.h"

namespace TA_Base_Core
{
    class IActionGroup : public IItem
    {
    public:

        virtual ~IActionGroup() {};

        /**
         * isControlType
         *
         * Returns whether or not the isControlType flag is set in the database.
         *
         * @return boolean - True if the action group is a control type. False otherwise
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. ACCESSLEVEL should be an
         *             ta_uint32 ), or if the wrong ammount of data is retrieved.
         */
        virtual bool isControlType() = 0;

        /**
         * getDisplayName
         *
         * @return std::string display name of the action group
         *
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. ACCESSLEVEL should be an
         *             ta_uint32 ), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getDisplayName() = 0;
    };

    typedef boost::shared_ptr<IActionGroup> IActionGroupPtr;
    typedef std::vector<IActionGroupPtr> IActionGroupPtrList;
}

#endif // !defined(IActionGroup_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
