/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/CallGenerator.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
// CallGenerator.h: interface for the CallGenerator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CALLGENERATOR_H__931BF936_1645_488D_A443_9402476EE77B__INCLUDED_)
#define AFX_CALLGENERATOR_H__931BF936_1645_488D_A443_9402476EE77B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/threads/src/Thread.h"
#include "RadioTypes.h"

namespace TA_IRS_App
{
	class Radio;

	class CallGenerator : public TA_Base_Core::Thread  
	{
		public:

			CallGenerator(Radio& radio);
			virtual ~CallGenerator();

			virtual void run();
			virtual void terminate();

		private:

			bool CreateNewCall(CallDetailsType& callDetails);

			Radio&   m_radio;
			bool     m_bGenerate;
	};
};  // Namespace TA_IRS_App

#endif // !defined(AFX_CALLGENERATOR_H__931BF936_1645_488D_A443_9402476EE77B__INCLUDED_)
