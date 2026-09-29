/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/stis_manager/src/CallbackButton.h $
 * @author:  Adam Radics
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 *
 * This is a button that listens for its own button press events
 * and passes them on to the specified interface.
 * Done so that the owning dialog doesnt have to listen for and handle the events.
 */

#pragma once
#include "IButtonListener.h"

namespace TA_IRS_App
{
    class CallbackButton : public CButton
    {
    public:

        CallbackButton();
        virtual ~CallbackButton();

        void setCallback(IButtonListener* buttonPressListener);

    public:

        // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(CallbackButton)
        //}}AFX_VIRTUAL

        // Generated message map functions

    protected:

        //{{AFX_MSG(CallbackButton)
        afx_msg BOOL onButtonClicked();
        //}}AFX_MSG

        DECLARE_MESSAGE_MAP()

    private:

        IButtonListener* m_buttonPressListener = nullptr;
    };

    /////////////////////////////////////////////////////////////////////////////

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}
