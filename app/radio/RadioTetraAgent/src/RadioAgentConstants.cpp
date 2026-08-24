/**
  * The source code in this file is the property of
  * MI Services Group and is not for redistribution
  * in any form.
  *
  * Source : $Source: /cvs/1TS-9999-T/code/transactive/app/radio/RadioTetraAgent/src/Attic/RadioAgentConstants.cpp,v $
  * @author  Adam Radics
  * @version $Revision: 1.1.2.1 $
  * Last modification : $Date: 2006/12/15 03:08:56 $
  * Last modified by : $Author: adamr $
  *
  */

#include "core/types/src/ta_types.h"
#include "app/radio/RadioTetraAgent/src/RadioAgentConstants.h"

namespace TA_IRS_App
{

    const short RadioAgentConstants::SYSTEM_ERROR_THRESHOLD_OTE_DEFAULT = 4;
    
    const ta_uint32 RadioAgentConstants::FALLBACK_TIMEOUT_OTE_DEFAULT = 30;

    const std::string RadioAgentConstants::FALLBACK_DATAPOINT_PREFIX = "BaseStationFallback";

    const char RadioAgentConstants::FALLBACK_DATAPOINT_SEPARATOR = ':';

    const ta_uint32 RadioAgentConstants::GET_NE_WAIT_TIME = 5;

    const std::string RadioAgentConstants::TCP_SERVER_DATAPOINT_PREFIX = "TcpServer:";

    const std::string RadioAgentConstants::DEFAULT_OCC_LOGIN_NAME = "OCCA";
    
    const std::string RadioAgentConstants::DEFAULT_STATION_LOGIN_SUFFIX = "SMS";
    
    const std::string RadioAgentConstants::DEFAULT_SDS_QUERY_LOGIN_NAME = "DBQ";

	const std::string RadioAgentConstants::SDS_RADIO_SESSION_NAME  = "RadioSession_SDSQuery";
	const ta_uint32 RadioAgentConstants::DEFAULT_AUDIO_VOLUME = 8;

	const ACE_Time_Value RadioAgentConstants::RADIO_EVENT_ALIVE_TIME = ACE_Time_Value(30); // Reduce to 30 seconds

	const std::string RadioAgentConstants::OCC2_RADIO_SESSION_NAME = "RadioSession_OCC_2";
	const std::string RadioAgentConstants::BOCC2_RADIO_SESSION_NAME = "RadioSession_BOCC_2";
    const std::string RadioAgentConstants::DEFAULT_OCC2_LOGIN_NAME  = "OCCB";
	const std::string RadioAgentConstants::RADIO_LINK_DP_SUFFIX = "diiRADW-LinkFaultNormal-G";
	//const std::string RadioAgentConstants::RADIO_LINK_DP_SUFFIX = "aiiRadio-NormalFallBack";

	const ta_uint32 RadioAgentConstants::KEEP_ALIVE_DEFAULT_INTERVAL = 3;

	const std::string RadioAgentConstants::RADIOSESSION_ISCS_DEFAULT_NAME = "ISCSSessionName";

	const std::string RadioAgentConstants::RPARAM_LANGUAGE = "Lang";
	const std::string RadioAgentConstants::LANGUAGE_EN = "en";
        const std::string RadioAgentConstants::LANGUAGE_ZH = "zh";
    const std::string RadioAgentConstants::LANGUAGE_TW = "zh-tw";

} // namespace TA_IRS_App

