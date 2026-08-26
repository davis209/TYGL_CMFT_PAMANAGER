/**
  * PaBroadcastScheduleAccessFactory is a singleton used by the broadcast
  * scheduler to retrieve and update PA broadcast schedule records.
  */

#if !defined(PaBroadcastScheduleAccessFactory_7B4F5FCB_1F9D_4DB9_A266_77CA1B2C9001__INCLUDED_)
#define PaBroadcastScheduleAccessFactory_7B4F5FCB_1F9D_4DB9_A266_77CA1B2C9001__INCLUDED_

#include "core/types/src/ta_types.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "BroadcastScheduleService/include/BroadcastSchedulerService.h"

#include <functional>
#include <string>
#include <vector>

namespace TA_IRS_Core
{
    class PaBroadcastScheduleAccessFactory
    {
    private:
        PaBroadcastScheduleAccessFactory() { };
        PaBroadcastScheduleAccessFactory(const PaBroadcastScheduleAccessFactory& thePaBroadcastScheduleAccessFactory);
        PaBroadcastScheduleAccessFactory& operator=(const PaBroadcastScheduleAccessFactory&);

    public:
        virtual ~PaBroadcastScheduleAccessFactory() { };

        static PaBroadcastScheduleAccessFactory& getInstance();
        static void removeInstance();

        std::vector<pa_scheduler::BroadcastSchedule> getDuePaBroadcastSchedules(
            const ta_uint32 locationKey,
            const ta_uint32 maximumCount,
            const std::function<std::string(const pa_scheduler::BroadcastSchedule&)>& calculateNextRun);

        void updatePaBroadcastScheduleLog(const ta_uint64 runId,
                                          const pa_scheduler::ExecutionResult& execution);

    private:
        static PaBroadcastScheduleAccessFactory* s_instance;
        static TA_Base_Core::NonReEntrantThreadLockable s_singletonLock;
    };

} // closes TA_IRS_Core

#endif // !defined(PaBroadcastScheduleAccessFactory_7B4F5FCB_1F9D_4DB9_A266_77CA1B2C9001__INCLUDED_)
