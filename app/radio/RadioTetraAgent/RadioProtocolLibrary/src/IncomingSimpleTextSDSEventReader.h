/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Workfile:$
  * @author:  Haipeng Jiang
  * @version: $Revision: 1.1.2.6 $
  *
  * Last modification: $Date: 2006/09/13 08:08:55 $
  * Last modified by:  $Author: robertvh $
  *
  * <description>
  */
#if !defined(INCOMINGSIMPLETEXTSDSEVENTREADER_H)
#define INCOMINGSIMPLETEXTSDSEVENTREADER_H


#include "RadioProtocol.h"
#include "IncomingSDSEventReader.h"

#include <string>
#include <vector>



namespace TA_IRS_App
{
	class IncomingSimpleTextSDSEventReader: public IncomingSDSEventReader
	{
	public: 
		IncomingSimpleTextSDSEventReader(RadioEvent &radioEvent);
		virtual ~IncomingSimpleTextSDSEventReader();
		
		std::string getCallingNumber() const;
		std::string getCalledNumber() const; 

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
	private:
		IncomingSimpleTextSDSEventReader();
		IncomingSimpleTextSDSEventReader& operator = (const IncomingSimpleTextSDSEventReader& that);

	private:
		static const ta_uint32 REFERENCE_START_POS;
		static const ta_uint32 CALLING_NUMBER_START_POS;
		static const ta_uint32 CALLED_NUMBER_START_POS;
		static const ta_uint32 DATA_LENGTH_START_POS;
		static const ta_uint32 DATA_START_POS;
		static const ta_uint32 MAX_SDS_MESAGE_LENGTH; // TD10791
	};
}

#endif
