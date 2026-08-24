/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source: /cvs/1TS-9999-T/code/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/Attic/RadioMethodSimpleTextSDS.h,v $
  * @author:  Robert van Hugten
  * @version: $Revision: 1.1.2.2 $
  *
  * Last modification: $Date: 2006/03/17 11:04:39 $
  * Last modified by:  $Author: haipeng $
  *
  * This class encapsulates the Radio API SendSDS method.
  *
  */

///////////////////////////////////////////////////////////
//  RadioMethodSimpleTextSDS.h
//  Implementation of the Class RadioMethodSimpleTextSDS
//  Created on:      29-Nov-2003 09:57:09 AM
///////////////////////////////////////////////////////////

#if !defined(RadioMethodSimpleTextSDS_B874D102_C2B6_4e5a_A4DC_D9D9081591EF__INCLUDED_)
#define RadioMethodSimpleTextSDS_B874D102_C2B6_4e5a_A4DC_D9D9081591EF__INCLUDED_

#include "RadioMethodCallReference.h"
#include "core/types/src/ta_types.h"
#include <vector>

namespace TA_IRS_App
{
	class RadioMethodSimpleTextSDS : public RadioMethodCallReference 
	{
		public:
			RadioMethodSimpleTextSDS(RadioSessionReference sessionRef, RadioCallReference callRef);
			virtual ~RadioMethodSimpleTextSDS();

		public:
			void setCalledNumber(std::string destination);
			void setMessage(std::vector<unsigned char> data);

            // Returns the most bytes a single SDS message can take
            static const ta_uint32 getMaxSdsMessageLength();// TD10791, TD14634

		private:
			RadioMethodSimpleTextSDS(const RadioMethodSimpleTextSDS& theRadioMethodSimpleTextSDS);

			static const ta_uint32 SIMPLE_SDS_DESTINATION_START;     
			static const ta_uint32 SIMPLE_SDS_DATA_TYPE_START;       
			static const ta_uint32 SIMPLE_SDS_DATA_LENGTH_START;     
			static const ta_uint32 SIMPLE_SDS_DATA_START;            
			static const ta_uint32 SIMPLE_SDS_LENGTH;
            static const ta_uint32 MAX_SDS_MESAGE_LENGTH; // TD10791
	};
};
#endif // !defined(RadioMethodSimpleTextSDS_B874D102_C2B6_4e5a_A4DC_D9D9081591EF__INCLUDED_)
