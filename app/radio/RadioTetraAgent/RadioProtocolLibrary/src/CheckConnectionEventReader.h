/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_TIP/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/CheckConnectionEventReader.h $
  * @author:  
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: $
  * Last modified by:  $Author: $
  *
  * Declaration of CheckConnection Event Reader
  *
  */


#if !defined(CheckConnectionEventReader_INCLUDED_)
#define CheckConnectionEventReader_INCLUDED_

#include "RadioEventReader.h"


namespace TA_IRS_App
{
    class CheckConnectionEventReader : public RadioEventReader
    {
        public:

            CheckConnectionEventReader(RadioEvent & event);
            virtual ~CheckConnectionEventReader();

        private:
            static const unsigned int CHECKCONNECTION_EVENT_LENGTH;
    };
};

#endif // !defined(CheckConnectionEventReader_INCLUDED_)
