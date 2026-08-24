// RadioStatusSummary.cpp: implementation of the RadioStatusSummary class.
//
//////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290 4284)
#endif // defined _MSC_VER

#include <string>
#include <vector>
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "RadioStatusSummary.h"
#include "core/data_access_interface/entity_access/src/IConsole.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"

#include "boost/tokenizer.hpp"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_Core;

RadioStatusSummary* RadioStatusSummary::m_me = NULL;

NonReEntrantThreadLockable RadioStatusSummary::m_lockForStatusUpdate;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RadioStatusSummary::RadioStatusSummary()
	: 
	m_SDSStatus(0),
	m_connectionStatus(0),
	m_RadioSessionList(NULL)
{
	m_RadioSessionList.clear();
}

RadioStatusSummary::~RadioStatusSummary()
{

}

RadioStatusSummary* RadioStatusSummary::getInstance()
{
	if ( 0 == m_me )
	{
		// Double checking to prevent multiple threads
		// creating multiple instances.

		ThreadGuard guard( m_lockForStatusUpdate );

		if ( 0 == m_me )
		{
			m_me = new RadioStatusSummary();
    	}
	}

	return m_me;
}

void RadioStatusSummary::removeInstance( )
{
	// 
	// Guard this to prevent multiple threads atempting
	// to delete/create simultaneously
	//
	ThreadGuard guard( m_lockForStatusUpdate );
	if ( m_me != NULL )
	{
		delete m_me;
		m_me = NULL;
	}
}

void RadioStatusSummary::setSDSStatus(bool isConnected)
{
	ThreadGuard guard(m_lockForStatusUpdate);
	m_SDSStatus = isConnected;

	LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Radio SDS Status isConnected=%s", m_SDSStatus ? "True" : "False");
}

void RadioStatusSummary::setConnectionStatus(bool isConnected)
{
	ThreadGuard guard(m_lockForStatusUpdate);
	m_connectionStatus = isConnected;

	LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Radio connection Status isConnected=%s", m_connectionStatus?"True":"False");
}

void RadioStatusSummary::setRadioSessionStatus(std::string sessionId, std::string sessionName, bool isLoggedIn)
{
	ThreadGuard guard(m_lockForStatusUpdate);

	bool isExisting = false;
	int indexToBeRemoved = -1;

	RadioSession incomingSessionStatus;
	incomingSessionStatus.sessionId = sessionId;
	incomingSessionStatus.sessionName = sessionName;
	incomingSessionStatus.isLoggedIn = isLoggedIn;

	if (m_RadioSessionList.size() > 0)
	{
		for (int i = 0; i < m_RadioSessionList.size(); i++)
		{
			if (m_RadioSessionList[i].sessionName.compare(sessionName) == 0)
			{
				// found RadioSession based on object name

				if (m_RadioSessionList[i].sessionId.compare(sessionId) == 0)
				{
					m_RadioSessionList[i].isLoggedIn = isLoggedIn;
					isExisting = true;
				}
				else
				{
					// sessionId seems to have changed for the RadioSession, remove it
					indexToBeRemoved = i;
					isExisting = false;
				}

			}
		}

		if (indexToBeRemoved >= 0)
			m_RadioSessionList.erase(m_RadioSessionList.begin() + indexToBeRemoved);
	}

	if (m_RadioSessionList.size() <= 0 || !isExisting)
	{
		m_RadioSessionList.push_back(incomingSessionStatus);
	}

}

TA_Base_Bus::IRadioCorbaDef::RadioStatusSummary* RadioStatusSummary::getRadioStatusSummary()
{
	ThreadGuard guard(m_lockForStatusUpdate);

	TA_Base_Bus::IRadioCorbaDef::RadioStatusSummary* list = new TA_Base_Bus::IRadioCorbaDef::RadioStatusSummary;
	TA_Base_Bus::IRadioCorbaDef::RadioSessionStatusList sessionList;

	sessionList.length(m_RadioSessionList.size());

	for (int i = 0; i < m_RadioSessionList.size(); i++)
	{
		sessionList[i].sessionName = m_RadioSessionList[i].sessionName.c_str();
		sessionList[i].isLoggedIn = m_RadioSessionList[i].isLoggedIn;
	}

	list->isSDSValid = m_SDSStatus;
	list->isCommunicationValid = m_connectionStatus;
	list->sessionStatusList = sessionList;

	return list;
}



