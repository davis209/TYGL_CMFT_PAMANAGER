 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/pa/src/PaAtsTriggeredBroadcast.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * PaAtsTriggeredBroadcast is an implementation of IPaAtsTriggeredBroadcast. It holds the data specific to an PaAtsTriggeredBroadcast entry
  * in the database, and allows read-only access to that data.
  *
  */


#if !defined(PaAtsTriggeredBroadcast_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define PaAtsTriggeredBroadcast_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

#include "core/types/src/ta_types.h"
#include <string>

#include "core/data_access_interface/pa/src/PaAtsTriggeredBroadcastHelper.h"
#include "core/data_access_interface/pa/src/IPaAtsTriggeredBroadcast.h"

namespace TA_IRS_Core
{


    class PaAtsTriggeredBroadcast : public IPaAtsTriggeredBroadcast
    {

    public:



        /**
         * PaAtsTriggeredBroadcast (constructor)
         *
         * Construct an PaAtsTriggeredBroadcast class based around a key.
         *
         * @param key The key of this PaAtsTriggeredBroadcast in the database
         */
        PaAtsTriggeredBroadcast(const ta_uint32 key);

        /**
         * PaAtsTriggeredBroadcast (constructor)
         *
         * Constructs a new instance using the information provided in IData interface
         *
         * @see PaAtsTriggeredBroadcastHelper::PaAtsTriggeredBroadcastHelper(IData*)
         *
         */
        PaAtsTriggeredBroadcast(ta_uint32 row, TA_Base_Core::IData& data);

        virtual ~PaAtsTriggeredBroadcast();

        virtual ta_uint32 getKey();
 
        virtual ta_uint32 getAtsEventType();
        virtual ta_uint32 getLocationKey();
        virtual ta_uint32 getDestinationLocationKey();
        virtual ta_uint32 getPlatformId();
        virtual bool          getIsLastTrain();
        virtual ta_uint32 getPaZoneGroupKey();
        virtual ta_uint32 getPaDvaMessageKey();
        virtual bool          getIsEnabledParameter();
        virtual std::string   getDvaMessageVersion();
        /**
         * getName
         *
         * Returns the name of this item for the IItem interface.  
         * For the PA ATS Triggered Broadcast this is 
         * just the key in a string format e.g. "PA ATS Triggered Broadcast 1".
         *
         * @return The key description for this item as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *             ta_uint32 ), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getName();

        /**
         * invalidate
         *
         * Make the data contained by this PaAtsTriggeredBroadcast as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();


    private:

        // Assignment PaAtsTriggeredBroadcast not used so it is made private
		PaAtsTriggeredBroadcast& operator=(const PaAtsTriggeredBroadcast &);
		
        // Copy constructor should not be used and has therefore been made private. If a copy
        // constructor is required care should be taken. The copy constructor fo the PaAtsTriggeredBroadcastHelper
        // has only been written for ConfigLocation objects and will not copy the PaAtsTriggeredBroadcast helper
        // for a read-only object (ie this one).
        PaAtsTriggeredBroadcast( const PaAtsTriggeredBroadcast& thePaAtsTriggeredBroadcast);  

        TA_IRS_Core::PaAtsTriggeredBroadcastHelper * m_paAtsTriggeredBroadcastHelper;
    };
} // closes TA_IRS_Core

#endif // !defined(PaAtsTriggeredBroadcast_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
