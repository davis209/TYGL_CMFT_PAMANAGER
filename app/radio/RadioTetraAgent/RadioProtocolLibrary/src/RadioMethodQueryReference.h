/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodQueryReference.h $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class encapsulates the Radio API Change Reference method.
  *
  */

///////////////////////////////////////////////////////////
//  RadioMethodQueryReference.h
//  Implementation of the Class RadioMethodQueryReference
//  Created on:      29-Nov-2003 09:57:09 AM
///////////////////////////////////////////////////////////

#if !defined(RadioMethodQueryReference_B874D102_C2B6_4e5a_A4DC_D9D9081591EF__INCLUDED_)
#define RadioMethodQueryReference_B874D102_C2B6_4e5a_A4DC_D9D9081591EF__INCLUDED_

#include "core/types/src/ta_types.h"
#include "RadioMethod.h"
#include <vector>

namespace TA_IRS_App
{
	class RadioMethodQueryReference : public RadioMethod
	{
		public:

            enum EKeys
            {
                K_KEY_ALL                = 0,
                K_CALL_TYPE              = 1,
                K_FULL_DUPLEX            = 2,
                K_COMM_TYPE              = 3,
                K_SLOTS                  = 5,
                K_CALLING                = 6,
                K_CALLED                 = 7,
                K_AREA_SELECTION         = 8,
                K_AUDIO                  = 9,
                K_STATUS                 = 10,
                K_TIMESTAMP              = 11,
                K_TYPE                   = 12,
                K_NAME                   = 13,
                K_REGION                 = 14,
				K_ORGANISATION           = 15,
                K_MNEMONIC               = 17,
                K_TSI                    = 18,
                K_MEMBERS_COUNT          = 19,
                K_MEMBERS                = 20,
                K_SUPPLEMENTARY_SERVICES = 34,
                K_FORWARD_UNCONDITIONAL  = 35,
                K_FORWARD_UNREACHABLE    = 36,
                K_VOLUME_R               = 38,
                K_VOLUME_L               = 39,
                K_CHANNELS               = 41,
                K_DISCONNECT_REASON      = 49,
                K_CALL_ALL_OPTIONS       = 50,
                K_CALLER_PHONE_NUMBER    = 51,
                K_CALLING_AREA           = 54,
                K_CALLED_AREA            = 55,
                K_CIRCUIT_DATA_MODE      = 59,
                K_PRIORITY               = 92,
                K_CALLCONNECT_TIME       = 93,
				K_DEFAULT_CODEPAGE       = 94,      //new value based on the design??
				K_CODEPAGE               = 95,      //new value based on the design??
				K_STATE_PROVISIONING     = 244,
				K_SUBSCRIBER_STATUS      = 245,
                K_KEY_ALL_SUBSCRIBER     = 10000,
                K_KEY_ALL_DATACALL       = 10001
            };

			RadioMethodQueryReference(RadioSessionReference sessionRef);
			virtual ~RadioMethodQueryReference();

            void setQueryType(const EKeys key);
            void setQueryReference(ta_uint32 reference);

		private:
			RadioMethodQueryReference(const RadioMethodQueryReference& theRadioMethodQueryReference);
            
            static const unsigned int QUERY_REFERENCE_LENGTH;
            static const unsigned int REFERENCE_PARAMETER_POSITION;
            static const unsigned int KEY_PARAMETER_POSITION;
	};
};
#endif // !defined(RadioMethodQueryReference_B874D102_C2B6_4e5a_A4DC_D9D9081591EF__INCLUDED_)
