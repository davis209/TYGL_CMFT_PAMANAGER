/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/GroupCallAckEventReader.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Implementation of SubscriberActivity Event Reader
  *
  */


#if !defined(GroupCallAckEventReader_INCLUDED_)
#define GroupCallAckEventReader_INCLUDED_

#include "core/types/src/ta_types.h"
#include "app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioEventReader.h"
#include "app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioProtocol.h"
#include <vector>
#include <string>


namespace TA_IRS_App
{
    typedef std::vector< std::string > TSIVector;

    class GroupCallAckEventReader : public RadioEventReader
    {
        public:
            GroupCallAckEventReader(RadioEvent & event);
            virtual ~GroupCallAckEventReader();

            CallReference getCallReference() const;
            ta_uint32 getNumberOfOperators() const;
            int getTsiVector(TSIVector & tsiVector) const;

        private:
            static const unsigned int CALL_REFERENCE_POSITION;
            static const unsigned int NUMBER_OF_OPERATORS_POSITION;
            static const unsigned int TSI_VECTOR_STRLEN_POSITION;
            static const unsigned int TSI_VECTOR_STRING_POSITION;
    };
}; // namespace TA_IRS_App

#endif // !defined(GroupCallAckEventReader_INCLUDED_)
