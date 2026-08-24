/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_TIP/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/KeepAliveEventReader.h $
  * @author:  
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: $
  * Last modified by:  $Author: $
  *
  * Declaration of KeepAlive Event Reader
  *
  */


#if !defined(KeepAliveEventReader_INCLUDED_)
#define KeepAliveEventReader_INCLUDED_

#include "core/types/src/ta_types.h"
#include "app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioEventReader.h"

namespace TA_IRS_App
{
    class KeepAliveEventReader : public RadioEventReader
    {
        public:

            KeepAliveEventReader(RadioEvent & event);
            virtual ~KeepAliveEventReader();

			ta_uint32 getEventCounter();

        private:
            static const unsigned int KEEPALIVE_EVENT_LENGTH;
    };
};

#endif // !defined(KeepAliveEventReader_INCLUDED_)
