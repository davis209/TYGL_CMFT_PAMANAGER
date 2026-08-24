/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source: /cvs/1TS-9999-T/code/transactive/app/radio/RadioTetraAgent/src/Attic/RadioException.h,v $
  * @author:  Adam Radics
  * @version: $Revision: 1.1.2.1 $
  *
  * Last modification: $Date: 2007/04/03 05:35:47 $
  * Last modified by:  $Author: adamr $
  *
  * Parent of all Radio Agent exceptions.
  *
  */


#if !defined(RadioException_H)
#define RadioException_H

#include "core/exceptions/src/TransactiveException.h"

namespace TA_IRS_App
{
	class RadioException : public TA_Base_Core::TransactiveException
	{

	public:

		RadioException() throw();
        RadioException(const std::string& msg) throw();

		virtual ~RadioException() throw();
	};
};

#endif // !defined(RadioException_H)
