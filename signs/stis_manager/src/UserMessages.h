/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File$
 * @author:  Adam Radics
 * @version: $Revision$
 *
 * Last modification: $DateTime$
 * Last modified by:  $Author$
 *
 * A single way to display messages,
 * and a single place to store common error messages.
 *
 */

#pragma once
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include <boost/format.hpp>
#include <map>
#include <vector>

using boost::format;
using boost::io::group;
using boost::str;

namespace TA_IRS_App
{
    class UserMessages
    {
    public:

        // This is used for the dislayError method.
        // basically, this is for initialisation errors that you only want to
        // appear once. Eg there is no tis agent, to stop this popping up multiple
        // times, use displayErrorOnce. Any subsequent calls with the same type won't be displayed.
        // This list will grow as needed.
        enum ErrorType
        {
            UNABLE_TO_LOAD_PREDEFINED, UNABLE_TO_LOAD_TEMPLATE
        };

        // MESSAGES
        static const char* NO_TIS_AGENT_PREDEFINED_UNAVAILABLE;
        static const char* INFO_REQUEST_SUCCESSFUL;
        static const char* QUESTION_DISPLAY;
        static const char* QUESTION_DISPLAY_MESSAGE;
        static const char* QUESTION_DISPLAY_TEMPLATE;
        static const char* QUESTION_DISPLAY_MESSAGE_TEMPLATE;
        static const char* QUESTION_CLEAR;
        static const char* QUESTION_CLEAR_CURRENT_MESSAGES_BY_MESSAGE_TAG;
        static const char* QUESTION_DELETE_PID_GROUP;
        static const char* QUESTION_UPGRADE_LIBRARY;
        static const char* ERROR_NO_AGENT_NO_PREDEFINED;
        static const char* ERROR_NO_PREDEFINED_IN_DB;
        static const char* ERROR_LOADING_PREDEFINED;
        static const char* ERROR_NO_TEMPLATE_IN_DB;
        static const char* ERROR_LOADING_TEMPLATE;
        static const char* ERROR_START_TIME_AFTER_END;
        static const char* ERROR_START_TIME_EQUALS_END;
        //xufeng++ 2006/10/14 TD14367
        static const char* ERROR_START_TIME_BEFORE_CURRENT_TIME;
        //++xufeng 2006/10/14 TD14367
        static const char* ERROR_END_TIME_PASSED;
        static const char* ERROR_REQUEST_FAILED;
        static const char* ERROR_UPGRADE_FAILED;
		static const char* ERROR_NO_PIDS_SELECTED;
		static const char* ERROR_INVALID_TIME_FORMAT;
		static const char* ERROR_INVALID_TIME_VALUE;
		static const char* INFO_SCHEDULE_PID_SUCCESS;
		static const char* INFO_PID_CONTROL_SUCCESS;
		static const char* ERROR_PID_CONTROL_FAILED;

        /**
         * ~UserMessages
         *
         * Standard destructor.
         */
        virtual ~UserMessages();

        /**
         * set the parent window
         *
         * Standard destructor.
         */
        void setParent(CWnd* parent);

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         * Initially, message suppression will be on.
         * This object is not threadlocked, it is displaying message boxes,
         * so should only ever be called from the GUI thread.
         *
         * @return      UserMessages&
         *              A reference to an instance of a UserMessages object.
         *
         */
        static UserMessages& getInstance();

        /**
         * setMessageSuppression
         *
         * Enable or disable message suppression.
         * While suppression is on, messages will be queued. Questions cannot be queued.
         * When turning suppression off, all queued messages will be shown.
         *
         * @param suppressionEnabled
         */
        void setMessageSuppression(bool suppressionEnabled);

        /**
         * displayErrorOnce
         *
         * This only displays each error type once.
         * Eg on initialisation, multiple attempts to contact the train agent are made.
         * the first time it fails, display a message, after that don't. This prevents each page
         * popping up an unable to contact X agent message.
         *
         * @param errorType The type of error
         * @param message  The message to display
         *
         */
        void displayErrorOnce(ErrorType errorType, const char* message);

        /**
         * displayError
         *
         * Display an error message box.
         *
         * @param message  The message text
         * @param nType    The dialog box style (default is error, with an ok button)
         *
         */
        void displayError(const char* message, UINT nType = MB_OK | MB_ICONSTOP | MB_TOPMOST);

        void displayError(std::string message, UINT nType = MB_OK | MB_ICONSTOP | MB_TOPMOST)
        {
            displayError(message.c_str(), nType);
        }

        void displayError(std::wstring message, UINT nType = MB_OK | MB_ICONSTOP | MB_TOPMOST);

        /**
         * displayWarning
         *
         * Display a warning message box
         *
         * @param message  The message text
         * @param nType    The dialog box style (default is exclamation, with an ok button)
         *
         */
        void displayWarning(const char* message, UINT nType = MB_OK | MB_ICONEXCLAMATION | MB_TOPMOST);

        void displayWarning(std::string message, UINT nType = MB_OK | MB_ICONEXCLAMATION | MB_TOPMOST)
        {
            displayWarning(message.c_str(), nType);
        }

        /**
         * displayInfo
         *
         * Display an info message box
         *
         * @param message  The message text
         * @param nType    The dialog box style (default is information, with an ok button)
         *
         */
        void displayInfo(const char* message, UINT nType = MB_OK | MB_ICONINFORMATION | MB_TOPMOST | MB_TOPMOST);

        void displayInfo(std::string message, UINT nType = MB_OK | MB_ICONINFORMATION | MB_TOPMOST | MB_TOPMOST)
        {
            displayInfo(message.c_str(), nType);
        }

        void displayInfo_if(bool test, std::string message, UINT nType = MB_OK | MB_ICONINFORMATION | MB_TOPMOST | MB_TOPMOST)
        {
            if (test)
            {
                displayInfo(message.c_str(), nType);
            }
        }

        /**
         * askQuestion
         *
         * Ask a question, questions cannot be suppressed, as an answer is needed.
         *
         * @param message  The message text
         * @param nType    The dialog box style (default is question, with yes/no buttons)
         *
         * @return IDYES or IDNO (or whatever is appropriate for the given style.)
         *
         */
        int askQuestion(const char* message, UINT nType = MB_YESNO | MB_ICONQUESTION | MB_TOPMOST);

        int askQuestion(std::string message, UINT nType = MB_YESNO | MB_ICONQUESTION | MB_TOPMOST)
        {
            return askQuestion(message.c_str(), nType);
        }

        int askQuestionUTF8(std::string message, UINT nType = MB_YESNO | MB_ICONQUESTION | MB_TOPMOST);

        /**
         * breakMessage
         *
         * Message boxes break on word boundaries. If an operator were to do something stupid
         * (like in an FAT maybe) by sending a message of all W's to test the limits of the system
         * then this would come out in a message box spanning multiple screens.
         *
         * This is the life story of TES 842.
         *
         * This wont affect a regular message - because there are no words in the english language,
         * not even supercalafrajalisticexpialadocious that span more than one screen.
         *
         */
        static std::string breakMessage(std::string theString);

        /**
         * UserMessages
         *
         * Private constructors.
         */
        UserMessages();
        UserMessages& operator=(const UserMessages&) = delete;
        UserMessages(const UserMessages&) = delete;

    private:

        /**
         * displayMessage
         *
         * Just a wrapper for AfxMessageBox.
         *
         * @param message
         * @param nType
         *
         * @return
         *
         */
        int displayMessage(const char* message, UINT nType = MB_OK | MB_ICONSTOP | MB_TOPMOST);

        /**
         * hasBeenDisplayed
         *
         * Has the error type been displayed?
         *
         * @param errorType The error type
         *
         * @return true if the message has been displayed or queued.
         */
        bool hasBeenDisplayed(ErrorType errorType);

        // map of already displayed messages
        std::map<ErrorType, bool> m_alreadyDisplayed;

        // a vector of queued messages
        std::vector<std::string> m_queuedMessages;

        // whether messages are being suppressed
        bool m_suppressMessages = true;

        //haipeng added
        CWnd* m_parent = nullptr;  //message box's parent window
    };
}
