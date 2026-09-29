/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/stis_manager/src/RATISPage.h $
 * @author:  Adam Radics
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 *
 * RATIS message creation page
 *
 */

#if !defined(AFX_RATISPAGE_H__5A23D4F5_9776_4B3E_B2DB_F7D2A35B42E3__INCLUDED_)
#define AFX_RATISPAGE_H__5A23D4F5_9776_4B3E_B2DB_F7D2A35B42E3__INCLUDED_

#if _MSC_VER > 1000
    #pragma once
#endif // _MSC_VER > 1000

#include "IMessageSelectionListener.h"

#include <string>

//hongran++ TD17562
//Max length of the ratis message content
#define MAX_RATIS_TEXT_CHARACTERS 256
//++hongran TD17562

namespace TA_IRS_App
{
    struct RatisMessage
    {
        std::string tag;
        std::string destination;
        std::string messageContent;
        bool clear;
        bool override;
        bool vetting;
    };

    class CRATISPage : public CDialog
    {
    public:

        CRATISPage(CWnd* pParent = NULL);

        void Init();
        /**
         * getMessage
         *
         * Gets all attributes from the page
         *
         * @return
         */
        RatisMessage getMessage() const;

        /**
         * windowShown
         *
         * This tab has just been switched to
         */
        void windowShown();

        /**
         * setMessageSelectionListener
         *
         * Sets the object to notify of selection changes
         *
         * @param messageSelectionListener
         */
        void setMessageSelectionListener(IMessageSelectionListener* messageSelectionListener);

    protected:

        // Dialog Data
        //{{AFX_DATA(CRATISPage)
        enum { IDD = IDD_RATIS };
        CButton m_radioClear;
        CButton m_radioNew;
        CButton m_chbOverride;
        CButton m_chbVetting;
        CEdit   m_messageContent;
        CComboBox   m_destination;
        CComboBox   m_tag;
        //}}AFX_DATA

        CString m_tempMessageContent;

        // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(CRATISPage)

    protected:

        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        //}}AFX_VIRTUAL

        // Generated message map functions
        //{{AFX_MSG(CRATISPage)
        virtual BOOL OnInitDialog();
        afx_msg void onEditchangeRatisTag();
        afx_msg void onSelchangeRatisTag();
        afx_msg void onEditchangeRatisDest();
        afx_msg void onSelchangeRatisDest();
        afx_msg void onChangeRatisContent();
        afx_msg void OnSpellCheck();
        afx_msg void OnRadioClear();
        afx_msg void OnRadioNew();
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()

        /**
         * OnOK
         *
         * This method has been implemented to hide accidental calls when
         * the Enter key is pressed. It can be overriden with new behaviour if required.
         */
        virtual afx_msg void OnOK();

        /**
         * OnCancel
         *
         * This method has been implemented to hide accidental calls when
         * the ESC key is pressed. It can be overriden with new behaviour if required.
         */
        virtual afx_msg void OnCancel();

    private:

        /**
         * messageIsValid
         *
         * Checks whether all fields are valid.
         */
        bool messageIsValid();

        /**
         * notifyListener
         *
         * notifies the listener of a change of status
         */
        void notifyListener();

        // tell this object when message selection changes
        IMessageSelectionListener* m_messageSelectionListener;

        // true if a valid message exists
        bool m_validMessage;
    };

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // TA_IRS_App

#endif // !defined(AFX_RATISPAGE_H__5A23D4F5_9776_4B3E_B2DB_F7D2A35B42E3__INCLUDED_)
