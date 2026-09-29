#pragma once
#include "app/signs/common_library/src/stis_protocol/message_types/MessageDataTypes.h"

namespace TA_IRS_App::summaryalarm::detail
{
    using namespace STIS_PROTOCOL::MESSAGE_DATA_TYPES;

    struct SummaryAlarm
    {
        SummaryAlarm();

        void operator()(std::shared_ptr<A33> a33) const;
        void operator()(std::shared_ptr<A30> a30) const;

        struct Impl;
        std::shared_ptr<Impl> m_impl;
    };
}

namespace TA_IRS_App
{
    using summaryalarm::detail::SummaryAlarm;
}
