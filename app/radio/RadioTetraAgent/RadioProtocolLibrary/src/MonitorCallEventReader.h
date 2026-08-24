/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/MonitorCallEventReader.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Implementation of MonitorCall Event Reader
  *
  */


#if !defined(MonitorCallEventReader_INCLUDED_)
#define MonitorCallEventReader_INCLUDED_

#include "core/types/src/ta_types.h"
#include "RadioEventReader.h"


namespace TA_IRS_App
{
    class MonitorCallEventReader : public RadioEventReader
    {
        public:

            MonitorCallEventReader(RadioEvent & event);
            virtual ~MonitorCallEventReader();

            ta_int32 getCallReference() const;

        private:
            static const unsigned int MONITORCALL_EVENT_LENGTH;
            static const unsigned int CALL_REFERENCE_START;
    };
};

#endif // !defined(MonitorCallEventReader_INCLUDED_)
