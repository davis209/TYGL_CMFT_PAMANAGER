#ifndef __EVENT_BATCH_DATA_H_INCLUDED__
#define __EVENT_BATCH_DATA_H_INCLUDED__
#pragma once

#include "core/data_access_interface/src/IDbWritableData.h"
#include "core/data_access_interface/src/ICombinedEventData.h"
#include "core/database/src/CommonType.h"
#include <vector>

namespace TA_Base_Core
{
    class EventBatchData
        : public IDbWritableData
    {
    public:

        EventBatchData();
        virtual ~EventBatchData();

        void addData(ICombinedEventDataPtr pEvCombined);
        virtual void applyChanges();

    private:

        std::string getLocalDatabaseName();
        bool prepareInsertStmt(ICombinedEventDataPtr pData, SQLStatement& outStmt);

    private:

        std::string m_localDatabase;
        std::vector<ICombinedEventDataPtr> m_dataList;

    }; //!EventBatchData

} //! TA_Base_Core

#endif // !__EVENT_BATCH_DATA_H_INCLUDED__
