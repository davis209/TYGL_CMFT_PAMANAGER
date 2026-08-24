/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodJoin.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#if !defined(RadioMethodJoin_H)
#define RadioMethodJoin_H

#include "core/types/src/ta_types.h"
#include "RadioMethodCallReference.h"


namespace TA_IRS_App
{
	/**
	* Join the operator into the given active group call
	*/
	class RadioMethodJoin : public RadioMethodCallReference
	{
		public:

            /**
             * Constructor
             *
             * @param sessionRef    the usual session reference
             * @param callRef       a group CallReference
             */
			RadioMethodJoin(RadioSessionReference sessionRef, ta_uint32 callRef);

		public:
	};
};

#endif // !defined(RadioMethodJoin_H)
