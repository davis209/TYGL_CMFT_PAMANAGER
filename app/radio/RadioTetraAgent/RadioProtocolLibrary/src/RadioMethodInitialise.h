/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodInitialise.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#if !defined(RadioMethodInitialise_3912F287_8824_42e9_A012_6E6773A17131__INCLUDED_)
#define RadioMethodInitialise_3912F287_8824_42e9_A012_6E6773A17131__INCLUDED_

#include "core/types/src/ta_types.h"
#include "RadioMethod.h"

namespace TA_IRS_App
{
	/**
	* Initialises a session ready for use. The initialise method should be called
	* after  AttachSession event and before any tetra based methods are dispatched.
	* An audio link, in numeric format , has to be specified to enable voice calls.
	* For each session, a different audio link must be specified. By using 0xffffffff
	* audio link no audio resources will be allocated for current session (e.g.
	* QueryReference with K_MAX_CHANNELS key will return 0).
	* @generator Enterprise Architect
	* @created 07-Nov-2003 11:04:01
	* @version 1.0
	* @updated 07-Nov-2003 13:19:20
	*/
	class RadioMethodInitialise : public RadioMethod
	{
		public:
			RadioMethodInitialise(ta_uint32 sessionRef);
			virtual ~RadioMethodInitialise();

		public:
			/**
			* Constructor
			* @param    sessionRef
			* @param    linkId    0 - no audio to be used
			* 1..n audio channel to be used.
			*/
			void setLinkId(ta_uint32 linkId);
	};
};

#endif // !defined(RadioMethodInitialise_3912F287_8824_42e9_A012_6E6773A17131__INCLUDED_)
