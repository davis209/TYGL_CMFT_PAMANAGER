/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/DatabaseKey.h $
 * @author:  Jade Lee
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * DatabaseKey is small helper class to unify definition of
 * invalid and valid database primary keys (some disrepancies
 * as to whether they should be 0 or -1 to indicate invalid)
 *
 */

#ifndef _DatabaseKey_HEADER_
#define _DatabaseKey_HEADER_

#include "core/types/src/ta_types.h"
namespace TA_Base_Core
{
    class DatabaseKey
    {
    public:

        /**
         * getInvalidKey
         *
         * @return a reserved key value that is invalid (ie assumed not to be
         *          used in a database as a primary key)
         *
         */
        static ta_uint32 getInvalidKey() { return -1; }

        /**
         * isInvalidKey
         *
         * @param pKey the key value to test
         *
         * @return true if the key is invalid (meaning shouldn't be used in database),
         *          which means pKey == getInvalidKey()
         *
         */
        static bool isInvalidKey(ta_uint32 pKey) { return getInvalidKey() == pKey; }

        /**
         * isInvalidKey
         *
         * @param pKey the key value to test
         *
         * @return true if the key is valid
         *
         */
        static bool isValidKey(ta_uint32 pKey) { return !isInvalidKey(pKey); }
    };
}

#endif // _DatabaseKey_HEADER_
