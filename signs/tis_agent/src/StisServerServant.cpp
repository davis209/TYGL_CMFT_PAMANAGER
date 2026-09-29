#include "pch.h"
#include "StisServerServant.h"
#include "app/common/lib/corbalib/vc10/include/ICosDmdServer.hh"
#include "app/signs/common_library/src/stis_protocol/STISMessageClient.h"
#include "core/utility/src/base_ex/DebugUtilEx.h"
#include "core/utility/src/base_ex/DAI.h"
#include "core/utility/src/base_ex/LocationAccessFactoryEx.h"
#include "core/corba/src/ServantBase.h"
#include <atomic>

namespace TA_IRS_App::legacytisagent::detail
{
    using namespace TA_Base_Ex;
    using namespace TA_Base_Core;
    using namespace STIS_PROTOCOL::MESSAGE_TYPES;
    using namespace STIS_PROTOCOL::INTERFACES;

    struct StisServerServant::Impl : virtual ServantBase, virtual POA_ste::dmd::cos::ICosDmdServer
    {
        Impl()
        {
            m_corba_name = DAI::get_corba_name_from_string("StisServer");
        }

        void start()
        {
            activateServantWithName(m_corba_name.getObjectName());
        }

        void stop()
        {
            deactivateServant();
        }

        virtual void cosPoll() override {}
        virtual void cosPollControl() override {}

        virtual ::CORBA::Long cosDisplayPaEmgMsg(const ::ste::dmd::cos::CosLocSeq& locations, const char* msg, ::CORBA::Long speed) override
        {
            FUNCTION_ENTRY("cosDisplayPaEmgMsg");

            DestinationList destinations;

            for (CORBA::ULong i = 0; i < locations.length(); ++i)
            {
                Destination d;
                d.station_id = LocationAccessFactoryEx::toName(locations[i]);
                destinations.push_back(std::move(d));
            }

            AdHodMessage adhoc;
            std::string msgStr(msg);
            adhoc.message_text.assign(msgStr.begin(), msgStr.end());
            adhoc.priority = 1; // Emergency
            adhoc.start_time = std::string(14, '0');
            adhoc.end_time = std::string(14, '9');

            m_message_client.submit_M11_DisplayAdHocMessageRequestList(destinations, adhoc);

            CORBA::Long scheduleId = ++m_next_schedule_id;
            LOG_INFO("cosDisplayPaEmgMsg: sent M11 to %d locations, scheduleId=%d", (int)locations.length(), (int)scheduleId);

            FUNCTION_EXIT;
            return scheduleId;
        }

        virtual void cosDeletePaEmgMsg(const ::ste::dmd::cos::CosLocSeq& locations, ::CORBA::Long schdId) override
        {
            FUNCTION_ENTRY("cosDeletePaEmgMsg");

            DestinationList destinations;

            for (CORBA::ULong i = 0; i < locations.length(); ++i)
            {
                Destination d;
                d.station_id = LocationAccessFactoryEx::toName(locations[i]);
                destinations.push_back(std::move(d));
            }

            std::vector<int> priorities = {1, 2, 3};
            m_message_client.submit_M20_ClearCurrentMessageRequestList(destinations, priorities);

            LOG_INFO("cosDeletePaEmgMsg: sent M20 to %d locations, scheduleId=%d", (int)locations.length(), (int)schdId);

            FUNCTION_EXIT;
        }

        CorbaName m_corba_name;
        STISMessageClient m_message_client{"--server=local-tis-gateway"};
        std::atomic<CORBA::Long> m_next_schedule_id{0};
    };

    StisServerServant& StisServerServant::instance()
    {
        static auto s_instance = new StisServerServant{};
        return *s_instance;
    }

    StisServerServant::StisServerServant()
        : m_impl(std::make_shared<Impl>())
    {
    }

    void StisServerServant::start()
    {
        m_impl->start();
    }

    void StisServerServant::stop()
    {
        m_impl->stop();
    }
}
