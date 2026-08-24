#ifndef IRADIODIRECTORYCALLBACK_H
#define IRADIODIRECTORYCALLBACK_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/IRadioDirectoryCallback.h $
  * @author  A. Parker
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2008/11/28 16:26:01 $
  * Last modified by : $Author: builder $
  *
  * This interface defines the methods used to notify events from the 
  * RadioDirectoryServant class.
  */
#include "RadioTypes.h"
#include <string>

#include "bus/radio/RadioAgentCorbaDef_4669/idl/src/IRadioDirectoryCorbaDef.h"
#include "app/radio/RadioTetraAgent/src/IRadioConnectionObserver.h"


namespace TA_Base_Core
{
    class IRASubscribers;
}

namespace TA_IRS_App
{

	class IRadioDirectoryCallback : public IRadioConnectionStatusObserver
	{
		public:

            enum SubscriberChangeType
            {
                NEW_SUBSCRIBER,
                UPDATE_SUBSCRIBER,
                DELETE_SUBSCRIBER,
                SCAN_SUBSCRIBERS
            };

			// Called when subscriber changes have occured
			virtual void onSubscriberChange(TA_Base_Core::IRASubscribers * subscriber, SubscriberChangeType changeType) = 0;
	};   // class IRadioDirectoryCallback
};  // Namespace TA_IRS_App

#endif // #ifndef IRADIODIRECTORYCALLBACK_H
