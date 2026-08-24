/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/IncomingSDSEventReader.h $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class is used to access the information associated with an incoming SDS
  * event.
  */

///////////////////////////////////////////////////////////
//  IncomingSDSEventReader.h
//  Implementation of the Class IncomingSDSEventReader
//  Created on:      27-Nov-2003 02:41:51 PM
///////////////////////////////////////////////////////////

#if !defined(IncomingSDSEventReader_8CC826EB_BD9E_4600_8B07_44D404026C4C__INCLUDED_)
#define IncomingSDSEventReader_8CC826EB_BD9E_4600_8B07_44D404026C4C__INCLUDED_

#include "core/types/src/ta_types.h"
#include "RadioEventReader.h"
#include <vector>
#include <string>

/**
 * This class is used to access the information associated with an incoming SDS
 * event.
 */
namespace TA_IRS_App
{
	class IncomingSDSEventReader : public RadioEventReader 
	{
		public:
			IncomingSDSEventReader(RadioEvent& event);
			virtual ~IncomingSDSEventReader();

        protected:
            /**
             * For derived SDS EventReader classes, to provide their own EventType
             */
            IncomingSDSEventReader(RadioEvent& event, EventType eventType);


		public:
            /**
             *
             */
			virtual bool isText() const;

            /**
             *
             */
			virtual bool isTrainMessage() const;

            /**
             *
             */
			virtual ta_uint32 getSDSReference() const;

            /**
             *
             */
			virtual std::string getSenderITSI() const;

            /**
             *
             */
			virtual std::string getDestinationITSI() const;

            /**
             *
             */
			virtual std::string getMessageText() const;

            /**
             *
             */
			virtual std::vector<unsigned char> getMessageData() const;


            /**
             * pre-coded sds is non-TL but these methods need to be virtual
             */
			virtual bool isPreCoded() const;

            /**
             *
             */
			virtual unsigned short getPreCodedStatus() const;

		private:
			IncomingSDSEventReader(const IncomingSDSEventReader& theIncomingSDSEventReader);
	};
};
#endif // !defined(IncomingSDSEventReader_8CC826EB_BD9E_4600_8B07_44D404026C4C__INCLUDED_)
