/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/stis_manager/src/CallbackDateTimeCtrl.h $
 * @author:  Adam Radics
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 *
 * This is a Date time control that listens for its own change events
 * and passes them on to the specified interface.
 * Done so that the owning dialog doesnt have to listen for and handle the events.
 */

#pragma once
#include "IDateTimeListener.h"
#include <afxdtctl.h>

namespace TA_IRS_App
{
    class CallbackDateTimeCtrl : public CDateTimeCtrl
    {
    public:

        CallbackDateTimeCtrl() = default;

        void setCallback(IDateTimeListener* listener);

    public:

        // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(CallbackDateTimeCtrl)
        //}}AFX_VIRTUAL

        // Generated message map functions

    protected:

        //{{AFX_MSG(CallbackDateTimeCtrl)
        afx_msg BOOL onDateTimeChange(NMHDR* pNMHDR, LRESULT* pResult);
        //}}AFX_MSG

        DECLARE_MESSAGE_MAP()

    private:

        IDateTimeListener* m_listener = nullptr;
    };
}
