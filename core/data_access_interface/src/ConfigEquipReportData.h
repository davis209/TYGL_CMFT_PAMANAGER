/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/ConfigEquipReportData.h $
 * @author:  Rod Rolirad
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * ConfigEquipReportData is an implementation of the IConfigEquipReportData interface
 * to allow access to the EquipReportData table
 *
 */

#if !defined(AFX_CONFIGEQUIPREPORTDATA_H__07FEC4FB_DCF8_451E_B299_9E4A719A0D94__INCLUDED_)
#define AFX_CONFIGEQUIPREPORTDATA_H__07FEC4FB_DCF8_451E_B299_9E4A719A0D94__INCLUDED_

#if _MSC_VER > 1000
    #pragma once
#endif // _MSC_VER > 1000

#include "core/types/src/ta_types.h"
#include <string>
#include <ctime>

#include "core/data_access_interface/src/IConfigEquipReportData.h"

namespace TA_Base_Core
{
    // forward declarations
    class IData;
    class EquipReportDataHelper;

    class ConfigEquipReportData : public IConfigEquipReportData
    {
    public:

        // constructors
        ConfigEquipReportData();
        ConfigEquipReportData(std::string& key);
        ConfigEquipReportData(std::string& key, std::string& reportKey);
        ConfigEquipReportData(const ConfigEquipReportData& theEquipReportData);

        /**
         * Constructor
         *
         * Construct a ConfigEquipReportData class based around dataset
         */
        ConfigEquipReportData(const ta_uint32 row, TA_Base_Core::IData& data);

        // destructor
        virtual ~ConfigEquipReportData();

        virtual ItemChanges getAllItemChanges() { return m_EquipReportDataChanges; }
        virtual bool hasChanged() { return !m_EquipReportDataChanges.empty(); }
        virtual bool isNew();
        virtual ta_uint32 getUniqueIdentifier() { return m_uniqueIdentifier; };
        virtual void applyChanges();
        virtual void deleteThisEquipReportData();
        virtual void invalidate();

        // get methods
        virtual ta_uint32 getKey();
        virtual std::string     getId();
        virtual std::string     getReportKey();
        virtual ta_uint32 getDataNodeKey();
        virtual ta_uint32 getDataPointKey();

        // set methods
        virtual void setReportKey(std::string& reportKey);
        virtual void setDataNodeKey(const ta_uint32 dataNodeKey);
        virtual void setDataPointKey(const ta_uint32 dataPointKey);
        virtual void updateEquipReportDataChanges(const std::string& name,
                                                  const std::string& oldValue,
                                                  const std::string& newValue);

        virtual void setName(const std::string& name) {};
        virtual std::string getName() { std::string blank(""); return blank; };

    private:

        // Assignment operator not used so it is made private
        ConfigEquipReportData& operator=(const ConfigEquipReportData&) {};

        static const std::string    REPORTKEY;

        EquipReportDataHelper*      m_EquipReportDataHelper;

        ta_uint32              m_uniqueIdentifier;
        static ta_int32                 s_nextAvailableIdentifier;

        ItemChanges                 m_EquipReportDataChanges;
    };
}
#endif // !defined(AFX_CONFIGEQUIPREPORTDATA_H__07FEC4FB_DCF8_451E_B299_9E4A719A0D94__INCLUDED_)
