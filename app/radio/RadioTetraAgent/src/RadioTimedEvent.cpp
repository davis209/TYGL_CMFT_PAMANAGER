// RadioTimedEventHash.cpp: implementation of the RadioTimedEventHash class.
//
//////////////////////////////////////////////////////////////////////

#include "core/types/src/ta_types.h"
#include "app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioProtocol.h"
#include "app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioEventReader.h"
#include "app/radio/RadioTetraAgent/src/RadioTimedEvent.h"
#include "app/radio/RadioTetraAgent/src/RadioAgentConstants.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace TA_IRS_App
{
	RadioTimedEvent::RadioTimedEvent(TA_IRS_App::IRadioEventProcessor * eventProcessor, bool CatchEvent)
	:m_RadioEventAliveTime(0),
	 m_RadioEventProcessor(eventProcessor),
	 m_CatchEvent(CatchEvent)
	{
		FUNCTION_ENTRY("Constructor");

		m_RadioEventAliveTime = TA_IRS_App::RadioAgentConstants::RADIO_EVENT_ALIVE_TIME;

		int activeStatus = m_RadioEventTimerHeap.activate();

		TA_ASSERT(activeStatus != -1, "Failed to activate Queue Thread.");

		if(activeStatus == 1)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Queue Thread is already activated.");
		}

		FUNCTION_EXIT;
	}

	RadioTimedEvent::~RadioTimedEvent()
	{
		FUNCTION_ENTRY("Destructor");

		RadioTimerEventMap::iterator it = m_RadioTimerEventMap.begin();

		for (; it != m_RadioTimerEventMap.end(); )
		{
			m_RadioEventTimerHeap.cancel(it->second);
			delete it->first;
			m_RadioTimerEventMap.erase(it++);
		}

		m_RadioEventTimerHeap.deactivate();

		FUNCTION_EXIT;
	}

	void RadioTimedEvent::checkRadioEvent(TA_IRS_App::RadioEvent* event)
	{
		FUNCTION_ENTRY("checkRadioEvent");

		try
		{
			if ( event->getStatus() == STATUS_KO_TOO_MANY_ATTACH_SESSIONS )
			{
				m_RadioEventProcessor->onReceiveEvent( *event );
			}
		}
		catch ( ... )
		{
			
		}

		EventType eventType = event->getEventType();

		if (eventType == EVENT_DISCONNECT)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, 
				"Event: 0x%c000+%lu, session: %lu, transaction: %lu, status: %ld --- to be added into Event cache.",
				((eventType<0xA000) ? '8':'A'),
			  	 (eventType-((eventType<0xA000)?0x8000:0xA000)),
				  event->getSessionRef(),
				  event->getTransactionId(),
				  event->getStatus());

			RadioTimerEventMap::iterator it = m_RadioTimerEventMap.find(event);
			if (it == m_RadioTimerEventMap.end())
			{
				TA_IRS_App::RadioEvent* tmpEvent = new TA_IRS_App::RadioEvent(*event);
				scheduleEvent(tmpEvent);
			}
			else
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, 
					"Event: 0x%c000+%lu, session: %lu, transaction: %lu, status: %ld --- already in cache.",
					((eventType<0xA000) ? '8':'A'),
					 (eventType-((eventType<0xA000)?0x8000:0xA000)),
					  event->getSessionRef(),
					  event->getTransactionId(),
					  event->getStatus());
			}
		}

		FUNCTION_EXIT;
	}

	void RadioTimedEvent::removeRadioEvent(TA_IRS_App::RadioEvent* event)
	{
		FUNCTION_ENTRY("removeRadioEvent");

		RadioTimerEventMap::iterator it = m_RadioTimerEventMap.find(event);
		if (it != m_RadioTimerEventMap.end())
		{
			if (it->second)
			{
				m_RadioTimerEventMap.erase(it);
				m_radioEventTimeStamps.erase(event);

				ta_uint32 transactionId = 0;

                if ( event !=NULL && event->isComplete()){
					transactionId = event->getTransactionId();
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, 
						"Event: 0x%c000+%lu, session: %lu, transaction: %lu, status: %ld --- deleting from cache.",
						((event->getEventType()<0xA000) ? '8':'A'),
						 (event->getEventType()-((event->getEventType()<0xA000)?0x8000:0xA000)),
						  event->getSessionRef(),
						  event->getTransactionId(),
						  event->getStatus());
				}	
				
				if ( NULL != event) {
					// remove the object reference after no more accessing it.
					delete event;
					event = NULL;
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Event with transaction: %lu --- deleted from cache.",transactionId);
				} else {
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Event not removed, it is already NULL");
				}
			}
		}

		FUNCTION_EXIT;
	}

	void RadioTimedEvent::scheduleEvent(TA_IRS_App::RadioEvent* event)
	{
		FUNCTION_ENTRY("scheduleEvent");

		m_RadioTimerEventMap[event] = 0;

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, 
			"Event: 0x%c000+%lu, session: %lu, transaction: %lu, status: %ld --- will be scheduled.",
			((event->getEventType()<0xA000) ? '8':'A'),
			 (event->getEventType()-((event->getEventType()<0xA000)?0x8000:0xA000)),
			  event->getSessionRef(),
			  event->getTransactionId(),
			  event->getStatus());

		int timerID = m_RadioEventTimerHeap.schedule(this, event, ACE_OS::gettimeofday() + m_RadioEventAliveTime);
		
		if (timerID != -1)
		{
			m_RadioTimerEventMap[event] = timerID;
			m_radioEventTimeStamps[event] = ACE_OS::gettimeofday();
		} 
		else
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Failed to schedule event.");
		}

		FUNCTION_EXIT;
	}

	void RadioTimedEvent::setCacheEvent(bool CatchEvent)
	{
		FUNCTION_ENTRY("setCatchEvent");

		m_CatchEvent = CatchEvent;

		if (!CatchEvent)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"Agent now running Control, stop cache.");
			processCachedEvent();
		}
		else
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"Agent now running Monitor, start cache.");
			RadioTimerEventMap::iterator it = m_RadioTimerEventMap.begin();
			for (; it != m_RadioTimerEventMap.end(); it++)
			{
				scheduleEvent(it->first);
			}
		}

		FUNCTION_EXIT;
	}

	void RadioTimedEvent::processCachedEvent()
	{
		FUNCTION_ENTRY("processCachedEvent");

		RadioTimerEventMap::iterator it = m_RadioTimerEventMap.begin();

		for (; it != m_RadioTimerEventMap.end(); it++)
		{
			if (it->second != 0)
			{
				m_RadioEventTimerHeap.cancel(it->second);
				it->second = 0; // Do not care canceled or not, just keep the event.
			}
		}

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Will now process all cached event.");

		it = m_RadioTimerEventMap.begin();

		for (; it != m_RadioTimerEventMap.end(); )
		{
			TA_IRS_App::RadioEvent* event = it->first;
			TA_IRS_App::EventType eventType = event->getEventType();

			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, 
				"Info of event that will be processed by RadioTimedEventHeap ===> Event: 0x%c000+%lu, session: %lu, transaction: %lu, status: %ld",
				((eventType<0xA000) ? '8':'A'),
				(eventType-((eventType<0xA000)?0x8000:0xA000)),
				event->getSessionRef(),
				event->getTransactionId(),
				 event->getStatus());

			m_RadioEventProcessor->onReceiveEvent(*event, getEventTimeStamp(event));

			delete it->first;
			m_RadioTimerEventMap.erase(it++);
			m_radioEventTimeStamps.erase(event);
		}

		FUNCTION_EXIT;
	}
}
