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

#include "stdafx.h"
#include "UserMessages.h"
#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/types/src/ta_types.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/CodeConverter.h"
#include "core/utility/src/core/StaticObject.h"
#include <boost/tokenizer.hpp>
#include <sstream>

using namespace TA_Base_Core;
using st::StaticObject;

namespace TA_IRS_App
{
	/*
    const char* UserMessages::INFO_REQUEST_SUCCESSFUL = "%s Request sent successfully";                                         // [type]
    const char* UserMessages::QUESTION_DISPLAY = "Are you sure you want to display the following %s message?\n\"%s\"";          // [type] [message]
    const char* UserMessages::QUESTION_DISPLAY_MESSAGE = "Are you sure you want to display the following message?\n\"%s\"";     // [message]
    const char* UserMessages::QUESTION_DISPLAY_TEMPLATE = "Are you sure you want to display the following template?\n%s";       // [template]
    const char* UserMessages::QUESTION_DISPLAY_MESSAGE_TEMPLATE = "Are you sure you want to display the following message?\n\"%s\"\n\nAre you sure you want to display the following template?\n%s";  // [message] [template]
    const char* UserMessages::QUESTION_CLEAR = "Are you sure you want to submit a %s request?";                     // [type]
    const char* UserMessages::QUESTION_CLEAR_CURRENT_MESSAGES_BY_MESSAGE_TAG = "Are you sure you want to Clear Current Messages by Message Tag(ID)?\n%s";     // [Tag]
    const char* UserMessages::QUESTION_DELETE_PID_GROUP = "Are you sure you want to delete the PID group %s?";      // [group]
    const char* UserMessages::QUESTION_UPGRADE_LIBRARY = "Are you sure you want to upgrade the library version?";
    const char* UserMessages::ERROR_NO_AGENT_NO_PREDEFINED = "Error connecting to the TIS Agent. STIS Predefined messages are unavailable.";
    const char* UserMessages::ERROR_NO_PREDEFINED_IN_DB = "The current STIS predefined message library was not found in the database.";
    const char* UserMessages::ERROR_LOADING_PREDEFINED = "Error loading the current STIS predefined message library from the database.";
    const char* UserMessages::ERROR_NO_TEMPLATE_IN_DB = "The current STIS template library was not found in the database.";
    const char* UserMessages::ERROR_LOADING_TEMPLATE = "Error loading the current STIS template library from the database.";
    const char* UserMessages::ERROR_START_TIME_AFTER_END = "Start time cannot be after End time. Cannot submit display request.";
    const char* UserMessages::ERROR_START_TIME_EQUALS_END = "Start time is the same as End time. Cannot submit display request.";
    const char* UserMessages::ERROR_START_TIME_BEFORE_CURRENT_TIME = "Start time cannot be before Current time. Cannot submit display request.";  //xufeng++ 2006/10/14 TD14367
    const char* UserMessages::ERROR_END_TIME_PASSED = "The End time has passed. Cannot submit display request.";
    const char* UserMessages::ERROR_REQUEST_FAILED = "%s request failed.\n%s";              // [Type], [error]
    const char* UserMessages::ERROR_UPGRADE_FAILED = "Error upgrading ISCS version. %s";    // [error]
	*/
	const char* UserMessages::INFO_REQUEST_SUCCESSFUL =
		"%s 請求已成功送出";                                         // [type]

	const char* UserMessages::QUESTION_DISPLAY =
		"您確定要顯示以下 %s 訊息嗎？\n\"%s\"";                       // [type] [message]

	const char* UserMessages::QUESTION_DISPLAY_MESSAGE =
		"您確定要顯示以下訊息嗎？\n\"%s\"";                            // [message]

	const char* UserMessages::QUESTION_DISPLAY_TEMPLATE =
		"您確定要顯示以下範本嗎？\n%s";                                // [template]

	const char* UserMessages::QUESTION_DISPLAY_MESSAGE_TEMPLATE =
		"您確定要顯示以下訊息嗎？\n\"%s\"\n\n您確定要顯示以下範本嗎？\n%s"; // [message] [template]

	const char* UserMessages::QUESTION_CLEAR =
		"您確定要送出一個 %s 請求嗎？";                                // [type]

	const char* UserMessages::QUESTION_CLEAR_CURRENT_MESSAGES_BY_MESSAGE_TAG =
		"您確定要依據訊息標籤（ID）清除目前的訊息嗎？\n%s";              // [Tag]

	const char* UserMessages::QUESTION_DELETE_PID_GROUP =
		"您確定要刪除 PID 群組 %s 嗎？";                               // [group]

	const char* UserMessages::QUESTION_UPGRADE_LIBRARY =
		"您確定要升級函式庫版本嗎？";

	const char* UserMessages::ERROR_NO_AGENT_NO_PREDEFINED =
		"連線至 TIS Agent 時發生錯誤。STIS 預設訊息無法使用。";

	const char* UserMessages::ERROR_NO_PREDEFINED_IN_DB =
		"在資料庫中找不到目前的 STIS 預設訊息函式庫。";

	const char* UserMessages::ERROR_LOADING_PREDEFINED =
		"從資料庫載入目前的 STIS 預設訊息函式庫時發生錯誤。";

	const char* UserMessages::ERROR_NO_TEMPLATE_IN_DB =
		"在資料庫中找不到目前的 STIS 範本函式庫。";

	const char* UserMessages::ERROR_LOADING_TEMPLATE =
		"從資料庫載入目前的 STIS 範本函式庫時發生錯誤。";

	const char* UserMessages::ERROR_START_TIME_AFTER_END =
		"開始時間不可晚於結束時間。無法送出顯示請求。";

	const char* UserMessages::ERROR_START_TIME_EQUALS_END =
		"開始時間與結束時間相同。無法送出顯示請求。";

	const char* UserMessages::ERROR_START_TIME_BEFORE_CURRENT_TIME =
		"開始時間不可早於目前時間。無法送出顯示請求。";  // xufeng++ 2006/10/14 TD14367

	const char* UserMessages::ERROR_END_TIME_PASSED =
		"結束時間已過。無法送出顯示請求。";

	const char* UserMessages::ERROR_REQUEST_FAILED =
		"%s 請求失敗。\n%s";                                         // [Type], [error]

	const char* UserMessages::ERROR_UPGRADE_FAILED =
		"升級 ISCS 版本時發生錯誤。%s";                               // [error]

	const char* UserMessages::ERROR_NO_PIDS_SELECTED =
		"未選擇任何PID。請至少選擇一個PID。";

	const char* UserMessages::ERROR_INVALID_TIME_FORMAT =
		"螢幕時間必須為4位數HHMM格式（例如 0630、2200）。";

	const char* UserMessages::ERROR_INVALID_TIME_VALUE =
		"時間無效。小時必須為00-23，分鐘必須為00-59。";

	const char* UserMessages::INFO_SCHEDULE_PID_SUCCESS =
		"排程PID開/關時間設定請求已成功送出。";

	const char* UserMessages::INFO_PID_CONTROL_SUCCESS =
		"PID開/關控制請求已成功送出。";

	const char* UserMessages::ERROR_PID_CONTROL_FAILED =
		"PID控制請求失敗。";

    UserMessages& UserMessages::getInstance()
    {
        return StaticObject<UserMessages>::value();
    }

    UserMessages::UserMessages()
    {
        //m_parent =  AfxGetMainWnd();
    }

    UserMessages::~UserMessages()
    {
    }

    void UserMessages::setParent(CWnd* parent)
    {
        m_parent = parent;
    }

    void UserMessages::setMessageSuppression(bool suppressionEnabled)
    {
        m_suppressMessages = suppressionEnabled;

        // if suppression has been turned off
        if ((!m_suppressMessages) && (m_queuedMessages.size() > 0))
        {
            displayMessage(boost::algorithm::join(m_queuedMessages, "").c_str());

            // reset queued errors
            m_queuedMessages.clear();
        }
    }

    void UserMessages::displayErrorOnce(ErrorType errorType, const char* message)
    {
        // if the message hasnt already bee displayed
        if (!hasBeenDisplayed(errorType))
        {
            // display the message, and set it to displayed
            m_alreadyDisplayed[errorType] = true;
            displayError(message);
        }
    }

    void UserMessages::displayError(const char* message, UINT nType /* = MB_OK | MB_ICONSTOP | MB_TOPMOST */)
    {
        // if suppression is enabled
        if (m_suppressMessages)
        {
            // queue the message
            m_queuedMessages.push_back(message);
        }
        else
        {
            // otherwise display it
            displayMessage(message, nType);
        }
    }

    void UserMessages::displayError(std::wstring message, UINT nType /* = MB_OK | MB_ICONSTOP | MB_TOPMOST */)
    {
        ::MessageBoxW(AfxGetApp()->m_pMainWnd->GetSafeHwnd(), message.c_str(), L"旅客資訊管理器", nType);
    }

    void UserMessages::displayWarning(const char* message, UINT nType /* = MB_OK | MB_ICONEXCLAMATION | MB_TOPMOST */)
    {
        // if suppression is enabled
        if (m_suppressMessages)
        {
            // queue the message
            m_queuedMessages.push_back(message);
        }
        else
        {
            // otherwise display it
            displayMessage(message, nType);
        }
    }

    void UserMessages::displayInfo(const char* message, UINT nType /* = MB_OK | MB_ICONINFORMATION | MB_TOPMOST */)
    {
        // if suppression is enabled
        if (m_suppressMessages)
        {
            // queue the message
            m_queuedMessages.push_back(message);
        }
        else
        {
            // otherwise display it
            displayMessage(message, nType);
        }
    }

    int UserMessages::askQuestion(const char* message, UINT nType /* = MB_YESNO | MB_ICONQUESTION | MB_TOPMOST */)
    {
        return displayMessage(message, nType);
    }

    int UserMessages::askQuestionUTF8(std::string message, UINT nType /*= MB_YESNO | MB_ICONQUESTION | MB_TOPMOST*/)
    {
		//return ::MessageBoxW(AfxGetApp()->m_pMainWnd->GetSafeHwnd(), CodeConverter::UTF8ToUnicode(message).c_str(), L"STIS Manager", nType);
        return ::MessageBox(AfxGetApp()->m_pMainWnd->GetSafeHwnd(), message.c_str(), "旅客資訊管理器", nType);
    }

    bool UserMessages::hasBeenDisplayed(ErrorType errorType)
    {
        return m_alreadyDisplayed.count(errorType);
    }

    int UserMessages::displayMessage(const char* message, UINT nType/* = MB_OK | MB_ICONSTOP | MB_TOPMOST*/)
    {
        //return AfxMessageBox(message, nType);

        // TD14164 ++
        TA_Base_Bus::TransActiveMessage userMsg;
        CString reason = breakMessage(message).c_str();
        userMsg << reason;
        CString errMsg = userMsg.constructMessage(IDS_UE_020071);

        /*return AfxMessageBox(breakMessage(message).c_str(), nType);*/
        return ::MessageBox(AfxGetApp()->m_pMainWnd->GetSafeHwnd(), errMsg, "旅客資訊管理器" , nType);
        //return AfxMessageBox(errMsg, nType);
        // ++ TD14164
    }

    std::string UserMessages::breakMessage(std::string theString)
    {
        typedef boost::tokenizer< boost::char_separator<char> > tokenizer;

        //CWnd* appWindow = AfxGetMainWnd();
        CWnd* appWindow = UserMessages::getInstance().m_parent;
        TA_ASSERT(appWindow, "Top level window is null");
        CRect windowRect;
        appWindow->GetWindowRect(&windowRect);
        // 1280 is the minimum size for this application
        int maximumLineLength = 640;

        if (windowRect.Width() > 1920) // 1280
        {
            maximumLineLength = windowRect.Width() / 2;
        }

        std::string newString;
        CDC deviceContext;
        deviceContext.CreateDC(_T("DISPLAY"), NULL, NULL, NULL);

        // break on newlines
        boost::char_separator<char> lineSep("\n");
        tokenizer lineTokens(theString, lineSep);

        // for each line
        for (tokenizer::iterator line_iter = lineTokens.begin(); line_iter != lineTokens.end(); ++line_iter)
        {
            // calculate the length
            CSize lineSize = deviceContext.GetTextExtent(line_iter->c_str());

            // if the line is too long
            if (lineSize.cx > maximumLineLength)
            {
                // build it word by word
                std::string newLine;
                int newLineLength = 0;

                // break on spaces
                boost::char_separator<char> wordSep(" ");
                tokenizer wordTokens(*line_iter, wordSep);

                for (tokenizer::iterator word_iter = wordTokens.begin(); word_iter != wordTokens.end(); ++word_iter)
                {
                    // for each word
                    std::string word(*word_iter);
                    word.append(" ");

                    // calculate the length
                    CSize wordSize = deviceContext.GetTextExtent(word.c_str());

                    // if it is too long
                    if (wordSize.cx > maximumLineLength)
                    {
                        // end the line and start a new one
                        if (newLine.length() > 0)
                        {
                            newLine.append("\n");
                            newString.append(newLine);
                            newLine = "";
                            newLineLength = 0;
                        }

                        while (wordSize.cx > maximumLineLength)
                        {
                            // put a space in at the maximum length (or thereabouts)
                            ta_int32 spacePosition = (maximumLineLength * word.length()) / wordSize.cx;

                            // put in some of the word, and a newline
                            newString.append(word.substr(0, spacePosition));
                            newString.append("\n");
                            // remove it from the word
                            word.erase(0, spacePosition);

                            // calcualte the length of the leftovers
                            wordSize = deviceContext.GetTextExtent(word.c_str());
                        }

                        if (word.size() > 0)
                        {
                            newLine.append(word);
                            newLineLength = newLineLength + wordSize.cx;
                        }
                    }
                    else
                    {
                        // if it fits on the line - add it
                        if ((newLineLength + wordSize.cx) <= maximumLineLength)
                        {
                            newLine.append(word);
                            newLineLength = newLineLength + wordSize.cx;
                        }
                        else
                        {
                            // end the line and start a new one
                            newLine.append("\n");
                            newString.append(newLine);
                            newLine = "";
                            newLineLength = 0;
                        }
                    }
                }

                if (newLine.size() > 0)
                {
                    newLine.append("\n");
                    newString.append(newLine);
                    newLine = "";
                    newLineLength = 0;
                }
            }
            else
            {
                newString.append(*line_iter);
                newString.append("\n");
            }
        }

        return newString;
    }
}
