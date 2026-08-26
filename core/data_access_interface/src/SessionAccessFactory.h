/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/SessionAccessFactory.h $
 * @author:  Karen Graham
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * SessionAccessFactory is a singleton that is used to retrieve Session objects either from the
 * database or newly created. All Session objects returned will adhear to the ISession interface.
 */

#if !defined(SESSIONACCESSFACTORY_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define SESSIONACCESSFACTORY_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include "core/types/src/ta_types.h"
#include <string>
#include <vector>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/ISession.h"

namespace TA_Base_Core
{
    class SessionAccessFactory
    {
    public:

        enum SESSION_FLAG { ERROR_SESSION_ID_FALG, CURRENT_SESSION_ID_FALG, OBSELETE_SESSION_ID_FALG, SUPER_SESSION_ID_FALG };

        virtual ~SessionAccessFactory() {}

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of an SessionAccessFactory object.
         */
        static SessionAccessFactory& getInstance();

        /**
         * getSuperSessionId
         *
         * Returns the session ID that represents the super session.
         *
         * @return std::string The session Id for the super session.
         */
        static const std::string& getSuperSessionId();

        /**
         * getSession
         *
         * Retrieves the specified Session and returns the associated data as an
         * object conforming to the ISession interface.
         *
         * @param sessionId The sessionId to the session to retrieve
         *
         * @return A pointer to an object conforming to the ISession interface.
         * N.B.: It is the responsibility of the *client* to delete the returned ISession
         * object
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException A DataException will be thrown if there is no Session
         *            matching the supplied PKEY, or if there is more than one Session
         *            matching the supplied PKEY.
         */
        ISession* getSession(const std::string& sessionId);
        ISessionPtr getSessionPtr(const std::string& sessionId);

        /**
         * getOwnedSessions
         *
         * Retrieves all unended sessions owned by the entity specified.
         *
         * @param entityKey The entity key identifying the entity.
         *
         * @return A vector of pointers to an object conforming to the ISession interface.
         * N.B.: It is the responsibility of the *client* to delete the returned ISession
         * objects
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         */
        std::vector<ISession*> getOwnedSessions(ta_uint32 entityKey);

        std::vector<ISession*> SessionAccessFactory::getOwnedActiveSessions(ta_uint32 entityKey);
        /**
         * getAllSessions
         *
         * Retrieves all sessions from the database.
         *
         * @param sessionId The sessionId to the session to retrieve
         *
         * @return A pointer to an object conforming to the ISession interface.
         * N.B.: It is the responsibility of the *client* to delete the returned ISession
         * object
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException A DataException will be thrown if there is no Session
         *            matching the supplied PKEY, or if there is more than one Session
         *            matching the supplied PKEY.
         */
        std::vector<ISession*> getAllSessions(bool readWrite);
        std::vector<ISession*> getAllLocalSessions(bool readWrite, ta_uint32 locationId);

        /**
         * createSession
         *
         * Creates an ISession object, and returns a pointer to it. It populates the database
         * with a new sessionId containing the operator and console keys specified.
         * N.B.: It is the responsibility of the *client* to delete the returned
         * ISession object
         * N.B2: The new session will be allocated a uuid on construction. To retrieve
         * the UUID simply call getSessionId after construction.
         *
         * @param ta_uint32 - The key of the operator logged in
         * @param ta_uint32 - The key of the console the operator is using.
         * @param ta_uint32 - The key of the profile the operator is using.
         *
         * @return A pointer to an ISession object
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException A DataException will be thrown if the method has trouble
         *            retrieving the next available pkey.
         */
        ISession* createSession(const ta_uint32 operatorKey,
                                const ta_uint32 consoleKey,
                                const ta_uint32 profileKey,
                                const ta_uint32 locationKey,
                                const ta_uint32 entityKey,
                                const bool displayOnly = false);

        /**
         * createSession
         *
         * Creates an ISession object, and returns a pointer to it. It populates the database
         * with a new sessionId containing the operator and console keys specified.
         * N.B.: It is the responsibility of the *client* to delete the returned
         * ISession object
         * N.B2: The new session will be allocated a uuid on construction. To retrieve
         * the UUID simply call getSessionId after construction.
         *
         * @param std::string - the sessionID for this session
         * @param ta_uint32 - The key of the operator logged in
         * @param ta_uint32 - The key of the console the operator is using.
         * @param ta_uint32 - The key of the profile the operator is using.
         *
         * @return A pointer to an ISession object
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException A DataException will be thrown if the method has trouble
         *            retrieving the next available pkey.
         */
        ISession* createSession(const std::string& sessionID,
                                const ta_uint32 operatorKey,
                                const ta_uint32 consoleKey,
                                const ta_uint32 profileKey,
                                const ta_uint32 locationKey,
                                const ta_uint32 entityKey,
                                const bool displayOnly = false);

        /**
         * createEndedSession
         *
         * Creates an ISession object, that will be used to set the HAS_ENDED column to 1.
         *
         * @param std::string - the sessionID for this session
         *
         * @return A pointer to an ISession object
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         */
        ISession* createEndedSession(const std::string& sessionID);

        /**
         * deleteSession
         *
         * This will remove a session from the database. This should only be used by the Control
         * Station. Following a call to this method the ISession passed must then be deleted by
         * the caller.
         *
         * @param ISession* A pointer to a non-NULL session to be removed from the database.
         *
         * @exception DatabaseException This exception will be thrown under the followin circumstances:
         *            1) The database has inadvertantly been closed, and cannot be reopened.
         *            2) The execution of the sql query fails.
         */
        void deleteSession(ISession* session);

        //TD18707 xinsong++
        static std::string fillLocInfoToSessionId(ta_uint32 locationKey, const std::string& uuidString);

        //if it is a superID, return superID, if error, return error_ID
        static ta_uint32 getLocIDFromSessionId(const std::string& sessionId);

        static void getSessionFlag(const std::string& sessionId, SESSION_FLAG& sessionFlag);

        static bool isSupperSessionId(const std::string& sessionId);
        //return false if it is a superId
        static bool isCurrentSessionIdFormat(const std::string& sessionId);
        static bool isObsoleteSessionIdFormat(const std::string& sessionId);
        static bool isValidSessionId(const std::string& sessionId);

        bool isSystemProfileSession(const std::string& sessionId);
        ta_uint32 getOperatorKey(const std::string& sessionId);
        bool isEndedSession(const std::string& sessionId);

    private:

        //
        // These are private as this method is a singleton
        //
        SessionAccessFactory()
        {
        };
        SessionAccessFactory(const SessionAccessFactory& theSessionAccessFactory);
        SessionAccessFactory& operator=(const SessionAccessFactory&);

        /**
         * getLocalDatabaseName
         *
         * returns the name of the local database
         */
        std::string getLocalDatabaseName();

        // The name of the local database to send updates to
        std::string m_localDatabase;

        void getSessions(IDatabase* pDatabase, const SQLStatement& sql, std::vector<ISession*>& sessions);

        static SessionAccessFactory* m_instance;
        static std::string m_transactiveSuperSessionId;
    };

} // closes TA_Base_Core

#endif // !defined(SessionAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
