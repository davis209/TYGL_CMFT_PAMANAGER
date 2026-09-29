/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/stis_manager/src/ITimeTypeListener.h $
 * @author:  Justin Ebedes
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 *
 * This interface is implemented by the class that needs
 * to be notified when the time type changes (cyclic or
 * continuous).
 *
 */

#if !defined(ITIMETYPELISTENER_H)
#define ITIMETYPELISTENER_H

#include "TimeControlManager.h"

namespace TA_IRS_App
{
    class ITimeTypeListener
    {
    public:

        /**
         * timeTypeChanged
         *
         * Gets called when the time type is changed.
         *
         * @param newTimeType  The new time type.
         */
        virtual void timeTypeChanged(TimeControlManager::TimeType newTimeType) = 0;
    };

} // TA_IRS_App

#endif // !defined(ITIMETYPELISTENER_H)