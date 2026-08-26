#ifndef __EVENT_DATA_H_INCLUDED__
#define __EVENT_DATA_H_INCLUDED__
#pragma once

#include "core/data_access_interface/src/ICombinedEventData.h"
#include "core/data_access_interface/src/IDbWritableData.h"

namespace TA_Base_Core
{
    class EventData
        : public IDbWritableData
    {
    public:

        EventData(ICombinedEventDataPtr pEvCombined, bool bPropagate = false);
        virtual ~EventData();

        virtual void applyChanges();

    private:

        std::string getLocalDatabaseName();

    private:

        std::string m_localDatabase;
        ICombinedEventDataPtr m_pEvCombined;
        bool m_bPropagate;
    }; //!EventData

} //!TA_Base_Core

#endif // !__EVENT_DATA_H_INCLUDED__
