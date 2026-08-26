/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/DbConnectionStringsImpl.h $
 * @author:  R.Stagg
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * This class reads and parses a csv file to get the connection
 * strings for all known data types
 */

#pragma once
#include "DbConnectionStrings.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/utilities/src/SingletonImpl.h"

namespace TA_Base_Core
{
    class DbConnectionStringsImpl
        : public RunParamUser,
          public SingletonImpl<DbConnectionStringsImpl>
    {
    public:

        DbConnectionStrings& getDbConnectionStrings(const std::string& filepath);
        DbConnectionStringsImpl();

    protected:

        DbConnectionStrings& parseFile(const std::string& filepath, DbConnectionStrings&);
        DbConnectionStrings& parseFileString(const std::string& str, DbConnectionStrings&);
        virtual void onRunParamChange(const std::string& name, const std::string& value) override;

    protected:

        ReEntrantThreadLockable m_lock;
        std::map<std::string, DbConnectionStrings> m_dbconnectionstrings;
    };
}
