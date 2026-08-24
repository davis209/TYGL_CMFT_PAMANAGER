#ifndef IRADIOSESSIONSERVANT_H
#define IRADIOSESSIONSERVANT_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/IRadioSessionServant.h $
  * @author  A. Parker
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2008/11/28 16:26:01 $
  * Last modified by : $Author: builder $
  *
  * This interface defines the methods that are available on the RadioSessionServant
  * to the RadioSession class.
  */
#include "core/types/src/ta_types.h"
#include "RadioTypes.h"
#include <string>

namespace TA_IRS_App
{
	class IRadioSessionServant
	{
		public:

			// Called to notify that the session cannot be initialised.
			virtual void onSessionInitialisationFailure() = 0;

			// Called to notify that the session has been initialised.
			virtual void onSessionInitialisationComplete() = 0;

			// Called to notify that the session cannot be logged in.
			virtual void onSessionLoginFailure(const std::string loginName) = 0;

			// Called to notify that the session has been loggen in to the radio.
			virtual void onSessionLoginComplete() = 0;

			virtual std::string getCurrentSession() = 0;

			// Called when the authorisation stack is updated
			virtual void onAuthorisationChange(const TA_IRS_App::EAction action,const TA_IRS_App::AuthorisationRequestType& details) = 0;
			virtual ta_uint32 getEntityKey() = 0;
			virtual ta_uint32 getEntityLocationKey() = 0;

			// Called to notify that the session id has been changed.
			virtual void onSessionUpdate() = 0;
	};   // class IRadioCallback
};  // Namespace TA_IRS_App

#endif
