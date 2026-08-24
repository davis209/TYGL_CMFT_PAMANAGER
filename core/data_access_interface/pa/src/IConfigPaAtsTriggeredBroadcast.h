/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/pa/src/IConfigPaAtsTriggeredBroadcast.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * IConfigPaAtsTriggeredBroadcast - This is the interface supported by objects that allow configuration of
  * PaAtsTriggeredBroadcasts. It provides methods for retrieving, settings, and writing data for an PaAtsTriggeredBroadcast object.
  * The set methods in this interface do not set the values in the database
  * straight away. They only set the values in the object after validating that
  * they are correct.
  */

#if !defined(IConfigPaAtsTriggeredBroadcast_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)
#define IConfigPaAtsTriggeredBroadcast_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_

#include "core/types/src/ta_types.h"
#include <string>

#include "core/data_access_interface/src/IConfigItem.h"
#include "core/data_access_interface/pa/src/IPaAtsTriggeredBroadcast.h"

namespace TA_IRS_Core
{

    class IConfigPaAtsTriggeredBroadcast : public virtual TA_Base_Core::IConfigItem, public virtual IPaAtsTriggeredBroadcast
    {

    public:

        virtual ~IConfigPaAtsTriggeredBroadcast() {};


        virtual void setAtsEventType(ta_uint32 eventType) =0;
        virtual void setLocationKey(ta_uint32 locationKey) =0;
        virtual void setDestinationLocationKey(ta_uint32 key) =0;
        virtual void setPlatformId(ta_uint32 platformId) =0;
        virtual void setIsLastTrain(bool isEnabled) =0;
        virtual void setPaZoneGroupKey(ta_uint32 key) =0;
        virtual void setPaDvaMessageKey(ta_uint32 key) =0;
        virtual void setIsEnabledParameter(bool isEnabled) =0;
        virtual void setDvaMessageVersion(const std::string& version) =0;

        /**
         * getDateCreated
         *
         * Returns the date created for this PaAtsTriggeredBroadcast
         *
         * @return The date created for this PaAtsTriggeredBroadcast as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSKEY should be an 
         *             ta_uint32 ), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual time_t getDateCreated() =0;


        /**
         * getDateModified
         *
         * Returns the date modified for this PaAtsTriggeredBroadcast.
         *
         * @return The date modified for this PaAtsTriggeredBroadcast as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSKEY should be an 
         *             ta_uint32 ), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual time_t getDateModified() =0;

        
        
        /**
         * deleteThisObject
         *
         * Removes this PaAtsTriggeredBroadcast from the database. 
         * The calling application MUST then delete this PaAtsTriggeredBroadcast object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this PaAtsTriggeredBroadcast was initially loaded from the database
         *      OR     - writePaAtsTriggeredBroadcastData() has already been called
         *      This PaAtsTriggeredBroadcast has not been deleted
         */
        virtual void deleteThisObject() = 0;

    };
} //close namespace TA_IRS_Core
#endif // !defined(IConfigPaAtsTriggeredBroadcast_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)
