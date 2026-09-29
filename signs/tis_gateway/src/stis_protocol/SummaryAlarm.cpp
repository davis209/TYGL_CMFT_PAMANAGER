#include "pch.h"
#include "SummaryAlarm.h"
#include "app/signs/common_library/src/stis_protocol/message_types/MessageDataTypes.h"
#include "bus/scada/common_library/src/CommonDefs.h"
#include "core/utility/src/base_ex/DebugUtilEx.h"
#include "core/utility/src/base_ex/DataPointUtil.h"
#include "core/utility/src/base_ex/RunParamsEx.h"
#include "core/utility/src/base_ex/LocationAccessFactoryEx.h"
#include "core/utility/src/core/StdEx.h"

#define RUNPARAM_MAXCOMMUNICATIONFAILCOUNT "MaxCommunicationFailCount"

namespace
{
    size_t DEFAULT_MAX_COMMUNICATION_FAIL_COUNT = 3;
}

namespace TA_IRS_App::summaryalarm::detail
{
    using namespace TA_Base_Core;
    using namespace TA_Base_Bus;
    using TA_Base_Ex::RunParamsEx;

    struct SummaryAlarm::Impl
    {
        Impl()
        {
            if (ThisLocation::is_occ())
            {
                m_major = m_util.get_datapoint("OCC.TIS.STIS.SEV-OCC.diiSEVOCC-MajorNormalAlarm-G");
                m_minor = m_util.get_datapoint("OCC.TIS.STIS.SEV-OCC.diiSEVOCC-MinorNormalAlarm-G");
            }
            else
            {
                m_major = m_util.get_datapoint(str(boost::format("%s.TIS.STIS.SEV.diiSEV-MajorNormalAlarm-G") % ThisLocation::name()));
                m_minor = m_util.get_datapoint(str(boost::format("%s.TIS.STIS.SEV.diiSEV-MinorNormalAlarm-G") % ThisLocation::name()));
            }

            m_communication_link = m_util.get_datapoint(str(boost::format("%s.BMF.FEP.PHYSICAL.diiSTIS-LinkFaultNormal-G") % ThisLocation::name()));

            RunParamsEx::get_or_inplace(RUNPARAM_MAXCOMMUNICATIONFAILCOUNT, DEFAULT_MAX_COMMUNICATION_FAIL_COUNT);
        }

        template <class T>
        void process(std::shared_ptr<T> a) const
        {
            if (m_major && m_minor)
            {
                if (a)
                {
                    auto summary = a->AlarmSummary.value();
                    m_util.set_datapoint_boolean_value(m_minor, is_minor(summary), QUALITY_GOOD_NO_SPECIFIC_REASON);
                    m_util.set_datapoint_boolean_value(m_major, is_major(summary), QUALITY_GOOD_NO_SPECIFIC_REASON);
                }
                else
                {
                    m_util.set_datapoint_boolean_value(m_minor, 0, QUALITY_BAD_COMM_FAILURE);
                    m_util.set_datapoint_boolean_value(m_major, 0, QUALITY_BAD_COMM_FAILURE);
                }
            }

            m_communication_fail_count = (a ? 0 : m_communication_fail_count + 1);

            if (m_communication_link)
            {
                m_util.set_datapoint_boolean_value(m_communication_link, DEFAULT_MAX_COMMUNICATION_FAIL_COUNT <= m_communication_fail_count, QUALITY_GOOD_NO_SPECIFIC_REASON);
            }
        }

        void operator()(std::shared_ptr<A33> a33) const
        {
            process(a33);
        }

        void operator()(std::shared_ptr<A30> a30) const
        {
            process(a30);
        }

        DataPoint* m_major = nullptr;
        DataPoint* m_minor = nullptr;
        DataPoint* m_communication_link = nullptr;
        DataPointUtil& m_util = DataPointUtil::instance();
        mutable std::atomic_size_t m_communication_fail_count = 0;
    };

    SummaryAlarm::SummaryAlarm()
        : m_impl(std::make_shared<Impl>())
    {
    }

    void SummaryAlarm::operator()(std::shared_ptr<A33> a33) const
    {
        m_impl->operator()(std::move(a33));
    }

    void SummaryAlarm::operator()(std::shared_ptr<A30> a30) const
    {
        m_impl->operator()(std::move(a30));
    }
}
