/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/Session.h $
 * @author:  Karen Graham
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * Session is an implementation of ISession. It holds the data specific to an Session entry
 * in the database, and allows read-only access to that data.
 */

#if !defined(Session_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define Session_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

#include "core/types/src/ta_types.h"
#include <string>

#include "core/data_access_interface/src/ISession.h"

namespace TA_Base_Core
{
    class Session : public ISession
    {
    public:

        /**
         * Constructor
         *
         * Construct an Session class based around a key.
         *
         * @param key The key of this Session in the database
         */
        Session(const std::string& sessionId);

        /**
         * Session
         *
         * Create a new session that needs to be written to the database (using applyChanges),
         * the sessionID will be created automagically
         *
         * @param   ta_uint32 operatorKey
         * @param ta_uint32 consoleKey
         * @param ta_uint32 profileKey
         */
        Session(ta_uint32 operatorKey, ta_uint32 consoleKey, ta_uint32 profileKey,
                ta_uint32 locationKey, ta_uint32 entityKey, bool displayOnly);

        /**
         * Session
         *
         * Create a new session that needs to be written to the database (using applyChanges)
         *
         * @param std::string sessionID
         * @param ta_uint32 operatorKey
         * @param ta_uint32 consoleKey
         * @param ta_uint32 profileKey
         * @param bool isNew - indicates if this is a new session yet to be written to the database
         */
        Session(const std::string& sessionID,
                ta_uint32 operatorKey,
                ta_uint32 consoleKey,
                ta_uint32 profileKey,
                ta_uint32 locationKey,
                ta_uint32 entityKey,
                bool          displayOnly,
                bool          hasEnded,
                bool          isNew);

        /**
         * Destructor
         */
        virtual ~Session();

        /**
         * getSessionId
         *
         * Returns the sessionId for this PMSESSION entry.
         *
         * @return The key (uuid) for this Session as a string.
         */
        virtual std::string getSessionId();

        /**
         * getOperatorKey
         *
         * Returns the key for the operator of this session
         *
         * @return The key for the operator as an ta_uint32 .
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. OPERATORKEY should be an
         *             ta_uint32 ), or if the wrong ammount of data is retrieved.
         */
        virtual ta_uint32 getOperatorKey();

        /**
         * getOperatorName
         *
         * Returns the name for the operator of this session
         *
         * @return The name for the operator as an ta_uint32 .
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. OPERATORKEY should be an
         *             ta_uint32 ), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getOperatorName();

        /**
         * getConsoleKey
         *
         * Returns the key for the console of this session
         *
         * @return The key for the console as an ta_uint32 .
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. OPERATORKEY should be an
         *             ta_uint32 ), or if the wrong ammount of data is retrieved.
         */
        virtual ta_uint32 getConsoleKey();

        /**
         * getProfileKey
         *
         * Returns the key for the profile of this session
         *
         * @return The key for the profile as an ta_uint32 .
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. OPERATORKEY should be an
         *             ta_uint32 ), or if the wrong ammount of data is retrieved.
         */
        virtual ta_uint32 getProfileKey();

        /**
         * getLocationKey
         *
         * Returns the key for the location of this session
         *
         * @return The key for the location as an ta_uint32 .
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. OPERATORKEY should be an
         *             ta_uint32 ), or if the wrong ammount of data is retrieved.
         */
        virtual ta_uint32 getLocationKey();

        /**
         * getDisplayMode
         *
         * Returns the status of display mode
         *
         * @return A boolean indicating whether the session is display only
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. OPERATORKEY should be an
         *             ta_uint32 ), or if the wrong ammount of data is retrieved.
         */
        virtual bool getDisplayMode();

        /**
         * getHasEnded
         *
         * Returns the ended status of this session
         *
         * @return A boolean indicating whether the session has ended.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. OPERATORKEY should be an
         *             ta_uint32 ), or if the wrong ammount of data is retrieved.
         */
        virtual bool getHasEnded();

        /**
         * setHasEnded
         *
         * Marks the session as ended
         *
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. OPERATORKEY should be an
         *             ta_uint32 ), or if the wrong ammount of data is retrieved.
         */
        virtual void setHasEnded();

        /**
         * invalidate
         *
         * Make the data contained by this Session as invalid. The next call to get...()
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();

        /**
         * applyChanges
         *
         * This will apply all changes made to the database.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if the a parameter name cannot be found, or if
         *            there is more than one value for a parmeter.
         * @exception DataConfigurationException If the data contained in the Operator object
         *            is not sufficent to create an entry in the database, a
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in
         *            all sections.
         */
        virtual void applyChanges();
        virtual ta_uint32 getOverrideOperatorKey();
        virtual ta_uint32 getOverrideProfileKey();
        virtual void setDisplayOnly(bool displayOnly);
        void updateOverrideInfoToDb(ta_uint32 userOverrideId, ta_uint32 profileOverrideId);
        void updateOverrideInfoToDb();

    private:

        // Copy constructor and assignment operator are not used so they are made private
        Session(const Session& theSession);
        Session& operator=(const Session&);
        Session();

        /**
         * reload()
         *
         * This method reloads the data from the database. It is called when a get... method
         * is called and the data state is not valid.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. OPERATORKEY should be an
         *             ta_uint32 ), or if the wrong ammount of data is retrieved.
         */
        void reload();

        /**
         * getLocalDatabaseName
         *
         * returns the name of the local database
         */
        std::string getLocalDatabaseName();

        virtual ta_uint32 getEntityKey();

    private:

        // The name of the local database to send updates to
        std::string m_localDatabase;

        std::string m_sessionId;
        ta_uint32 m_operatorKey;
        ta_uint32 m_consoleKey;
        ta_uint32 m_profileKey;
        ta_uint32 m_locationKey;
        ta_uint32 m_entityKey;
        ta_uint32 m_overrideOperatorKey;
        ta_uint32 m_overrideProfileKey;
        bool          m_hasEnded;
        bool          m_displayOnly;
        std::string m_operatorName;

        bool m_isValidData;
        bool m_isWritten;
        bool m_isUpdateOnly;
    };
} // closes TA_Base_Core

#endif // !defined(Session_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
