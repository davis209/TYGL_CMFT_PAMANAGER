/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 *
 * Source:    $File: //depot/4669_T01271350/4669/transactive/app/signs/stis_manager/src/IMessageSelectionListener.h $
 * @author:   Adam Radics
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 *
 * Implemented by the GUI to be notified when message selection changes.
 *
 */

#pragma once
#include "core/data_access_interface/tis_agent_4669/src/ITemplateLibrary.h"

namespace TA_IRS_App
{
    class IMessageSelectionListener
    {
    public:

        using Template = TA_Base_Core::Template;
        using TemplatePtr = TA_Base_Core::TemplatePtr;

        /**
         * predefinedMessageSelected
         *
         * Called when the predefined message tab is selected.
         * Sets whether a message is selected and if so the details needed to
         * populate certain fields on the display page.
         *
         * This is also called when a message is selected/deselected.
         *
         * @param tabSwitched          only true when the tab has just been switched to
         * @param validMessageSelected
         * @param priority
         *
         */
        virtual void predefinedMessageSelected(bool tabSwitched,
                                               bool validMessageSelected,
                                               const char* title = "",
                                               unsigned short priority = 0) = 0;

        /**
         * adHocMessageSelected
         *
         * Called when the ad hoc message tab is selected.
         * Sets whether a message has been entered.
         *
         * This is also called when a message is types/cleared.
         *
         * @param tabSwitched          only true when the tab has just been switched to
         * @param validMessageEntered
         */
        virtual void adHocMessageSelected(bool tabSwitched,
                                          bool hasSelection,
                                          bool validMessageEntered,
                                          std::string title) = 0;

        /**
         * templateSelected
         *
         * Called when the template tab is selected.
         * Sets whether a template is selected and if so the details needed to
         * populate certain fields on the display page.
         *
         * This is also called when a template is selected/deselected.
         *
         * @param tabSwitched          only true when the tab has just been switched to
         * @param validTemplateSelected
         */
        virtual void templateSelected(bool tabSwitched, bool validLcdTemplateSelected, bool validLedTemplateSelected, TemplatePtr lcdTmp, TemplatePtr ledTmp) = 0;

        virtual void predefinedMessageSelectChanged(const std::string description) = 0;

        //virtual void templateSelectChanged(const Template& tmp) = 0;

        virtual ~IMessageSelectionListener() = default;
    };
}
