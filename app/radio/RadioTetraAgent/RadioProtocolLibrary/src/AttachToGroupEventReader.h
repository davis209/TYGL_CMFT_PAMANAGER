/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/AttachToGroupEventReader.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Implementation of MonitorSubscriber Event Reader
  *
  */


#if !defined(AttachToGroupEventReader_INCLUDED_)
#define AttachToGroupEventReader_INCLUDED_

#include "RadioEventReader.h"


namespace TA_IRS_App
{
    class AttachToGroupEventReader : public RadioEventReader
    {
        public:

            AttachToGroupEventReader(RadioEvent & event);

            std::string getGTSI() const;

        private:
            static const unsigned int ATTACHTOGROUP_EVENT_LENGTH;
            static const unsigned int GROUP_TSI_START;
    };
};

#endif // !defined(AttachToGroupEventReader_INCLUDED_)
