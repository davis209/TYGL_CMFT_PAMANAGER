/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/J155/transactive/app/signs/stis_manager/src/MonitorInfo.h $
 * @author:  Karen Graham
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * Calculates the screen positions of launch pad (if exist), banner and applications
 *
 */

#pragma once
#include <map>
#include <memory>

namespace TA_Base_Bus
{
    namespace TA_GenericGui
    {
        enum EScreen : int;
    }
}

struct MonitorInfo
{
    using EScreen = TA_Base_Bus::TA_GenericGui::EScreen;

    static MonitorInfo& instance();

    MonitorInfo();

    int getNumberOfMonitors() const
    {
        return m_numberOfMonitors;
    }

    int getEachScreenWidth() const
    {
        return m_eachScreenWidth;
    }

    int getScreenNumber(EScreen targetScreen, int val) const;
    int getWhichScreenCoordinateIsOn(int xCoord) const;
    RECT getRect(EScreen targetScreen, int val = -1);

    int m_screenTop = 0;
    int m_screenBottom = 0;
    int m_eachScreenWidth = 0;
    int m_monitorHeight = 0;
    int m_numberOfMonitors = 0;
    int m_totalScreenWidth = 0;
    std::map<int, RECT> m_rects;
};

using MonitorInfoPtr = std::shared_ptr<MonitorInfo>;
