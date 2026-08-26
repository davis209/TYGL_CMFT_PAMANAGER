/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/DbConnection.h $
 * @author:  R.Stagg
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * This class will return a connection string for a given data type/action
 */

#pragma once

#include "core/data_access_interface/src/DbStatus.h"
#include "core/data_access_interface/src/DataTypeEnums.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/Condition.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/SingletonImpl.h"
#include <map>
#include <string>
#include <vector>
#include <set>

namespace TA_Base_Core
{
    class DbConnection
        : public RunParamUser,
          public SingletonImpl<DbConnection>
    {
        typedef std::pair<EDataTypes, EDataActions> DataKey;
        typedef std::pair<std::string, std::string> DataConnection; // database name, connection string
        typedef std::map<DataKey, std::vector<DataConnection> > DatabaseMap;

    public:

        /**
         * Fills in the conection string with the correct string for the given datatype
         * and action. An exception will be thrown if a string can not be found.
         * @param dataType    The type of data this connection is for.
         * @param action    the action to be preformed on this datatype
         * @param connectionString    The returned connection string goes here.
         *
         */
        void getConnectionString(EDataTypes dataType, EDataActions action, std::string& connectionString, const std::string& options = "");
        std::string getConnectionString(EDataTypes dataType, EDataActions action);

        /** [aqiu] moved from Database Factory and adapt, because DbConnection class already keep the db connection information
         * getDatabaseName
         *
         * get local database name based on connection file name
         */
        std::string getDatabaseName(/*const std::string&,*/ const EDataTypes dataType, const EDataActions action);

        DbConnection();
        virtual ~DbConnection() {}

    protected:

        virtual void onRunParamChange(const std::string& name, const std::string& value);
        DatabaseMap& getDbMap();

    protected:

        DatabaseMap m_dbMap;
        ReEntrantThreadLockable m_lock;
        Condition m_condition;
        std::set<std::string> m_dbNameRunParams;

        std::string m_dbConnectionFileName;
    };
}
