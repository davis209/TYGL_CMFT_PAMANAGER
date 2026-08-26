/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/MmsRequestSchematicData.cpp $
 * @author:  Luo HuiRong
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * It holds the data specific to a MmsRequestScheduledData
 * entry in the database.
 */

#ifdef _MSC_VER
    #pragma warning(disable:4786)
#endif // _MSC_VER

#include "core/types/src/ta_types.h"
#include "MmsRequestSchematicData.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
    MmsRequestSchematicData::MmsRequestSchematicData(ta_uint32 key,
                                                     ta_uint32 entityKey,
                                                     const std::string& equipmentID,
                                                     const std::string& requestor,
                                                     const std::string& problemReport,
                                                     const timeb targetDate,
													 const timeb createDate):
        m_key(key), m_entityKey(entityKey), m_equipmentID(equipmentID.c_str()),
        m_requestor(requestor.c_str()),
        m_problemReport(problemReport.c_str()), m_targetDate(targetDate), m_createDate(createDate)
    {
        m_targetDate.dstflag = -1;
        m_targetDate.timezone = -1;

		m_createDate.dstflag = -1;
		m_createDate.timezone = -1;
    }

    MmsRequestSchematicData::~MmsRequestSchematicData()
    {
    }

    ta_uint32 MmsRequestSchematicData::getKey()
    {
        return m_key;
    }

    ta_uint32   MmsRequestSchematicData::getEntityKey()
    {
        return m_entityKey;
    }

    std::string MmsRequestSchematicData::getEquipmentID()
    {
        return m_equipmentID;
    }

    std::string MmsRequestSchematicData::getRequestor()
    {
        return m_requestor;
    }

    std::string MmsRequestSchematicData::getProblemReport()
    {
        return m_problemReport;
    }

    timeb MmsRequestSchematicData::getTargetDate()
    {
        return m_targetDate;
    }

	timeb MmsRequestSchematicData::getCreateDate()
	{
		return m_createDate;
	}

} // closes TA_Base_Core
