
#if !defined(SDSTLTRANSFEREVENTREADER_H)
#define SDSTLTRANSFEREVENTREADER_H


#include "RadioProtocol.h"
#include "RadioEventReader.h"


#include <string>
#include <vector>

namespace TA_IRS_App
{
	class SDSTLTransferEventReader: public RadioEventReader
	{
	public:
		SDSTLTransferEventReader(RadioEvent & event);

	private:
		static const ta_uint32 EVENT_SDSTL_TRANSFER_PARAMETER_LENGTH;
	private:
		SDSTLTransferEventReader();
		SDSTLTransferEventReader& operator = (SDSTLTransferEventReader & that);
	};
}
#endif