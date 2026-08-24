/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Workfile:$
  * @author:  Haipeng Jiang
  * @version: $Revision: 1.1.2.6 $
  *
  * Last modification: $Date: 2006/08/23 08:01:57 $
  * Last modified by:  $Author: robertvh $
  *
  * <description>
  */
#if !defined(INCOMINGSDSTLTRANSFEREXTENDEDEVENTREADER_H)
#define INCOMINGSDSTLTRANSFEREXTENDEDEVENTREADER_H

#include "RadioProtocol.h"
#include "IncomingSDSTLTransferEventReader.h"
#include "core/types/src/ta_types.h"

namespace TA_IRS_App
{

	class IncomingSDSTLTransferExtendedEventReader: public IncomingSDSTLTransferEventReader
	{
	public:
		IncomingSDSTLTransferExtendedEventReader(RadioEvent & radioEvent);
		virtual ~IncomingSDSTLTransferExtendedEventReader();
		
		ECodePage getCharacterEncoding(); 

	private:
		IncomingSDSTLTransferExtendedEventReader();
		IncomingSDSTLTransferExtendedEventReader& operator = (const IncomingSDSTLTransferExtendedEventReader & that);

	private:
		static const ta_uint32 CODE_PAGE_START_POS;
		static const ta_uint32 DATA_LENGTH_START_POS;
		static const ta_uint32 DATA_START_POS;
	};

}

#endif
