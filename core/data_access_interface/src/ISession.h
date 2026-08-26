/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/ISession.h $
 * @author:  Karen Graham
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * ISession is an interface to an Session object. It allows the Session object implementation
 * to be chagned (e.g. if necessary due to a database schema change) without changing code
 * that uses it.
 */

#if !defined(ISession_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define ISession_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include "core/types/src/ta_types.h"
#include <string>
#include <boost/shared_ptr.hpp>
#include <vector>

namespace TA_Base_Core
{
    class ISession
    {
    public:

        virtual ~ISession() {};

        /**
         * getSessionId
         *
         * Returns the sessionId for this PMSESSION entry.
         *
         * @return The key (uuid) for this Session as a string.
         */
        virtual std::string getSessionId() = 0;

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
        virtual ta_uint32 getOperatorKey() = 0;

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
        virtual std::string getOperatorName() = 0;

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
        virtual ta_uint32 getConsoleKey() = 0;

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
        virtual ta_uint32 getProfileKey() = 0;

        /**
         * getLocationKey
         *
         * Returns the key for the location of this session
         * NOTE: Apparently for a "Central" profile, the location key will be 0.  Therefore
         * this method cannot be used in all cases if the physical location of the session is
         * required.
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
        virtual ta_uint32 getLocationKey() = 0;

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
        virtual bool getHasEnded() = 0;

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
        virtual void setHasEnded() = 0;

        virtual void setDisplayOnly(bool displayOnly) = 0;
        /**
         * invalidate
         *
         * Make the data contained by this Session as invalid. The next call to get...()
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate() = 0;

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
        virtual void applyChanges() = 0;
        virtual ta_uint32 getEntityKey() = 0;

        virtual bool getDisplayMode() = 0;
        virtual ta_uint32 getOverrideOperatorKey() = 0;
        virtual ta_uint32 getOverrideProfileKey() = 0;
        virtual void updateOverrideInfoToDb(ta_uint32 userOverrideId, ta_uint32 profileOverrideId) = 0;
        virtual void updateOverrideInfoToDb() = 0;
    };

    typedef boost::shared_ptr<ISession> ISessionPtr;
    typedef std::vector<ISessionPtr> ISessionPtrList;
}

#endif // !defined(ISession_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
