/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodSearchSubscribers.h $
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
//  RadioMethodSearchSubscribers.h
//  Implementation of the Class RadioMethodSearchSubscribers
//  Created on:      29-Nov-2003 09:57:09 AM
///////////////////////////////////////////////////////////

#if !defined(RadioMethodSearchSubscribers_B874D102_C2B6_4e5a_A4DC_D9D9081591EF__INCLUDED_)
#define RadioMethodSearchSubscribers_B874D102_C2B6_4e5a_A4DC_D9D9081591EF__INCLUDED_

#include "RadioMethod.h"
#include <vector>

namespace TA_IRS_App
{
	class RadioMethodSearchSubscribers : public RadioMethod
	{
		public:

            enum EView
            {
                NORMAL                      = 0,
                K_MEMBERS                   = 1,
                K_PARENTS                   = 2,
                K_AUTHORIZED                = 3,
                K_DESPATCHER                = 4,
                K_PATCH                     = 5,
                K_PATCH_MEMBERS             = 6,
                K_PATCH_OWNER_DISPATCHER    = 7,
                K_PATCH_PARENTS             = 8,
                K_PATCH_CREATOR             = 9,
                K_DEFINED_LA                = 10,
                K_ORGANIZATION              = 11,
                K_GROUPS_MEMBERS            = 12,
                K_GROUPS_PARENTS            = 13
            };

			RadioMethodSearchSubscribers(RadioSessionReference sessionRef);
			virtual ~RadioMethodSearchSubscribers();

            void setSearchType(const EView view);
            void setCriteria(const std::string& criteria);

            /** 
              * getSerialisationLevel
              *
              * This gets the level of serialisation that the agent must be set to
              * for this method to be serialised.
              *
              * @return a level (1)
              */
            virtual unsigned char getSerialiseTransactionLevel() const;

		private:
			RadioMethodSearchSubscribers(const RadioMethodSearchSubscribers& theRadioMethodSearchSubscribers);
            
            static const unsigned int SEARCH_SUBSCRIBERS_BASE_LENGTH;
            static const unsigned int VIEW_PARAMETER_POSITION;
            static const unsigned int CRITEREA_PARAMETER_START_POSITION;
	};
};
#endif // !defined(RadioMethodSearchSubscribers_B874D102_C2B6_4e5a_A4DC_D9D9081591EF__INCLUDED_)
