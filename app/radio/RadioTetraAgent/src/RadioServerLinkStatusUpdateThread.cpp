// RadioServerLinkStatusUpdateThread.cpp: implementation of the RadioServerLinkStatusUpdateThread class.
//
//////////////////////////////////////////////////////////////////////

#include "core/types/src/ta_types.h"
#include "RadioServerLinkStatusUpdateThread.h"
#include "RadioAgentConstants.h"

#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"

#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/types/public_types/Constants.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utility/src/base_ex/DataPointUtil.h"
#include "core/utility/src/base_ex/LocationAccessFactoryEx.h"

using TA_Base_Ex::DataPointUtil;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RadioServerLinkStatusUpdateThread::RadioServerLinkStatusUpdateThread(): 
m_dataPoint( NULL ),
m_terminate(false),
m_DpServerStatus(6)
{
	FUNCTION_ENTRY( "RadioServerLinkStatusUpdateThread" );

	FUNCTION_EXIT;
}

RadioServerLinkStatusUpdateThread::~RadioServerLinkStatusUpdateThread()
{
	FUNCTION_ENTRY( "~RadioServerLinkStatusUpdateThread" );

	FUNCTION_EXIT;
}

bool RadioServerLinkStatusUpdateThread::setDataPoint( TA_Base_Bus::DataPoint* dataPoint )
{
    FUNCTION_ENTRY( "setDataPoint" );

    if (dataPoint)
    {

        DataPointUtil::instance().bind_boolean_datapoint(dataPoint, [&]
        {
            return checkStatus();
        });

		std::string datapointName = dataPoint->getDataPointName();
		m_dataPoint = dataPoint;
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
			"Radio Server Link Status Datapoint set to %s",
			datapointName.c_str());

		FUNCTION_EXIT;
        return true;
    }

    if ( NULL == m_dataPoint && NULL != dataPoint)
    {
        std::string datapointName = dataPoint->getDataPointName();

        if ( std::string::npos != datapointName.rfind( TA_IRS_App::RadioAgentConstants::RADIO_LINK_DP_SUFFIX ) )
        {
            m_dataPoint = dataPoint;

            // set initial status
            m_semaphore.post();

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                         "Radio Server Link Status Datapoint set to %s",
                         datapointName.c_str() );

            FUNCTION_EXIT;
            return true;
        }
    }

    FUNCTION_EXIT;
    return false;
}

void RadioServerLinkStatusUpdateThread::run()
{
	FUNCTION_ENTRY( "run" );

	//while(!m_terminate)
	//{
	//	m_semaphore.wait();

 //       if ( NULL != m_dataPoint )
	//	{
	//		try
	//		{
	//			TA_THREADGUARD(m_setValueLock)

 //               TA_Base_Bus::DpValue dpValue( m_dataPoint->getDataPointDataType(),
 //                                             m_dataPoint->getEnumLabelsMap(),
 //                                             m_dataPoint->getBooleanLabels() );

	//			//dpValue.setFloat(m_DpServerStatus.to_ulong());
	//			dpValue.setBoolean(m_DpServerStatus.to_ulong());
 //               
 //               timeb newTimestamp = TA_Base_Core::defaultTime;
	//		    newTimestamp.time = time( NULL );

 //               TA_Base_Bus::DataPointState newState( dpValue,
 //                                                     newTimestamp,
 //                                                     TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );

 //               m_dataPoint->updateFieldState( newState );

	//			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
 //                            "RadioServerLinkStatusUpdateThread - Datapoint status value set to %d", m_DpServerStatus.to_ulong());

	//		}
 //           catch( TA_Base_Core::TransactiveException& e )
 //           {
 //               LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );
 //           }
 //           catch( ... )
 //           {
 //               LOG_EXCEPTION_CATCH( SourceInfo, "...", "While setting server status datapoint" );
 //           }
	//	}
	//    else
	//    {
	//        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
	//                     "RadioServerLinkStatusUpdateThread - Datapoint not initialised" );
	//    }
	//}
}

void RadioServerLinkStatusUpdateThread::terminate()
{
	m_terminate = true;
	m_semaphore.post();
}

void RadioServerLinkStatusUpdateThread::setLinkStatus(DpServerStatus statusCode, bool value)
{
	TA_THREADGUARD(m_setValueLock)

	m_DpServerStatus[statusCode] = value;
	m_semaphore.post();
}

bool RadioServerLinkStatusUpdateThread::checkStatus()
{
	bool value = false;
	bool isPriDown = m_DpServerStatus[RadioServerLinkStatusUpdateThread::Server0Down];
	bool isSecDown = m_DpServerStatus[RadioServerLinkStatusUpdateThread::Server1Down];

	if (isPriDown && isSecDown)
		value = true;
	else
		value = false;
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
		"Server Status Primary: %s - Secondary: %s", isPriDown ? "OFFLINE":"ONLINE", isSecDown ? "OFFLINE": "ONLINE");

	return value;
}

void RadioServerLinkStatusUpdateThread::processEntityUpdateEvent(ta_uint32 entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType)
{
	return;
}
