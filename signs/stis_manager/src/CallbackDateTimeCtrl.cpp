/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/stis_manager/src/CallbackDateTimeCtrl.cpp $
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

#include "stdafx.h"
#include "CallbackDateTimeCtrl.h"

#ifdef _DEBUG
    #define new DEBUG_NEW
    #undef THIS_FILE
    static char THIS_FILE[] = __FILE__;
#endif

namespace TA_IRS_App
{
    BEGIN_MESSAGE_MAP(CallbackDateTimeCtrl, CDateTimeCtrl)
        //{{AFX_MSG_MAP(CallbackDateTimeCtrl)
        ON_NOTIFY_REFLECT_EX(DTN_DATETIMECHANGE, onDateTimeChange)
        //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    void CallbackDateTimeCtrl::setCallback(IDateTimeListener* listener)
    {
        m_listener = listener;
    }

    BOOL CallbackDateTimeCtrl::onDateTimeChange(NMHDR* pNMHDR, LRESULT* pResult)
    {
        if (m_listener)
        {
            m_listener->dateTimeChanged(this);
        }

        // allow other windows to handle it
        return FALSE;
    }
}
