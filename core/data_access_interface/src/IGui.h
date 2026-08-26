/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/IGui.h $
 * @author:  Karen Graham
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * IGui is an interface to an gui object. It allows the gui object implementation
 * to be changed (e.g. if necessary due to a database schema change) without changing code
 * that uses it.
 */

#if !defined(IGui_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IGui_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include "core/types/src/ta_types.h"
#include <string>

#include "core/data_access_interface/src/IItem.h"

namespace TA_Base_Core
{
    class IGui : public virtual IItem
    {
    public:

        virtual ~IGui() {};

        /**
         * getFullName
         *
         * Returns the full name of this Gui. If this is the first time data for this gui
         * has been requested, all the data is loaded from the database.
         *
         * @return The name for this operator as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. ACCESSLEVEL should be an
         *             ta_uint32 ), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getFullName() = 0;

        /**
         * getExecutable
         *
         * Returns the executable for this Gui.
         *
         * @return The executable for this Gui as a std::string.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. ACCESSLEVEL should be an
         *             ta_uint32 ), or if the wrong amount of data is retrieved.
         */
        virtual std::string getExecutable() = 0;

        /**
         * getHelpFilePath
         *
         * Returns the help file path for the Gui
         *
         * @return The help file path for this Gui as a std::string.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. ACCESSLEVEL should be an
         *             ta_uint32 ), or if the wrong amount of data is retrieved.
         */
        virtual std::string getHelpFilePath() = 0;

        /**
         * isIconDisplayed
         *
         * Returns whether the icon should be displayed for this Gui
         *
         * @return true if the GUI should be displayed.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. ACCESSLEVEL should be an
         *             ta_uint32 ), or if the wrong ammount of data is retrieved.
         */
        virtual bool isIconDisplayed() = 0;        

        /**
          * getAppMaxInstances
          *
          *
          * @return the maximum number of application instances. 0 means infinte number of instances.
          *
          * @exception DatabaseException A DatabaseException will be thrown if there is a
          *            problem establishing a connection with the database, or if an
          *            error is encountered while retrieving data.
          * @exception DataException A DataException will be thrown if the data cannot be
          *            converted to the required format (e.g. ACCESSLEVEL should be an
          *             ta_uint32 ), or if the wrong ammount of data is retrieved.
          *
          * pre: Either - this gui was initially loaded from the database
          *      OR     - setAccessLevel() has already been called
          */
        virtual ta_uint32 getAppMaxInstances() = 0;

        /**
          * setAppMaxInstances
          *
          *
          * Set the maximum number of application instances that will be launched.
          *
          * @exception DatabaseException A DatabaseException will be thrown if there is a
          *            problem establishing a connection with the database, or if an
          *            error is encountered while retrieving data.
          * @exception DataException A DataException will be thrown if the data cannot be
          *            converted to the required format (e.g. ACCESSLEVEL should be an
          *             ta_uint32 ), or if the wrong ammount of data is retrieved.
          *
          * pre: Either - this gui was initially loaded from the database
          *      OR     - setAccessLevel() has already been called
          */
        virtual void setAppMaxInstances(const std::string &numInstances) = 0;

        /**
         * shouldReposition
         *
         * Returns whether the application should be repositioned if it should not be relaunched
         * (only meaningful if application is limited to 1 running instance).
         *
         * @return true if the GUI should be repositioned
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. ACCESSLEVEL should be an
         *             ta_uint32 ), or if the wrong ammount of data is retrieved.
         */
        virtual bool shouldReposition() = 0;

        /**
         * shouldPassParameters
         *
         * Returns whether the application should be re-passed the parameters if it
         * should not be relaunched
         *
         * @return true if the GUI should be passed the parameters
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. ACCESSLEVEL should be an
         *             ta_uint32 ), or if the wrong ammount of data is retrieved.
         */
        virtual bool shouldPassParameters() = 0;

        /**
         * shouldTerminateOnLogout
         *
         * Returns whether the application should be closed when the user logs out of the system
         *
         * @return true if the GUI should terminate on a logout
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. ACCESSLEVEL should be an
         *             ta_uint32 ), or if the wrong ammount of data is retrieved.
         */
        virtual bool shouldTerminateOnLogout() = 0;
    };

    typedef boost::shared_ptr<IGui> IGuiPtr;
    typedef std::vector<IGuiPtr> IGuiPtrList;
    typedef boost::shared_ptr<IGuiPtrList> IGuiPtrListPtr;
}

#endif // !defined(IGui_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
