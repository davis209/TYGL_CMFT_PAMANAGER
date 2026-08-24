/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/ChangePasswordEventReader.h $
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

#if !defined(ChangePasswordEventReader_INCLUDED_)
#define ChangePasswordEventReader_INCLUDED_

#include "RadioEventReader.h"
#include <vector>

namespace TA_IRS_App
{
	class ChangePasswordEventReader : public RadioEventReader
	{
	public:
		ChangePasswordEventReader(RadioEvent& event);
		virtual ~ChangePasswordEventReader();

	
	private:

		ChangePasswordEventReader(const ChangePasswordEventReader& theReader);
		static const unsigned int LOGIN_EVENT_LENGTH;
	};
};
#endif // !defined(ChangePasswordEventReader_INCLUDED_)
