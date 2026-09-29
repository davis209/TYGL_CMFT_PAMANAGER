/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/stis_manager/src/PIDController.h $
 * @author:  San Teo
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 *
 * This class captures and performs the following control requests via the run params:
 * 1. Display the current message on a specific PID
 * 2. Turning a PID on/off
 * 3. Lock/unlock a PID.
 *
 */

#pragma once
#include "core/types/src/ta_types.h"
#include "core/utilities/src/RunParams.h"
#include <vector>

namespace TA_IRS_App
{
    class PIDController : public TA_Base_Core::RunParamUser
    {
    public:

        PIDController();

        virtual ~PIDController() {}

        /**
         * initialise
         *
         * We're pulling all the registration for run params outside the constructor
         * since we don't want this class to listen to the run param changes before
         * the application is fully initialised.
         *
         */
        void initialise();

        /**
         * onRunParamChange
         *
         * Called for a runparam change. selection and deselection.
         *
         */
        void onRunParamChange(const std::string& name, const std::string& value);

    private:

        /**
         * getAndDisplayCurrentMessage
         *
         * This method gets the message that is currently being displayed by the PID from the agent.
         * It then tells the GraphWorX display to display the information.
         *
         * @param station 3 character station ID, e.g 'DBG'
         * @param name The full PID name DBG.TIS.Platform.PDP02
         * @param displayId The the identifier of the graphworx display we want to activate
         *      this is typically taken from the runparams <STN>.<displayIdentifier> identifier
         *      that is sent through in the head of every runparam message
         * @param displayCmd Display command that we need to pass to GraphWorX
         *
         */
        void getCurrentMessage(const std::string& station, const std::string& name, const ta_int32 displayId, const std::string& displayCmd);

        /**
         * changeState
         *
         * Initiate a "soft turn-on" or "soft turn-off" for the selected PID
         *
         * @param name The full PID name DBG.TIS.Platform.PDP02
         * @param on True if it is to turn on the PID.  False otherwise.
         *
         */
        void changeState(const std::string& name, const bool on);

        /**
         * lock
         *
         * Lock or unlocks the selected PID so that it cannot be included in a display request.
         *
         * @param name The full PID name DBG.TIS.Platform.PDP02
         * @param lock True if it is to lock the PID.  False otherwise.
         *
         */
        void lock(const std::string& name, const bool lock);

        /**
         * tokenizeString
         *
         * Copied from STISManagerDlg
         *
         * @param theString
         * @param
         *
         * @return
         *
         */
        //std::vector<std::string> tokenizeString(const std::string& theString, const std::string& separatorList );

        /**
         * timeDateString
         *
         * Gets the tme string in the readable format.
         *
         * @param timestamp e.g."20040112152359" It is in the Format "YYYYMMDDHHMMSS"
         *
         * @return
         *
         */
        std::string timeDateString(const std::string& timeString);
    };
}
