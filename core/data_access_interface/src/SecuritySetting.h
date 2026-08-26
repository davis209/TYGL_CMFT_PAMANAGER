/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/data_access_interface/src/Region.h $
 * @author:  Karen Graham
 * @version: $Revision: #4 $
 *
 * Last modification: $DateTime: 2012/12/14 11:34:13 $
 * Last modified by:  $Author: liwei.gong $
 *
 * Region is an implementation of IRegion. It holds the data specific to an Region entry
 * in the database, and allows read-only access to that data.
 *
 */

#pragma once
#include <boost/operators.hpp>

namespace TA_Base_Core
{
    struct SecuritySetting : boost::equality_comparable<SecuritySetting>
    {
        int automaticLogoutTime;
        int failedLoginAttemtps;
        int passwordReuseMax;
        int passwordWarnTime;
        int passwordReuseTime;
        int passwordLifeTime;
        int passwordLockTime;

        bool operator == (const SecuritySetting& rhs) const
        {
            return automaticLogoutTime == rhs.automaticLogoutTime &&
                   failedLoginAttemtps == rhs.failedLoginAttemtps &&
                   passwordReuseMax == rhs.passwordReuseMax &&
                   passwordWarnTime == rhs.passwordWarnTime &&
                   passwordReuseTime == rhs.passwordReuseTime &&
                   passwordLifeTime == rhs.passwordLifeTime &&
                   passwordLockTime == rhs.passwordLockTime;
        }
    };
}
