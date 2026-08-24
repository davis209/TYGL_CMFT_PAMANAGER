/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source: /cvs/1TS-9999-T/code/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/Attic/RadioMethodSendSDS.h,v $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: 1.1.2.2.38.2 $
  *
  * Last modification: $Date: 2006/02/24 01:23:30 $
  * Last modified by:  $Author: Noel R. Tajanlangit $
  *
  * This class encapsulates the Radio API SendSDS method.
  *
  */

///////////////////////////////////////////////////////////
//  RadioMethodUserDefinedSDS.h
//  Implementation of the Class RadioMethodSendSDS
//  Created on:      23-Jan-2018 02:30:00 PM
///////////////////////////////////////////////////////////

#ifndef __RADIO_METHOD_USER_DEFINED_SDS_H_INCLUDED__
#define __RADIO_METHOD_USER_DEFINED_SDS_H_INCLUDED__


#include "RadioMethodCallReference.h"
#include "core/types/src/ta_types.h"
#include <vector>

namespace TA_IRS_App
{
	class RadioMethodUserDefinedSDS : public RadioMethodCallReference 
	{
		public:
			RadioMethodUserDefinedSDS(RadioSessionReference sessionRef, RadioCallReference callRef);
			virtual ~RadioMethodUserDefinedSDS();

		public:
			// set the destination TSI
			void setCalledNumber(const std::string & number);
			
			// Set the raw message data excluding the PID
			void setMessage(std::vector<unsigned char> data);

			// Set the PID value if not set default is 70
			void setPID(const unsigned short PID);

            // Returns the most bytes a single SDS message can take
            static const ta_uint32 getMaxSdsMessageLength();// TD10791, TD14634

		private:
			RadioMethodUserDefinedSDS(const RadioMethodUserDefinedSDS& theRadioMethodUserDefinedSDS);
		
			static const ta_uint32 CALLED_NUMBER_POSITION;
			static const ta_uint32 DATA_LENGTH_POSITION;
			static const ta_uint32 PID_POSITION;
			static const ta_uint32 DATA_POSITION;
						
			static const ta_uint32 PID_LENGTH;
			static const ta_uint32 MAX_MESAGE_LENGTH;
			static const ta_uint32 PARAMETER_LENGTH;

			static const unsigned short DEFAULT_PID;

	};
};

#endif //__RADIO_METHOD_USER_DEFINED_SDS_H_INCLUDED__

