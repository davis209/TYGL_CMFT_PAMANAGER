/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodSendCircuitData.h $
  * @author:  Abhinav Mathur
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class encapsulates the Radio API SendCircuitData method.
  *
  */

///////////////////////////////////////////////////////////
//  RadioMethodSendCircuitData.h
//  Implementation of the Class RadioMethodSendCircuitData
//  Created on:      27-Jan-2005 09:57:09 AM
///////////////////////////////////////////////////////////

#if !defined(RadioMethodSendCircuitData__INCLUDED_)
#define RadioMethodSendCircuitData__INCLUDED_

#include "RadioMethodCallReference.h"
#include <vector>



namespace TA_IRS_App
{
	class RadioMethodSendCircuitData : public RadioMethodCallReference 
	{
        //const static unsigned int SENDCIRCUITDATA_PARAMETER_LENGTH;
        const static unsigned int SENDCIRCUITDATA_DATACALLREFERENCE_START_POS;
        const static unsigned int SENDCIRCUITDATA_FLUSH_START_POS;
        const static unsigned int SENDCIRCUITDATA_DATA_LENGTH_START_POS;
        const static unsigned int SENDCIRCUITDATA_DATA_START_POS;

	public:

        	const static unsigned int SENDCIRCUITDATA_MESSAGE_LENGTH;

			RadioMethodSendCircuitData(RadioSessionReference sessionRef, RadioCallReference callRef,
				const std::vector<unsigned char>& data);

	private:
        /// "Hidden" copy constructor
			RadioMethodSendCircuitData(const RadioMethodSendCircuitData& theRadioMethodSendCircuitData);
	};
};
#endif // !defined(RadioMethodSendCircuitData__INCLUDED_)
