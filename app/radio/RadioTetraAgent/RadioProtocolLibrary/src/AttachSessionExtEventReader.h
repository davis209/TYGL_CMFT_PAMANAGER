/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source: /cvs/1TS-9999-T/code/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/Attic/AttachSessionExtEventReader.h,v $
  * @author:  Andy Parker
  * @version: $Revision: 1.1.2.2 $
  *
  * Last modification: $Date: 2005/01/24 02:39:19 $
  * Last modified by:  $Author: darrens $
  *
  * This class encapsulates the Radio API Login Event.
  *
  */

///////////////////////////////////////////////////////////
//  AttachSessionExtEventReader.h
//  Implementation of the Class AttachSessionExtEventReader
//  Created on:      29-Nov-2003 09:57:09 AM
///////////////////////////////////////////////////////////

#if !defined(AttachSessionExtEventReader_INCLUDED_)
#define AttachSessionExtEventReader_INCLUDED_

#include "RadioEventReader.h"
#include <vector>

namespace TA_IRS_App
{
	class AttachSessionExtEventReader : public RadioEventReader 
	{
		public:
			AttachSessionExtEventReader(RadioEvent& event);
			virtual ~AttachSessionExtEventReader();

		private:
			AttachSessionExtEventReader(const AttachSessionExtEventReader& theReader);
            
	};
};
#endif // !defined(LoginEventReader_INCLUDED_)
