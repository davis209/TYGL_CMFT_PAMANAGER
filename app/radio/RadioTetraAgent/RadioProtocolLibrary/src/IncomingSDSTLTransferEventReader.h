/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Workfile:$
  * @author:  Haipeng Jiang
  * @version: $Revision: 1.1.2.7 $
  *
  * Last modification: $Date: 2006/09/13 08:08:55 $
  * Last modified by:  $Author: robertvh $
  *
  * <description>
  */
#if !defined(INCOMINGSDSTLTRANSFEREVENTREADER_H)
#define INCOMINGSDSTLTRANSFEREVENTREADER_H

#include "RadioProtocol.h"
#include "RadioEventReader.h"
#include "IncomingSDSEventReader.h"
#include "core/types/src/ta_types.h"
#include <string>
#include <vector>

#include <time.h>

namespace TA_IRS_App
{
	class IncomingSDSTLTransferEventReader: public IncomingSDSEventReader
	{
	public:
		IncomingSDSTLTransferEventReader(RadioEvent & radioEvent);
		virtual ~IncomingSDSTLTransferEventReader();
		
		std::string getCallingNumber() const;
		std::string getCalledNumber() const;
		ESDSTLProtocol getProtocol() const;

		/**
		  * <description>
			Consolidate the separate API fields (month, day, hour, minute) into a more common and reusable timestamp object supported by transactive.
		
			The Year is implicitly the same as current year, unless the current month is Jan but received day/month is 31/Dec, then the Year is the previous year.
			
		  * @return std::string
		  *
		  * @exception <exceptions> Optional
		  */
		tm getTimestamp() const;
		time_t getTimestamp_t() const;
		bool getReceiveRecieptFlag() const;
		bool getComsumedRecieptFlag() const;

		/**
		  * <description>
		  * Note: the API document incorrectly calls this "short_mode", it is really "report_type"
		  * @return ESDSTLReportType
		  *
		  * @exception <exceptions> Optional
		  */
		ESDSTLReportType getReportType() const;
		std::string getMessage() const;
		
		
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


    protected:
        /**
         * Used by IncomingSDSTLTransferExtendedEventReader
         * @param radioEvent - the usual event that this reader was created for
         * @param eventType - the type of the event, usually EVENT_INCOMING_TLSDS_TRANSFER_EX
         * @param dataStartPos - the position of the Data LONGSTRING in the
         *        Parameters. Should be the start of the DWORD that represents
         *        the length, and should be relative to the parameters, not the
         *        entire message.
         */
		IncomingSDSTLTransferEventReader(RadioEvent & radioEvent, EventType eventType, ta_uint32 dataStartPos);

	private:
        /**
         * "Hidden" copy constructor and assignment operator
         */
		IncomingSDSTLTransferEventReader(const IncomingSDSTLTransferEventReader & original);
		IncomingSDSTLTransferEventReader & operator=(const IncomingSDSTLTransferEventReader & rhs);

    protected:
		static const ta_uint32 SDS_REFERENCE_START_POS;
		static const ta_uint32 CALLING_NUMBER_START_POS;
		static const ta_uint32 CALLED_NUMBER_START_POS;
		static const ta_uint32 PROTOCOL_START_POS;
		static const ta_uint32 TIMESTAMP_MONTH_START_POS;
		static const ta_uint32 TIMESTAMP_DAY_START_POS;
		static const ta_uint32 TIMESTAMP_HOUR_START_POS;
		static const ta_uint32 TIMESTAMP_MINUTE_START_POS;
		static const ta_uint32 RESPONSE_REQUESTED_START_POS;
		static const ta_uint32 SHORT_MODE_START_POS;

        // these following 2 values are for reference only - use m_dataStartPosition
        // and m_dataStartPosition+4 respectively, instead
		static const ta_uint32 DATA_LENGTH_START_POS;
		static const ta_uint32 DATA_START_POS;

        ///
        /// the start of the Data LONGSTRING. Is actually the start of the DWORD
        /// that specifies the length, relative to the Parameter list (not the 
        /// entire message)
		ta_uint32 m_dataStartPosition;
	};
}

#endif
