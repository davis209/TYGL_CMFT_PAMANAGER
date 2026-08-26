/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/Gui.h $
 * @author:  Karen Graham
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * Gui is an implementation of IGui. It holds the data specific to an gui entry
 * in the database, and allows read-only access to that data.
 */

#if !defined(Gui_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define Gui_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

#include "core/types/src/ta_types.h"
#include <string>
#include <vector>
#include "core/data_access_interface/src/IGui.h"

namespace TA_Base_Core
{
    class IData;
    class GuiHelper;

    class Gui : public IGui
    {
    public:

        /**
         * Constructor
         *
         * Construct an Gui class based around a key.
         *
         * @param key The key of this Gui in the database
         */
        Gui(const ta_uint32 key);

        /**
         * Constructor
         *
         * Construct a Gui class based around dataset
         */
        Gui(const ta_uint32 row, TA_Base_Core::IData& data);

        /**
         * Destructor
         */
        virtual ~Gui();

        /**
         * getKey
         *
         * Returns the key for this Gui.
         *
         * @return The key for this gui as an ta_uint32 .
         */
        virtual ta_uint32 getKey();

        /**
         * getName
         *
         * Returns the name of this Gui. If this is the first time data for this gui
         * has been requested, all the data is loaded from the database.
         *
         * @return The name for this gui as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. ACCESSLEVEL should be an
         *             ta_uint32 ), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getName();

        /**
         * getName
         *
         * Returns the full name of this Gui. If this is the first time data for this gui
         * has been requested, all the data is loaded from the database.
         *
         * @return The name for this gui as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. ACCESSLEVEL should be an
         *             ta_uint32 ), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getFullName();

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
        virtual std::string getExecutable();

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
        virtual std::string getHelpFilePath();

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
        virtual bool isIconDisplayed();

       
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
        virtual ta_uint32 getAppMaxInstances();

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
        virtual void setAppMaxInstances(const std::string& numInstances);

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
        virtual bool shouldReposition();

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
        virtual bool shouldPassParameters();

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
        virtual bool shouldTerminateOnLogout();

        /**
         * invalidate
         *
         * Make the data contained by this gui as invalid. The next call to get...()
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();

    private:

        // Copy constructor and assignment operator are not used so they are made private
        Gui(const Gui& theGui);
        Gui& operator=(const Gui&);

        TA_Base_Core::GuiHelper* m_guiHelper;
    };
} // closes TA_Base_Core

#endif // !defined(Gui_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
