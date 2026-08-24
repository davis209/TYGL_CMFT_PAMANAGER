/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/RadioDirectory.cpp $
 * @author:  Nick Jardine
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 *
 * This class provides the server implementation for the methods specifed on the IRadioDirectoryCorbaDef 
 * interface. It is also responsible for handling the periodic synchronisation of the subscriber database with
 * the TransActive database.
 */
#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "core/types/src/ta_types.h"
#include <vector>

#include "app/radio/RadioTetraAgent/src/RadioDirectory.h"
#include "app/radio/RadioTetraAgent/src/Radio.h"
#include "app/radio/RadioTetraAgent/src/RadioSessionException.h"
#include "app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioProtocolLibrary.h"
#include "app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioFields.h"

#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "bus/security/authentication_library/src/SessionInfo.h"
#include "core/data_access_interface/radio/src/RASubscribers.h"
#include "core/data_access_interface/radio/src/RAGroupMembersAccessFactory.h"
#include "core/data_access_interface/radio/src/RASubscribersAccessFactory.h"
#include "core/data_access_interface/radio/src/RAPatchMembersAccessFactory.h"
#include "core/data_access_interface/radio/src/RASpeedDialSetAccessFactory.h"
#include "core/data_access_interface/radio/src/RASpeedDialButtonAccessFactory.h"
#include "core/data_access_interface/radio/src/RASpeedDialButton.h"
//#include "core/data_access_interface/entity_access/src/RadioDirectoryEntityData.h"
//#include "core/data_access_interface/src/ILocation.h"
//#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/DataException.h"

using namespace TA_Base_Core;
using namespace TA_IRS_Core;

namespace TA_IRS_App
{

    //////////////////////////////////////////////////////////////////////
    // Construction/Destruction
    //////////////////////////////////////////////////////////////////////

    RadioDirectory::RadioDirectory( TA_IRS_App::Radio& radio) 
		: m_radio(radio)
    {

    }

    RadioDirectory::~RadioDirectory()
    {

    }


    void RadioDirectory::createPatch(std::vector<std::string> patchList,const std::string& patchName)
    {
        FUNCTION_ENTRY("createPatch");
        // To create a patch, need to create a new subscriber reference for it.
        RadioMethodNewReference* newReferenceMethod = new RadioMethodNewReference(m_radio.getISCSRadioSessionRef(),
            RadioMethodNewReference::SUBSCRIBER_REFERENCE);
        RadioEvent* newReferenceEvent = sendMethod(*newReferenceMethod);
        
        NewReferenceEventReader newReferenceReader(*newReferenceEvent);
        ta_uint32 subscriberReference = newReferenceReader.getReference();

        // Now need to use the subscriber reference in a change reference call to create the patch group.
        //RadioMethodChangeReference* changeReferenceMethod = new RadioMethodChangeReference
        FUNCTION_EXIT;
    }

    void RadioDirectory::deletePatch(const std::string& patchName)
    {
		// TODO
    }



    RadioEvent* RadioDirectory::sendMethod(RadioMethod& method)
    {
         FUNCTION_ENTRY("sendMethod");

        RadioEvent*  event = m_radio.sendMethod(method);

	    // Check for an error
	    if (event->getStatus()<0)
	    {
		    // Throw a radio session error
		    TA_THROW(RadioSessionException(event->getTextStatus(),event->getStatus()));
	    }

	    return event;
    }

}


