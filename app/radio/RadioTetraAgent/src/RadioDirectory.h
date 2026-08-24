/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/RadioDirectory.h $
 * @author:  Nick Jardine
 * @version: $Revision: #1 $
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 *
 * This class provides the server implementation for the methods specifed on the IRadioDirectorySessionCorbaDef 
 * interface. It is also responsible for handling the periodic synchronisation of the subscriber database with
 * the TransActive database.
 */

#if !defined(AFX_RADIODIRECTORY_H__50E4AB83_7237_4538_96DA_D26905ED8A7A__INCLUDED_)
#define AFX_RADIODIRECTORY_H__50E4AB83_7237_4538_96DA_D26905ED8A7A__INCLUDED_

#include <ctime>
#include <string>
#include <map>

//#include "app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodSearchSubscribers.h"

#include "bus/radio/RadioAgentCorbaDef_4669/idl/src/IRadioDirectoryCorbaDef.h"

#include "core/data_access_interface/radio/src/IRASubscribers.h"
//#include "core/threads/src/Thread.h"

namespace TA_IRS_Core
{
    //class RadioDirectoryEntityData; // Forward declaration
    //class IRASubscribers;
}

namespace TA_IRS_App
{
    class Radio;        // Forward declarations
    class RadioEvent;   // ********************
    class RadioMethod;  // ********************

    class RadioDirectory
    {
    public:
	    RadioDirectory(TA_IRS_App::Radio& radio);
	    virtual ~RadioDirectory();

        /**
         * createPatch
         *
         * Creates a patch from the supplied list of groups and saves them under the requested name.
         *
         * @param list The list of groups to add to the patch
         * @param patchName The name to apply to the patch
         */
        virtual void createPatch(std::vector<std::string> patchList,const std::string& patchName);

        /**
         * deletePatch
         *
         * Deletes the named patch from the radio system.
         *
         * @param patchName The name of the patch to delete.
         */
        virtual void deletePatch(const std::string& patchName);


    private:
        
        RadioEvent* sendMethod(RadioMethod& method);

		Radio& m_radio;
    };
}
#endif // !defined(AFX_RADIODIRECTORY_H__50E4AB83_7237_4538_96DA_D26905ED8A7A__INCLUDED_)
