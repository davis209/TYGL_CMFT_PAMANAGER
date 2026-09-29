/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/stis_manager/src/RATISPage.cpp $
 * @author:  Adam Radics
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 *
 * RATIS message creation page
 *
 */

#include "stdafx.h"
#include "stismanager.h"
#include "RATISPage.h"
#include <sstream>

#include "cots/ssce/sdk/include/sscemfc.hpp"
#include "core/data_access_interface/stis_manager/src/TiRatisTagAccessFactory.h"
#include "core/data_access_interface/stis_manager/src/TiRatisDestinationAccessFactory.h"

// TD12028
#include "core/exceptions/src/DatabaseException.h"
#include "core/utilities/src/DebugUtil.h"

#ifdef _DEBUG
    #define new DEBUG_NEW
    #undef THIS_FILE
    static char THIS_FILE[] = __FILE__;
#endif

// TD12028
using TA_Base_Core::DebugUtil;
using TA_Base_Core::DatabaseException;

namespace TA_IRS_App
{
    CRATISPage::CRATISPage(CWnd* pParent /*=NULL*/)
        : CDialog(CRATISPage::IDD, pParent),
          m_messageSelectionListener(NULL),
          m_validMessage(false)
    {
        //{{AFX_DATA_INIT(CRATISPage)
        //}}AFX_DATA_INIT
    }

    void CRATISPage::DoDataExchange(CDataExchange* pDX)
    {
        CDialog::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(CRATISPage)
        DDX_Control(pDX, IDC_RADIO_CLEAR, m_radioClear);
        DDX_Control(pDX, IDC_RADIO_NEW, m_radioNew);
        DDX_Control(pDX, IDC_CHB_OVERRIDE, m_chbOverride);
        DDX_Control(pDX, IDC_CHB_VETTING, m_chbVetting);
        DDX_Control(pDX, IDC_RATIS_CONTENT, m_messageContent);
        DDX_Control(pDX, IDC_COMBO2, m_destination);
        DDX_Control(pDX, IDC_COMBO1, m_tag);
        //}}AFX_DATA_MAP
    }

    BEGIN_MESSAGE_MAP(CRATISPage, CDialog)
        //{{AFX_MSG_MAP(CRATISPage)
        ON_CBN_EDITCHANGE(IDC_RATIS_TAG, onEditchangeRatisTag)
        ON_CBN_SELCHANGE(IDC_RATIS_TAG, onSelchangeRatisTag)
        ON_CBN_EDITCHANGE(IDC_RATIS_DEST, onEditchangeRatisDest)
        ON_CBN_SELCHANGE(IDC_RATIS_DEST, onSelchangeRatisDest)
        ON_EN_CHANGE(IDC_RATIS_CONTENT, onChangeRatisContent)
        ON_BN_CLICKED(IDC_SPELL_CHECK, OnSpellCheck)
        ON_BN_CLICKED(IDC_RADIO_CLEAR, OnRadioClear)
        ON_BN_CLICKED(IDC_RADIO_NEW, OnRadioNew)
        //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    RatisMessage CRATISPage::getMessage() const
    {
        CString messageText;
        m_messageContent.GetWindowText(messageText);

        CString messageTag;
        m_tag.GetWindowText(messageTag);

        CString messageDestination;
        m_destination.GetWindowText(messageDestination);

        RatisMessage message;
        message.messageContent = messageText.GetBuffer(0);
        message.clear = m_radioClear.GetCheck() != 0;
        message.tag = messageTag.GetBuffer(0);
        message.override = m_chbOverride.GetCheck() != 0;
        message.vetting = m_chbVetting.GetCheck() != 0;
        message.destination = messageDestination.GetBuffer(0);

        return message;
    }

    bool CRATISPage::messageIsValid()
    {
        RatisMessage msg = getMessage();

        // TODO - check lengths
        return (msg.destination.size() == 3) &&
               (msg.tag.size() == 8) &&
               ((msg.messageContent.size() > 0) || (msg.clear));
    }

    void CRATISPage::windowShown()
    {
        if (m_messageSelectionListener != NULL)
        {
            m_validMessage = messageIsValid();

            m_messageSelectionListener->ratisMessageSelected(true, m_validMessage, m_radioClear.GetCheck() != 0);
        }
    }

    void CRATISPage::onEditchangeRatisTag()
    {
        notifyListener();
    }

    void CRATISPage::onSelchangeRatisTag()
    {
        notifyListener();
    }

    void CRATISPage::onEditchangeRatisDest()
    {
        notifyListener();
    }

    void CRATISPage::onSelchangeRatisDest()
    {
        notifyListener();
    }

    void CRATISPage::onChangeRatisContent()
    {
        m_messageContent.GetWindowText(m_tempMessageContent);
        notifyListener();
    }

    void CRATISPage::notifyListener()
    {
        if (m_messageSelectionListener != NULL)
        {
            bool validMessage = messageIsValid();

            //TD 9991
            if (0 != m_radioClear.GetCheck())
            {
                m_validMessage = validMessage;
                m_messageSelectionListener->ratisMessageSelected(false, m_validMessage, true);
            }
            else
            {
                m_validMessage = validMessage;
                m_messageSelectionListener->ratisMessageSelected(false, m_validMessage, false);
            }
        }
    }

    void CRATISPage::setMessageSelectionListener(IMessageSelectionListener* messageSelectionListener)
    {
        m_messageSelectionListener = messageSelectionListener;
    }

    void CRATISPage::OnOK()
    {
    }

    void CRATISPage::OnCancel()
    {
    }

    void CRATISPage::OnSpellCheck()
    {
        CSentrySpellDlg spellDlg(this);

        spellDlg.SetKey(0x5B39C65E);
        SSCE_SetRegTreeName("HKEY_LOCAL_MACHINE\\Software\\Wintertree\\SSCE");
        spellDlg.Check(m_messageContent);
    }

    void CRATISPage::OnRadioClear()
    {
        m_radioNew.SetCheck(0);
        m_radioClear.SetCheck(1);

        m_messageContent.EnableWindow(FALSE);
        m_messageContent.SetWindowText("");
        GetDlgItem(IDC_SPELL_CHECK)->EnableWindow(false);
        notifyListener();
    }

    void CRATISPage::OnRadioNew()
    {
        //TD 9991
        m_messageContent.SetWindowText(m_tempMessageContent);
        GetDlgItem(IDC_SPELL_CHECK)->EnableWindow(true);

        m_radioNew.SetCheck(1);
        m_radioClear.SetCheck(0);

        m_messageContent.EnableWindow(TRUE);
        notifyListener();
    }

    void CRATISPage::Init()
    {
        //hongran++ TD17562
        m_messageContent.SetLimitText(MAX_RATIS_TEXT_CHARACTERS);
        //++hongran TD17562

        // TD12028 - add a try-catch statement to handle Database exceptions and other
        // unknown exceptions - lizettejl
        // Retrieve a list of RatisTags from DB and Initialize RatisTag combo box
        try
        {
            TA_IRS_Core::ITiRatisTags ratisTags = TA_IRS_Core::TiRatisTagAccessFactory::getInstance().getAllTiRatisTags(false);

            for (TA_IRS_Core::ITiRatisTagsIt itRatisTag = ratisTags.begin(); itRatisTag != ratisTags.end(); ++itRatisTag)
            {
                TA_IRS_Core::ITiRatisTag* pTiRatisTag = *itRatisTag;
                m_tag.AddString(pTiRatisTag->getRatisTag().c_str());

                // It is our responsibility to delete the pointers returned by getAllTiRatisTags to prevent memory leak
                delete pTiRatisTag;
                pTiRatisTag = NULL;
            }

            // Retrieve a list of Ratis Destinations from DB and Initialize Destination combo box
            TA_IRS_Core::ITiRatisDestinations ratisDests = TA_IRS_Core::TiRatisDestinationAccessFactory::getInstance().getAllTiRatisDestinations(false);

            for (TA_IRS_Core::ITiRatisDestinationsIt itRatisDest = ratisDests.begin(); itRatisDest != ratisDests.end(); ++itRatisDest)
            {
                TA_IRS_Core::ITiRatisDestination* pRatisDestination = *itRatisDest;

                m_destination.AddString(pRatisDestination->getDestination().c_str());

                delete pRatisDestination;
                pRatisDestination = NULL;
            }
        }
        catch (const DatabaseException& dbe)
        {
            LOG_EXCEPTION("TA_Base_Core::DatabaseException", dbe.what());
        }
        catch (...)
        {
            LOG_EXCEPTION("Unknown", "Caught when initializing CRatisPage");
        }
    }

    BOOL CRATISPage::OnInitDialog()
    {
        CDialog::OnInitDialog();

        m_radioNew.SetCheck(1);
        m_radioClear.SetCheck(0);

        m_tag.LimitText(8);
        m_destination.LimitText(3);

        return true;
    }
} // TA_IRS_App
