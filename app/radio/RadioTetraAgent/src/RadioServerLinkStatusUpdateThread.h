// RadioServerLinkStatusUpdateThread.h: interface for the RadioServerLinkStatusUpdateThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ATSSERVERLINKSTATUSUPDATETHREAD_H__C595673F_8DB0_4845_BDA9_411CDB08CA29__INCLUDED_)
#define AFX_ATSSERVERLINKSTATUSUPDATETHREAD_H__C595673F_8DB0_4845_BDA9_411CDB08CA29__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/types/src/ta_types.h"
#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"
#include "bus/scada/common_library/src/CommonDefs.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/threads/src/Thread.h"
#include "core/timers/src/TimedWaitSemaphore.h"

#include <bitset>

namespace TA_Base_Bus
{
    class DataPoint;
}

class RadioServerLinkStatusUpdateThread  : public TA_Base_Core::Thread,
										 public TA_Base_Bus::IEntityUpdateEventProcessor
{
public:

	enum DpServerStatus
	{
		ServerFallback = 0,
		Server0Down,
		Server1Down
	};

public:
	RadioServerLinkStatusUpdateThread();
	virtual ~RadioServerLinkStatusUpdateThread();
	bool setDataPoint( TA_Base_Bus::DataPoint* dataPoint );
	void setLinkStatus(DpServerStatus statusCode, bool value);

	void run();
	void terminate();

	bool checkStatus();

	void processEntityUpdateEvent(ta_uint32 entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType);

private:
	
	TA_Base_Bus::DataPoint* 				m_dataPoint;
	bool									m_terminate;

	TA_Base_Core::TimedWaitSemaphore		m_semaphore;
	std::bitset<8>							m_DpServerStatus;
	TA_Base_Core::ReEntrantThreadLockable	m_setValueLock;
};

#endif // !defined(AFX_ATSSERVERLINKSTATUSUPDATETHREAD_H__C595673F_8DB0_4845_BDA9_411CDB08CA29__INCLUDED_)
