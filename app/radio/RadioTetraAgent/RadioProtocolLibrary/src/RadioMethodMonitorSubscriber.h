/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodMonitorSubscriber.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#if !defined(RadioMethodMonitorSubscriber_H)
#define RadioMethodMonitorSubscriber_H

#include "core/types/src/ta_types.h"
#include "RadioMethod.h"

namespace TA_IRS_App
{
	/**
	* Monitor the given subscriber.
	*/
	class RadioMethodMonitorSubscriber : public RadioMethod
	{
		public:
            typedef enum MonitorOperation {
                MON_START       = 0,
                MON_STOP        = 1,
                MON_ONE_SHOT    = 2
            };

            /**
             * Constructor
             *
             * @param tsi Subscriber Id
             * @param operation one of the given enumeration: start, stop, one-shot
             * @param reference only used with MON_STOP, the original reference
             */
			RadioMethodMonitorSubscriber(RadioSessionReference sessionRef, const char * tsi, MonitorOperation operation, ta_int32 monitorRef =0);
			virtual ~RadioMethodMonitorSubscriber();

		public:
	};
};

#endif // !defined(RadioMethodMonitorSubscriber_H)
