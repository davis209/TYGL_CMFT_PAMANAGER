/**
  * PaBroadcastScheduleAccessFactory is a singleton used by the broadcast
  * scheduler to retrieve and update PA broadcast schedule records.
  */

#if !defined(PaBroadcastScheduleAccessFactory_7B4F5FCB_1F9D_4DB9_A266_77CA1B2C9001__INCLUDED_)
#define PaBroadcastScheduleAccessFactory_7B4F5FCB_1F9D_4DB9_A266_77CA1B2C9001__INCLUDED_

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "BroadcastSchedulerService.h"

#include <functional>
#include <string>
#include <vector>

namespace TA_IRS_App
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

        std::vector<TA_IRS_App::BroadcastSchedule> getDuePaBroadcastSchedules(
            const int locationKey,
            const int maximumCount,
            const std::function<std::string(const TA_IRS_App::BroadcastSchedule&)>& calculateNextRun);

        void updatePaBroadcastScheduleLog(const long runId,
                                          const TA_IRS_App::ExecutionResult& execution);

        // Keep PAAgent's DVA status records aligned with the records written
        // by PAManager for an accepted M44 command.
        void insertPaDvaMessageStatus(const TA_IRS_App::BroadcastSchedule& schedule,
                                      const unsigned int consoleId);
        bool updateLatestPaDvaMessageStatus(const int locationId,
                                            const unsigned int status);

    private:
        static PaBroadcastScheduleAccessFactory* s_instance;
        static TA_Base_Core::NonReEntrantThreadLockable s_singletonLock;
    };

}

#endif // !defined(PaBroadcastScheduleAccessFactory_7B4F5FCB_1F9D_4DB9_A266_77CA1B2C9001__INCLUDED_)
