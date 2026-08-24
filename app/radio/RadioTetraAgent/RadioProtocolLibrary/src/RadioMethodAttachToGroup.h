/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodAttachToGroup.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#if !defined(RadioMethodAttachToGroup_H)
#define RadioMethodAttachToGroup_H

#include "RadioMethod.h"

namespace TA_IRS_App
{
	/**
	* Monitor the given subscriber.
	*/
	class RadioMethodAttachToGroup : public RadioMethod
	{
		public:
            /**
             * Constructor
             *
             * @param sessionRef     The radio session reference
             * @param gtsi           The tsi of the group to attach to
             */
            RadioMethodAttachToGroup(RadioSessionReference sessionRef, const char * gtsi);
	};
}

#endif // !defined(RadioMethodAttachToGroup_H)
