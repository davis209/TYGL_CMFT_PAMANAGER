// RadioTimedEventHash.h: interface for the RadioTimedEventHash class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RADIOTIMEDEVENTHASH_H__6411C40F_39D6_4EB6_AA51_85BC0A41301A__INCLUDED_)
#define AFX_RADIOTIMEDEVENTHASH_H__6411C40F_39D6_4EB6_AA51_85BC0A41301A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>

#include "core/utilities/src/FunctionEntry.h"
#include "app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioEvent.h"
#include "app/radio/RadioTetraAgent/src/IRadioConnectionObserver.h"

#include "ace/Event_Handler.h"
#include "ace/Timer_Heap.h"
#include "ace/Timer_Queue_Adapters.h"

namespace TA_IRS_App
{
	class IRadioEventProcessor;
}

namespace TA_IRS_App
{
	class RadioTimedEvent : public ACE_Event_Handler
	{
	public:
		RadioTimedEvent(IRadioEventProcessor * eventProcessor, bool CatchEvent = true);
		virtual ~RadioTimedEvent();

		/************************************************************************/
		/*  Check if the event is what we want, if it is and not cached yet,    */
		/*  it will be cached into m_RadioTimerEventMap.                        */
		/************************************************************************/
		void checkRadioEvent(TA_IRS_App::RadioEvent* charEvent);

		/************************************************************************/
		/*  Remove the event from cache only when timer expired,                */
		/*  otherwise, we just keep all the event in cache,                     */
		/*  and process them when agent running control.					    */
		/************************************************************************/
		void removeRadioEvent(TA_IRS_App::RadioEvent* charEvent);

		/************************************************************************/
		/*  Schedule a timer that will expired in 3 minutes for each event      */
		/************************************************************************/
		void scheduleEvent(TA_IRS_App::RadioEvent* charEvent);

		/************************************************************************/
		/*  Set if cache event when RadioTCPConnection setIgnoreIncomingData    */
		/************************************************************************/
		void setCacheEvent(bool CatchEventOff);

		/************************************************************************/
		/*  Process all the rest event when Agent running control               */
		/*  Firstly cancel all timer in scheduling,                             */
		/*  and then process all event in m_RadioTimerEventMap                  */
		/************************************************************************/
		void processCachedEvent();

		/************************************************************************/
		/*  Called by ACE_Timer_Heap when a timer expired                       */
		/************************************************************************/
		virtual int handle_timeout (const ACE_Time_Value & expireTime, const void * arg)
		{
			// When we do not catch event any more, 
			// we should keep all events left in the queue for further process.
			if (m_CatchEvent)
			{
				TA_IRS_App::RadioEvent * event = (TA_IRS_App::RadioEvent *) arg;
				removeRadioEvent(event);
			}
			return 0;
		}

		time_t getEventTimeStamp(TA_IRS_App::RadioEvent* radioEv)
		{
			time_t retVal = 0;
			RadioEventTimeStampMap::iterator itFind = m_radioEventTimeStamps.find(radioEv);
			if (itFind != m_radioEventTimeStamps.end())
			{
				retVal = itFind->second.sec();
			}
			return retVal;
		}

		typedef ACE_Thread_Timer_Queue_Adapter<ACE_Timer_Heap>				RadioTimedQueue;
		typedef std::map<TA_IRS_App::RadioEvent*, long>						RadioTimerEventMap;
		typedef std::map<TA_IRS_App::RadioEvent*, ACE_Time_Value>			RadioEventTimeStampMap;

	private:
		RadioTimedQueue														m_RadioEventTimerHeap;
		RadioTimerEventMap													m_RadioTimerEventMap;
		ACE_Time_Value														m_RadioEventAliveTime;
		bool																m_CatchEvent;
		TA_IRS_App::IRadioEventProcessor								*	m_RadioEventProcessor;

		RadioEventTimeStampMap												m_radioEventTimeStamps;
	};
}

#endif // !defined(AFX_RADIOTIMEDEVENTHASH_H__6411C40F_39D6_4EB6_AA51_85BC0A41301A__INCLUDED_)
