/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/pa/src/IPaAtsTriggeredBroadcast.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * IPaAtsTriggeredBroadcast is an interface to a PaAtsTriggeredBroadcast object.  It allows the PaAtsTriggeredBroadcast object implementation
  * to be changed (e.g. if necessary due to a database schema change) without changing code
  * that uses it.
  *
  */

#if !defined(IPaAtsTriggeredBroadcast_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IPaAtsTriggeredBroadcast_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include "core/types/src/ta_types.h"
#include <string>
#include <vector>
#include "core/data_access_interface/src/IItem.h"

namespace TA_IRS_Core
{
    class IPaAtsTriggeredBroadcast;

    class IPaAtsTriggeredBroadcast : public virtual TA_Base_Core::IItem
    {

    public:

        virtual ta_uint32 getKey() =0;
        virtual ta_uint32 getAtsEventType() =0;
        virtual ta_uint32 getLocationKey() =0;
        virtual ta_uint32 getDestinationLocationKey() =0;
        virtual ta_uint32 getPlatformId() =0;
        virtual bool          getIsLastTrain() =0;
        virtual ta_uint32 getPaZoneGroupKey() =0;
        virtual ta_uint32 getPaDvaMessageKey() =0;
        virtual bool          getIsEnabledParameter() =0;
        virtual std::string   getDvaMessageVersion() =0;

        virtual ~IPaAtsTriggeredBroadcast() {};
        
        /**
         * invalidate
         *
         * Make the data contained by this item as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate() =0;
    };

    typedef std::vector<IPaAtsTriggeredBroadcast*> IPaAtsTriggeredBroadcasts;
    typedef IPaAtsTriggeredBroadcasts::iterator    IPaAtsTriggeredBroadcastsIt;

} //close namespace TA_IRS_Core


#endif // !defined(IPaAtsTriggeredBroadcast_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
