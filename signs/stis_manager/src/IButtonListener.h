/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 *
 * Source:    $File: //depot/4669_T01271350/4669/transactive/app/signs/stis_manager/src/IButtonListener.h $
 * @author:   Adam Radics
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 *
 * Implemented by the GUI to be notified when PID selection changes.
 *
 */

#pragma once

namespace TA_IRS_App
{
    class CallbackButton;

    class IButtonListener
    {
    public:

        /**
         * buttonPressed
         *
         * Gets called when a button is pressed.
         *
         * @param button   the button that was pressed
         */
        virtual void buttonPressed(CallbackButton* button) = 0;
    };
}
