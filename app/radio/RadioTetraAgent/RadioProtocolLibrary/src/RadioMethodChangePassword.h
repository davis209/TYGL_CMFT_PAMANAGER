/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodChangePassword.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#if !defined(RadioMethodChangePassword_INCLUDED_)
#define RadioMethodChangePassword_INCLUDED_

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
	class RadioMethodChangePassword : public RadioMethod
	{
	public:
		RadioMethodChangePassword(ta_uint32 sessionRef, std::string oldPassword, std::string newPassword);
		virtual ~RadioMethodChangePassword();

	public:
	};
};

#endif // !defined(RadioMethodChangePassword_INCLUDED_)
