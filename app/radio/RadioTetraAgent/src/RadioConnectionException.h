/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source: /cvs/1TS-9999-T/code/transactive/app/radio/RadioTetraAgent/src/Attic/RadioConnectionException.h,v $
  * @author:  Adam Radics
  * @version: $Revision: 1.1.2.1 $
  *
  * Last modification: $Date: 2007/04/03 05:35:47 $
  * Last modified by:  $Author: adamr $
  *
  * Thrown when there is a connection error with radio
  *
  */



#if !defined(RadioConnectionException_H)
#define RadioConnectionException_H

#include "app/radio/RadioTetraAgent/src/RadioException.h"

namespace TA_IRS_App
{
	class RadioConnectionException : public RadioException
	{

	public:
		RadioConnectionException() throw();
		virtual ~RadioConnectionException() throw();
	};
};

#endif // !defined(RadioConnectionException_H)
