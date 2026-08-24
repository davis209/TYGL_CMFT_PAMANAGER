/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/AttachMonitorAudioEventReader.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Implementation of MonitorSubscriber Event Reader
  *
  */


#if !defined(AttachMonitorAudioEventReader_INCLUDED_)
#define AttachMonitorAudioEventReader_INCLUDED_

#include "core/types/src/ta_types.h"
#include "RadioEventReader.h"


namespace TA_IRS_App
{
    class AttachMonitorAudioEventReader : public RadioEventReader
    {
        public:

            AttachMonitorAudioEventReader(RadioEvent & event);
            virtual ~AttachMonitorAudioEventReader();

            ta_uint32 getCallReference() const;
            ta_uint32 getAudioReferenceA() const;
            ta_uint32 getAudioReferenceB() const;

        private:
            static const unsigned int ATTACHMONITORAUDIO_EVENT_LENGTH;
            static const unsigned int CALL_REFERENCE_START;
            static const unsigned int AUDIO_REFERENCE_A_START;
            static const unsigned int AUDIO_REFERENCE_B_START;
    };
};

#endif // !defined(AttachMonitorAudioEventReader_INCLUDED_)
