/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/pa/src/IPaDvaMessageVersion.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * IPaDvaMessageVersion is an interface to a PaDvaMessageVersion object.  It allows the PaDvaMessageVersion object implementation
  * to be changed (e.g. if necessary due to a database schema change) without changing code
  * that uses it.
  *
  */

#if !defined(IPaDvaMessageVersion_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IPaDvaMessageVersion_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include "core/types/src/ta_types.h"
#include <string>
#include <vector>
#include "core/data_access_interface/src/IItem.h"

namespace TA_IRS_Core
{
    class IPaDvaMessageVersion;

    class IPaDvaMessageVersion : public virtual TA_Base_Core::IItem
    {
    public:
        enum EVersionColumn
        {
            PUBLIC_ADHOC1,
            PUBLIC_ADHOC2,
            PUBLIC_ADHOC3,
            PUBLIC_ADHOC4,
            PUBLIC_ADHOC5,
            PUBLIC_ADHOC6,
            PUBLIC_PRERECORDED,
            PRIVATE_ADHOC1,
            PRIVATE_ADHOC2,
            PRIVATE_ADHOC3,
            PRIVATE_ADHOC4,
            PRIVATE_ADHOC5,
            PRIVATE_ADHOC6,
            PRIVATE_PRERECORDED,

            VERSION_COLUMN_MAX
        };

        enum ELocation
        {
            DEPOT,
            OCC,
            STATION
        };

        virtual ta_uint32 getKey() =0;
        virtual ta_uint32 getLocationKey() =0;
        virtual std::string getMessageVersion(IPaDvaMessageVersion::ELocation location, ta_uint32 stationDvaMessageId) = 0;
        virtual std::string getVersionData(EVersionColumn column) = 0;

        virtual ~IPaDvaMessageVersion() {};
        
        /**
         * invalidate
         *
         * Make the data contained by this item as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate() =0;
    };

    typedef std::vector<IPaDvaMessageVersion*> IPaDvaMessageVersions;
    typedef IPaDvaMessageVersions::iterator    IPaDvaMessageVersionsIt;

} //close namespace TA_IRS_Core


#endif // !defined(IPaDvaMessageVersion_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
