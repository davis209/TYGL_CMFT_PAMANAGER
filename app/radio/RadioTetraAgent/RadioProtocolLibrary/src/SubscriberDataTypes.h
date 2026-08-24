/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $Source: g:\depot\MRTA_PPL_TIP\1001\transactive\app\radio\RadioTetraAgent\src\SubsMgntConstants.h $
 * @author:  Lucky Cuizon
 * @version: $Revision:  $
 *
 * Last modification: $Date: 2016/01/14 07:49:56 $
 * Last modified by:  $Author:  $
 * 
 * Provides Subscriber Management Data type
 */
#ifndef _SUBS_MNGMNT_CONSTANTS_H_
#define _SUBS_MNGMNT_CONSTANTS_H_
#include "core/types/src/ta_types.h"
#include <string>
#include <vector>

namespace TA_IRS_App
{
	namespace SubscDataTypes
	{
		enum SubscriberQueryType
		{
			//For Querying Subsriber status
			STATUS_SUBS,
			//For Enabling Subscriber TSI
			ENABLE_SUBS,
			//For Disabling Subscriber TSI
			DISABLE_SUBS
		};

		//Subscriber Management return status
		struct SubsMngmentMsgStat
		{
			std::string subsriberTSI;
			long status;
		};

		//A collection of Subscriber Management Message
		typedef std::vector<SubsMngmentMsgStat> SubsMngmentMsgStatList;

		//A collection us SubscriberTSI
		typedef std::vector<std::string> SubscriberList;


		//Return type of K_SUBSCRIBER_STATUS
		enum RadioSubMngmentStatus
		{
			STATUS_ENABLE = 0,
			STATUS_DISABLE_PENDING,
			STATUS_DISABLE,
			STATUS_ENABLE_PENDING
		};

		//Exception thrown for querying radio subscriber management
		enum RadioSubsExType
		{
			E_MEMBERS_COUNT = 0,
			E_SUBSCRIBER_EMPTY,
			E_NOT_M_TYPE,
			E_NOT_PROVISION
		};

		struct QuerySubsManagement
		{
			RadioSubMngmentStatus SubsriberStatus;
			ta_uint32 NumericSubsReference;

		};
	}
}

#endif