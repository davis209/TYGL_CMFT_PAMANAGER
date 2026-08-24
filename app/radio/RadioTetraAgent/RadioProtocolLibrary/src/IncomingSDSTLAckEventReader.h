/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Workfile:$
  * @author:  Haipeng Jiang
  * @version: $Revision: 1.1.2.4 $
  *
  * Last modification: $Date: 2006/07/24 07:40:38 $
  * Last modified by:  $Author: robertvh $
  *
  * <description>
  */
#if !defined(INCOMINGSDSTLACKEVENTREADER_H)
#define INCOMINGSDSTLACKEVENTREADER_H


#include "RadioProtocol.h"
#include "RadioEventReader.h"
#include "core/types/src/ta_types.h"
#include <string>
#include <vector>


namespace TA_IRS_App
{
	class IncomingSDSTLAckEventReader: public RadioEventReader
	{
	public: 
		IncomingSDSTLAckEventReader(RadioEvent &radioEvent);
		virtual ~IncomingSDSTLAckEventReader();
		
		
		/**
		* <description>
		* API incorrectly uses Delivery_Status, 
		* @return bool 
		* 
		* @exception <exceptions> Optional
		*/
		bool getAckReceivedFlag();
		
		/**
		* <description>
		* 
		* @return bool 
		* 
		* @exception <exceptions> Optional
		*/
		bool getAckConsumedFlag();  
	private:
		IncomingSDSTLAckEventReader();
		IncomingSDSTLAckEventReader& operator = (const IncomingSDSTLAckEventReader& that);

	private:
		static const ta_uint32 DELIVERY_STATUS_START_POS;
		static const ta_uint32 EVENT_INCOMING_SDSTL_PARAMETER_LENGTH;
	};

}

#endif
