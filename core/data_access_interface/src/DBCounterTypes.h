/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/DBCounterTypes.h $
 * @author:  Ripple
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * <description>
 *
 */
#ifndef DBCOUNTER_TYPES_H
#define DBCOUNTER_TYPES_H

#include "core/types/src/ta_types.h"
namespace TA_Base_Core
{
    static const ta_int32 MAX_COUNTERVAL = 999999999;

    enum DBCounterTypes
    {
        INCIDENT_COUNTTYPE                   =  1
    };

}  // closes TA_Base_Core

#endif
