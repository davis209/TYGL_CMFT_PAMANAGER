/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/pa/src/IPaZone.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * IPaZone is an interface to a PaZone object.  It allows the PAZone object implementation
  * to be changed (e.g. if necessary due to a database schema change) without changing code
  * that uses it.
  */

#if !defined(IPaZone_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IPaZone_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include "core/types/src/ta_types.h"
#include <string>
#include <vector>
#include <list>
#include "core/data_access_interface/src/IItem.h"

namespace TA_IRS_Core
{
    class IPaZone : public virtual TA_Base_Core::IItem
    {

    public:

        virtual ta_uint32 getKey() =0;
        virtual ta_uint32 getLocationKey() =0;
        virtual ta_uint32 getId() =0;
        virtual std::string getLabel() =0;
        virtual std::string getStnEntityName() =0;
        virtual std::string getOccEntityName() =0;
        //virtual ta_uint32 getStatusEntityKey() =0;
        virtual std::list<std::string> getAssociatedEquipmentEntities() =0;     

        virtual ~IPaZone() {};
        
        /**
         * invalidate
         *
         * Make the data contained by this item as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate() =0;
    };

    typedef std::vector<IPaZone*> IPaZones;
    typedef IPaZones::iterator    IPaZonesIt;

} //close namespace TA_Core


#endif // !defined(IPaZone_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
