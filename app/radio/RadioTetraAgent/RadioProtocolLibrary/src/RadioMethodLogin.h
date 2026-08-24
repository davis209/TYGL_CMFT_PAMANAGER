/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodLogin.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#if !defined(RadioMethodLogin_INCLUDED_)
#define RadioMethodLogin_INCLUDED_

#include "core/types/src/ta_types.h"
#include "RadioMethod.h"

namespace TA_IRS_App
{
	/**
	* Login the user to the radio session.
	* @generator Enterprise Architect
	* @created 07-Nov-2003 11:04:01
	* @version 1.0
	* @updated 07-Nov-2003 13:19:20
	*/
	class RadioMethodLogin : public RadioMethod
	{
		public:
			RadioMethodLogin(ta_uint32 sessionRef,std::string username,std::string password);
			virtual ~RadioMethodLogin();

		public:
	};
};

#endif // !defined(RadioMethodLogin_INCLUDED_)
