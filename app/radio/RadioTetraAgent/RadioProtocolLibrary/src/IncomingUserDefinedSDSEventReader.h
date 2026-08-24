/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source: /cvs/1TS-9999-T/code/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/Attic/IncomingSDSEventReader.h,v $
  * @author:  Andy Parker
  * @version: $Revision: 1.1.2.2.38.1 $
  *
  * Last modification: $Date: 2006/08/23 08:01:57 $
  * Last modified by:  $Author: robertvh $
  *
  * This class is used to access the information associated with an incoming SDS
  * event.
  */

///////////////////////////////////////////////////////////
//  IncomingSDSEventReader.h
//  Implementation of the Class IncomingSDSEventReader
//  Created on:      27-Nov-2003 02:41:51 PM
///////////////////////////////////////////////////////////

#ifndef __INCOMING_USER_DEFINED_SDS_EVENT_READER_H_INCLUDED__
#define __INCOMING_USER_DEFINED_SDS_EVENT_READER_H_INCLUDED__

#include "RadioProtocol.h"
#include "RadioEventReader.h"
#include "IncomingSDSEventReader.h"
#include <vector>
#include <string>

/**
 * This class is used to access the information associated with an incoming SDS
 * event.
 */
namespace TA_IRS_App
{
	class IncomingUserDefinedSDSEventReader 
		: public IncomingSDSEventReader
	{
		public:
			IncomingUserDefinedSDSEventReader(RadioEvent& event);
			virtual ~IncomingUserDefinedSDSEventReader();

        protected:
            /**
             * For derived SDS EventReader classes, to provide their own EventType
             */
            IncomingUserDefinedSDSEventReader(RadioEvent& event, EventType eventType);


		public:
			// ///////////////////////////////////
			//
			// IncomingSDSEventReader overrides
			//
			// ///////////////////////////////////
			virtual bool isText() const;
			virtual bool isTrainMessage() const;
			virtual ta_uint32 getSDSReference() const;
			virtual std::string getSenderITSI() const;
			virtual std::string getDestinationITSI() const;
			virtual std::string getMessageText() const;
			virtual std::vector<unsigned char> getMessageData() const;
			virtual bool isPreCoded() const;
			virtual unsigned short getPreCodedStatus() const;
			unsigned short getPID() const;

		private:
			IncomingUserDefinedSDSEventReader(const IncomingUserDefinedSDSEventReader& theIncomingSDSEventReader);

		private:
			static const ta_uint32 SDS_REFERENCE_START_POS;
			static const ta_uint32 CALLING_NUMBER_START_POS;
			static const ta_uint32 CALLED_NUMBER_START_POS;
			
			// these following 2 values are for reference only - use m_dataStartPosition
			// and m_dataStartPosition+4 respectively, instead
			static const ta_uint32 DATA_LENGTH_START_POS;
			static const ta_uint32 DATA_START_POS;
			static const ta_uint32 PID_POS;

			ta_uint32 m_dataStartPosition;
	};
};

#endif //__INCOMING_USER_DEFINED_SDS_EVENT_READER_H_INCLUDED__

