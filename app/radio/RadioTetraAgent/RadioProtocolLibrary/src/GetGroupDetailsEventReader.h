/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/GetGroupDetailsEventReader.h $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class encapsulates the Radio API Change Reference method.
  *
  */

///////////////////////////////////////////////////////////
//  GetGroupDetailsEventReader.h
//  Implementation of the Class GetGroupDetailsEventReader
//  Created on:      29-Nov-2003 09:57:09 AM
///////////////////////////////////////////////////////////

#if !defined(GetGroupDetailsEventReader_B874D102_C2B6_4e5a_A4DC_D9D9081591EF__INCLUDED_)
#define GetGroupDetailsEventReader_B874D102_C2B6_4e5a_A4DC_D9D9081591EF__INCLUDED_

#include "core/types/src/ta_types.h"
#include "RadioEventReader.h"

#include <vector>
#include <string>

namespace TA_IRS_App
{
    typedef std::vector< std::string > GroupDetailsRow;
    typedef std::vector< GroupDetailsRow > GroupDetailsTable;

	class GetGroupDetailsEventReader : public RadioEventReader 
	{
		public:
			GetGroupDetailsEventReader(RadioEvent& event);
			virtual ~GetGroupDetailsEventReader();

            const ta_uint32 getReference() const;
            int getDetailsTable( GroupDetailsTable & detailsTable ) const;

		private:
			GetGroupDetailsEventReader(const GetGroupDetailsEventReader& theGetGroupDetailsEventReader);
            
            static const unsigned int REFERENCE_PARAMETER_POSITION;
            static const unsigned int RESULT_TABLE_START_POSITION;
            static const unsigned int RESULT_TABLE_STRING_POSITION;
	};
};
#endif // !defined(GetGroupDetailsEventReader_B874D102_C2B6_4e5a_A4DC_D9D9081591EF__INCLUDED_)
