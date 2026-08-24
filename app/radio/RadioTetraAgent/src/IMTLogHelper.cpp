
#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290 4503)
#endif // defined _MSC_VER

#include "IMTLogHelper.h"

#include "bus/security/authentication_library/src/SessionInfo.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"

#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h" // TD19669
#include "core/message/src/AuditMessageSender.h"
#include "core/message/src/CommsMessageSender.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/TrainAudit_MessageTypes.h"
#include "core/message/types/MessageTypes.h"
#include "core/types/src/ta_types.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "bus/trains/TrainCommonLibrary/src/LocationCache.h"
#include "bus/trains/TrainTransactionModel/src/TrainStateLock.h"
#include "core/data_access_interface/src/ILocation.h"
#include <ace/OS.h>

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>


using namespace TA_Base_Core;
using namespace TA_IRS_Bus;
using namespace TA_IRS_App;


AuditMessageSender*                 IMTLogHelper::m_auditSender = NULL;
NonReEntrantThreadLockable          IMTLogHelper::m_lock;
IMTLogHelper*                         IMTLogHelper::m_me = NULL;


IMTLogHelper::IMTLogHelper()
{
    FUNCTION_ENTRY( "IMTLogHelper" );

 //   if (!m_auditSender)
 //   {
 //       m_auditSender = 
 //           MessagePublicationManager::getInstance().getAuditMessageSender( TrainAudit::Context );
 //   }

	//// Determine the local agent for the physical Location of this Console Session.
	//std::string m_sessionID = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
	//m_agentName = ConsoleAccessFactory::getInstance().getConsoleFromSession(m_sessionID)->getAgent();
	//m_key = ConsoleAccessFactory::getInstance().getConsoleFromSession(m_sessionID)->getKey();
	//m_locationKey = TA_IRS_Bus::LocationCache::getInstance().getLocationKey();
	//TA_Base_Core::IEntityDataList entityList = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation("TrainAgent", TA_IRS_Bus::LocationCache::getInstance().getLocationByKey(m_locationKey));

	//m_key = entityList[0]->getKey();

	FUNCTION_EXIT;
}

IMTLogHelper::~IMTLogHelper()
{
    FUNCTION_ENTRY( "~IMTLogHelper" );

    //if (m_auditSender)
    //{
    //    delete m_auditSender;
    //    m_auditSender = NULL;
    //}

 FUNCTION_EXIT;
}

IMTLogHelper* IMTLogHelper::getInstance()
{
    FUNCTION_ENTRY( "getInstance" );

	if ( 0 == m_me )
	{
		// Double checking to prevent multiple threads
		// creating multiple instances.

		ThreadGuard guard( m_lock );

		if ( 0 == m_me )
		{
			m_me = new IMTLogHelper();
    	}
	}

    FUNCTION_EXIT;
	return m_me;
}

void IMTLogHelper::removeInstance( )
{
    FUNCTION_ENTRY( "removeInstance" );

	// 
	// Guard this to prevent multiple threads atempting
	// to delete/create simultaneously
	//
	ThreadGuard guard( m_lock );
	if ( m_me != NULL )
	{
		delete m_me;
		m_me = NULL;
	}

    FUNCTION_EXIT;
}

// trainagent usage
void IMTLogHelper::log(std::string profileName, std::string transactionID, TA_IRS_Bus::CommonTypes::TrainIdType trainId, std::string funcGroup, std::string funcName, std::string stepName, std::string log, std::string result)
{
	FUNCTION_ENTRY("log");

	std::stringstream logStr;
	logStr << "[profile:" << profileName.c_str() <<
		"][transactionId:" << transactionID.c_str() <<
		"][subsystem:TRAIN" <<
		"][resourceName:TrainID " << trainId <<
		"][functionGroup:" << funcGroup.c_str() <<
		"][functionName:" << funcName.c_str() <<
		"][step:" << stepName.c_str() <<
		"][log:" << log.c_str() <<
		"][result:" << result.c_str() <<
		"][autoGenerateReport:Y" <<
		"]";

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "#IMT# %s", logStr.str().c_str());

	FUNCTION_EXIT;
}

// trainagent usage
void IMTLogHelper::log(std::string profileName, std::string transactionID, std::string trainId, std::string funcGroup, std::string funcName, std::string stepName, std::string log, std::string result)
{
	FUNCTION_ENTRY("log");

	std::stringstream logStr;
	logStr << "[profile:" << profileName.c_str() <<
		"][transactionId:" << transactionID.c_str() <<
		"][subsystem:TRAIN" <<
		"][resourceName:TrainID " << trainId.c_str() <<
		"][functionGroup:" << funcGroup.c_str() <<
		"][functionName:" << funcName.c_str() <<
		"][step:" << stepName.c_str() <<
		"][log:" << log.c_str() <<
		"][result:" << result.c_str() <<
		"][autoGenerateReport:Y" <<
		"]";

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "#IMT# %s", logStr.str().c_str());

	FUNCTION_EXIT;
}

// radiogateway usage
void IMTLogHelper::log(std::string resourceName, std::string resourceKey, std::string direction, ta_uint32 transactionID, std::string data)
{
	FUNCTION_ENTRY("log");

	std::stringstream logStr;
	logStr << "[resourceName:" << resourceName.c_str() <<
		"][resourceKeyWordId:" << resourceKey.c_str() <<
		"][subsystem:TRAIN" <<
		"][direction: " << direction.c_str() <<
		"][transactionId:" << transactionID <<
		"][data:" << data.c_str() <<
		"]";

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "#IMT# %s", logStr.str().c_str());

	FUNCTION_EXIT;
}

// radiogateway usage
void IMTLogHelper::log(std::string resourceName, std::string resourceKey, std::string direction, ta_uint32 transactionID, ta_uint32 partNo, std::string data)
{
	FUNCTION_ENTRY("log");

	std::stringstream logStr;
	logStr << "[resourceName:" << resourceName.c_str() <<
		"][resourceKeyWordId:" << resourceKey.c_str() <<
		"][subsystem:TRAIN" <<
		"][direction: " << direction.c_str() <<
		"][transactionId:" << transactionID <<
		"][data: (part " << partNo << "):" << data.c_str() <<
		"]";

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "#IMT# %s", logStr.str().c_str());

	FUNCTION_EXIT;
}

// radiogateway usage
void IMTLogHelper::logMerge(std::string resourceName, std::string resourceKey, ta_uint32 transactionID)
{
	FUNCTION_ENTRY("log");

	std::stringstream logStr;
	logStr << "[resourceName:" << resourceName.c_str() <<
		"][resourceKeyWordId:" << resourceKey.c_str() <<
		"][transactionId:" << transactionID <<
		"]";

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "#IMT# %s", logStr.str().c_str());

	FUNCTION_EXIT;
}

std::string IMTLogHelper::formatTime(time_t time)
{
    FUNCTION_ENTRY( "formatTime" );
	
    // Many places set time to 0 where the time is unused or unavailable.
    // However, this function will blindly return "01-Jan-1970 08:00:00" in 
    // these cases. In order to avoid misunderstanding we don't print the 
    // timestamp if the time is 0
    std::ostringstream timeStr;
    if ( time == 0 )
    {
        timeStr << "N/A";

        FUNCTION_EXIT;
        return timeStr.str();
    }
// song toan++
// TD 10320

    struct tm timeStruct;
    ACE_OS::localtime_r(&time, &timeStruct);

    // DD-Mon-YYYY HH:MM:SS
    timeStr << std::setw(2) << std::setfill('0') << timeStruct.tm_mday << "-"; // DD-

    // Mon-
    switch (timeStruct.tm_mon )
    {
        case 0: timeStr << "Jan-"; break;
        case 1: timeStr << "Feb-"; break;
        case 2: timeStr << "Mar-"; break;
        case 3: timeStr << "Apr-"; break;
        case 4: timeStr << "May-"; break;
        case 5: timeStr << "Jun-"; break;
        case 6: timeStr << "Jul-"; break;
        case 7: timeStr << "Aug-"; break;
        case 8: timeStr << "Sep-"; break;
        case 9: timeStr << "Oct-"; break;
        case 10: timeStr << "Nov-"; break;
        case 11: timeStr << "Dec-"; break;
    }

    timeStr << ( timeStruct.tm_year + 1900 ) << " "; // YYYY

    timeStr << std::setw(2) << std::setfill('0') << timeStruct.tm_hour << ":"; // HH:
    timeStr << std::setw(2) << std::setfill('0') << timeStruct.tm_min << ":"; // MM:
    timeStr << std::setw(2) << std::setfill('0') << timeStruct.tm_sec; // SS
    FUNCTION_EXIT;
    return timeStr.str();
}

void IMTLogHelper::tokenise(const std::string& str,
                          std::vector<std::string>& tokens,
                          const std::string& delimiters)
{
    FUNCTION_ENTRY( "tokenise" );

	/*
    // Skip delimiters at beginning.
    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (std::string::npos != pos || std::string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
	*/
	std::string::size_type firstPos = str.find(delimiters, 0);
    if (std::string::npos != firstPos && firstPos > 0 )
    {
		firstPos = 0;
    }
    // Find first "non-delimiter".
    std::string::size_type secondpos = str.find(delimiters, firstPos);
	
    while ( std::string::npos != secondpos)
    {
		
		tokens.push_back(str.substr(firstPos, secondpos - firstPos));
		
        // Found a token, add it to the vector.
		
        // Skip delimiters.  Note the "not_of"
		firstPos = secondpos + delimiters.size();
        secondpos = str.find_first_of(delimiters, firstPos);
		if (std::string::npos != secondpos)
		{
			tokens.push_back(str.substr(firstPos,secondpos - firstPos));
			firstPos = secondpos + delimiters.size();
			secondpos = str.find_first_of(delimiters, firstPos);
			if (secondpos == std::string::npos)
			{
				tokens.push_back(str.substr(firstPos,str.size() - firstPos));
			}
		}
		else
		{
			tokens.push_back(str.substr(firstPos,str.size() - firstPos));
		}
		
    }

    FUNCTION_EXIT;
}



