/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_TIP/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/DetachSessionEventReader.h $
  * @author:  
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: $
  * Last modified by:  $Author: $
  *
  * Declaration of DetachSession Event Reader
  *
  */


#if !defined(DetachSessionEventReader_INCLUDED_)
#define DetachSessionEventReader_INCLUDED_

#include "app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioEventReader.h"
#include "app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioProtocol.h"

namespace TA_IRS_App
{
    class DetachSessionEventReader : public RadioEventReader
    {
        public:

            DetachSessionEventReader(RadioEvent & event);
            virtual ~DetachSessionEventReader();

        private:
            static const unsigned int DETACHSESSION_EVENT_LENGTH;
    };
};

#endif // !defined(DetachSessionEventReader_INCLUDED_)
