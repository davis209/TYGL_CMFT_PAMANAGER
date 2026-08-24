/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/LoginEventReader.h $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class encapsulates the Radio API Login Event.
  *
  */

///////////////////////////////////////////////////////////
//  RequestAuthoriseEventReader.h
//  Implementation of the Class RequestAuthoriseEventReader
//  Created on:      29-Nov-2003 09:57:09 AM
///////////////////////////////////////////////////////////

#if !defined(LoginEventReader_INCLUDED_)
#define LoginEventReader_INCLUDED_

#include "RadioEventReader.h"
#include <vector>

namespace TA_IRS_App
{
	class LoginEventReader : public RadioEventReader 
	{
		public:
			LoginEventReader(RadioEvent& event);
			virtual ~LoginEventReader();

			const std::string getDGNA() const;

		private:

			LoginEventReader(const LoginEventReader& theReader);
            
			static const unsigned int LOGIN_EVENT_LENGTH;
            static const unsigned int DGNA_POSITION;
	};
};
#endif // !defined(LoginEventReader_INCLUDED_)
