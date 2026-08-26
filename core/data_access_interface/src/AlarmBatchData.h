#ifndef __ALARM_BATCH_DATA_H_INCLUDED__
#define __ALARM_BATCH_DATA_H_INCLUDED__
#pragma once

#include "core/data_access_interface/src/IDbWritableData.h"
#include "core/data_access_interface/src/IAlarmData.h"
#include "core/database/src/CommonType.h"
#include "core/types/src/ta_types.h"

#include <boost/shared_ptr.hpp>
#include <string>
#include <map>

namespace TA_Base_Core
{
    typedef boost::shared_ptr<IAlarmData> IAlarmDataPtr;

    class AlarmBatchData
        : public IDbWritableData
    {
    public:

        AlarmBatchData();
        ~AlarmBatchData();

        bool addData(IAlarmDataPtr pAlarmData);

        ta_uint32 getDataSize() { return m_dataList.size(); }

        void clearData() { m_dataList.clear(); }

        bool checkDuplicate(const std::string& alarmID);

        /**
         * applyChanges
         *
         * This will apply all changes made to the database.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if the a parameter name cannot be found, or if
         *            there is more than one value for a parmeter.
         * @exception DataException If the data contained in the Item object
         *            is not sufficent to create an entry in the database, a
         *            DataException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in
         *            all sections.
         *
         * pre: This item has not been deleted
         */
        virtual void applyChanges();

    private:

        bool prepareInsertStmt(IAlarmDataPtr pData, SQLStatement& outStmt);

    private:

        typedef std::map<std::string, IAlarmDataPtr> T_AlarmDataMap;

        T_AlarmDataMap m_dataList;
    }; //!AlarmBatchData

} //!TA_Base_Core

#endif // !__ALARM_BATCH_DATA_H_INCLUDED__
