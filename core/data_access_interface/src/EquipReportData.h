/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/EquipReportData.h $
 * @author:  Ripple
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * <description>
 *
 */

#if !defined(AFX_EQUIPREPORTDATA_H__30E577E7_FB1A_4A34_9101_5CBBEB7EC343__INCLUDED_)
#define AFX_EQUIPREPORTDATA_H__30E577E7_FB1A_4A34_9101_5CBBEB7EC343__INCLUDED_

#if _MSC_VER > 1000
    #pragma once
#endif // _MSC_VER > 1000

#include "core/types/src/ta_types.h"
#include <string>
#include <ctime>

#include "core/data_access_interface/src/IEquipReportData.h"

namespace TA_Base_Core
{
    class IData;
    class EquipReportDataHelper;

    class EquipReportData : public IEquipReportData
    {
    public:

        ta_uint32 getKey();
        std::string   getId();
        std::string   getReportKey();
        ta_uint32 getDataNodeKey();
        ta_uint32 getDataPointKey();
        void invalidate();

        EquipReportData(std::string& key);
        EquipReportData(std::string& key, std::string& reportKey);

        /**
         * Constructor
         *
         * Construct a EquipReportData class based around dataset
         */
        EquipReportData(const ta_uint32 row, TA_Base_Core::IData& data);

        virtual ~EquipReportData();

    private:

        // Name field does not apply to our table so we make it private and inaccessible
        std::string getName() { std::string empty(""); return empty; };

        EquipReportDataHelper* m_EquipReportDataHelper;
        EquipReportData& operator=(const EquipReportData&) {};
        EquipReportData(const EquipReportData& theEquipReportData) {};
    };
}

#endif // !defined(AFX_EQUIPREPORTDATA_H__30E577E7_FB1A_4A34_9101_5CBBEB7EC343__INCLUDED_)
