/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/stis_manager/src/LibraryVersionPage.h $
 * @author:  Adam Radics
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 *
 * This page shows the library versions at each location.
 */

#pragma once
#include "core/types/src/ta_types.h"
#include "Resource.h"
#include "LibraryVersionListCtrl.h"
#include "LibraryVersionMonitor.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include <string>
#include <vector>

class REBProgressManager;

namespace TA_IRS_App
{
    class STISPredefinedMessages;
    class STISTemplates;

    class LibraryVersionPage : public CDialog
    {
    public:

        LibraryVersionPage(CWnd* pParent = NULL);

        ~LibraryVersionPage();

        //haipeng added
        void init();
        //haipeng added

        // Dialog Data
        //{{AFX_DATA(LibraryVersionPage)
        enum { IDD = IDD_VERSION_PAGE };
        LibraryVersionListCtrl  m_subHeader1;
        LibraryVersionListCtrl  m_mainHeader1;
        CButton m_upgradeISCS;
        LibraryVersionListCtrl  m_stationVersionList1;
        CEdit   m_nextISCSMessageVersion;
        CEdit   m_currentISCSMessageVersion;
        LibraryVersionListCtrl  m_subHeader2;
        LibraryVersionListCtrl  m_mainHeader2;
        CButton m_upgradeISCS2;
        LibraryVersionListCtrl  m_stationVersionList2;
        CEdit   m_nextISCSTemplateVersion;
        CEdit   m_currentISCSTemplateVersion;
        //}}AFX_DATA

    protected:

        // the columns for the list control
        enum VersionListColumns
        {
            Station = 0,
            CurrentISCS = 1,
            CurrentSTIS = 2,
            NextISCS = 3,
            NextSTIS = 4,
            Spare = 5
        };

        virtual afx_msg void OnOK();
        virtual afx_msg void OnCancel();

        // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(LibraryVersionPage)

    public:

        virtual BOOL PreTranslateMessage(MSG* pMsg);

    protected:

        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        //}}AFX_VIRTUAL

        // Generated message map functions
        //{{AFX_MSG(LibraryVersionPage)
        virtual BOOL OnInitDialog();
        afx_msg void OnDestroy();
        afx_msg LRESULT onRightsChanged(WPARAM wParam, LPARAM lParam);
        afx_msg void OnUpgradeIscsMessage();
        afx_msg LRESULT OnMessageLibraryVersionChanged(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT OnNextMessageLibraryVersionChanged(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT OnCurrentMessageLibraryVersionChanged(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT OnOCCMessageLibrariesSynchronisedChanged(WPARAM wParam, LPARAM lParam);
        afx_msg void OnUpgradeIscsTemplate();
        afx_msg LRESULT OnTemplateLibraryVersionChanged(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT OnNextTemplateLibraryVersionChanged(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT OnCurrentTemplateLibraryVersionChanged(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT OnOCCTemplateLibrariesSynchronisedChanged(WPARAM wParam, LPARAM lParam);
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()

    private:

        /**
         * setupLists1
         *
         * Sets up the list controls
         */
        void setupLists1();

        /**
         * setupLists1
         *
         * Sets up the list2 controls
         */
        void setupLists2();

        /**
         * populateLists
         *
         * Sets up the list controls
         */
        void populateMessagePageData();

        /**
         * populateLists
         *
         * Sets up the list controls
         */
        void populateTemplatePageData();

        /**
         * enableUpgradeIscsMessageButton
         *
         * Based on a number of factors, enable the Upgrade ISCS button
         * if it should be enabled. Disable it if not.
         *
         * enable the upgrade ISCS button if
         * - the libraries are synchronised across the stations
         * - The library hasnt been upgraded yet
         */
        void enableUpgradeIscsMessageButton();

        /**
         * enableUpgradeIscsTemplateButton
         *
         * Based on a number of factors, enable the Upgrade ISCS button
         * if it should be enabled. Disable it if not.
         *
         * enable the upgrade ISCS button if
         * - the libraries are synchronised across the stations
         * - The library hasnt been upgraded yet
         */
        void enableUpgradeIscsTemplateButton();

        //std::uint32_t getEntityKeyFromName( const std::string& entityName );

        // access rights
        bool m_canUpgradeISCS;

        // the status of the pre defined message library
        unsigned short m_currentMessageLibraryVersion;
        unsigned short m_nextMessageLibraryVersion;
        bool m_messageLibrariesAreSynchronised;

        // the status of the pre template library
        unsigned short m_currentTemplateLibraryVersion;
        unsigned short m_nextTemplateLibraryVersion;
        bool m_templateLibrariesAreSynchronised;

        // Entity keys of datapoints at each location
        //std::map< std::uint32_t , LocationLibraryVersionInfo> m_locationInfo;

        TA_Base_Core::ReEntrantThreadLockable m_messageVersionInfoLock;
        TA_Base_Core::ReEntrantThreadLockable m_templateVersionInfoLock;

        STISPredefinedMessages* m_stisPredefinedMessages;
        STISTemplates* m_stisTemplates;
        //////////////////////////////////////////////////////////////////////////
        // =========
        typedef struct
        {
            std::string locationName;
            std::uint32_t locationkey;
            std::uint32_t currentVersionKey;
            std::uint32_t nextVersionkey;
        } LocationVerionInfo;
        typedef std::vector<LocationVerionInfo> stationLibraryVersionList;
        //void getLibraryVersion(const std::string& TTISStationLibraryVersionName, stationLibraryVersionList& libraryVersionList);
		CFont m_listfont;
    };

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}
