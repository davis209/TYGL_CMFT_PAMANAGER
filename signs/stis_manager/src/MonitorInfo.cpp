/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/J155/transactive/app/signs/stis_manager/src/MonitorInfo.cpp $
 * @author:  Karen Graham
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * Calculates the screen positions of launch pad (if exist), banner and applications
 *
 */

#include "StdAfx.h"
#include "MonitorInfo.h"
#include "bus/generic_gui/src/GenericGuiConstants.h"
#include "core/utilities/src/RunParamsEx.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/ToString.h"
#include "core/utility/src/core/StaticObject.h"
#include <boost/scope_exit.hpp>
#include <mutex>

using namespace TA_Base_Core;
using namespace TA_Base_Bus;
using st::StaticObject;

namespace
{
    int s_screenTop;
    int s_screenBottom;
    int s_monitorHeight;
    int s_eachScreenWidth;
    int s_numberOfMonitors;
    int s_totalScreenWidth;
}

MonitorInfo& MonitorInfo::instance()
{
    return StaticObject<MonitorInfo>::instance();
}

MonitorInfo::MonitorInfo()
{
    static std::once_flag s_once;

    std::call_once(s_once, [&]
    {
        RECT area;
        SystemParametersInfo(SPI_GETWORKAREA, 0, &area, 0);
        s_screenBottom = area.bottom;
        s_screenTop = area.top;

        struct Resolution
        {
            size_t width;
            size_t height;
        };

        Resolution resolutions[] =
        {
            {640, 256},   {512, 342},   {800, 240},   {512, 384},   {640, 320},   {640, 350},   {640, 360},   {480, 500},
            {720, 348},   {720, 350},   {640, 400},   {720, 364},   {800, 352},   {600, 480},   {640, 480},   {640, 512},
            {768, 480},   {800, 480},   {848, 480},   {854, 480},   {800, 600},   {960, 540},   {832, 624},   {960, 544},
            {1024, 576},  {1024, 600},  {960, 640},   {1024, 640},  {960, 720},   {1136, 640},  {1024, 768},  {1024, 800},
            {1152, 720},  {1152, 768},  {1280, 720},  {1120, 832},  {1280, 768},  {1152, 864},  {1334, 750},  {1280, 800},
            {1152, 900},  {1024, 1024}, {1366, 768},  {1280, 854},  {1600, 768},  {1280, 960},  {1080, 1200}, {1440, 900},
            {1280, 1024}, {1440, 960},  {1600, 900},  {1400, 1050}, {1440, 1024}, {1440, 1080}, {1600, 1024}, {1680, 1050},
            {1776, 1000}, {1600, 1200}, {1600, 1280}, {1920, 1080}, {1920, 1200}, {1920, 1280}, {2048, 1152}, {1792, 1344},
            {1856, 1392}, {2880, 900},  {1800, 1440}, {2048, 1280}, {1920, 1400}, {2538, 1080}, {2560, 1080}, {1920, 1440},
            {2160, 1440}, {2048, 1536}, {2304, 1440}, {2560, 1440}, {2304, 1728}, {2560, 1600}, {2560, 1700}, {2560, 1800},
            {2560, 1920}, {3440, 1440}, {2736, 1824}, {2880, 1800}, {2560, 2048}, {2732, 2048}, {2800, 2100}, {3200, 1800},
            {3000, 2000}, {3200, 2048}, {3200, 2400}, {3840, 2160}, {3840, 2400}, {4096, 2304}, {5120, 2160}, {4096, 3072},
            {4500, 3000}, {5120, 2880}, {5120, 3200}, {5120, 4096}, {6400, 4096}, {6400, 4800}, {7680, 4320}, {7680, 4800},
            {8192, 4608}, {8192, 8192}
        };

        size_t n = sizeof(resolutions) / sizeof(Resolution);
        s_totalScreenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
        s_monitorHeight = GetSystemMetrics(SM_CYSCREEN);
        s_numberOfMonitors = GetSystemMetrics(SM_CMONITORS);
        s_eachScreenWidth = s_totalScreenWidth / s_numberOfMonitors;
        BOOST_SCOPE_EXIT_ALL(&) { LOG_DEBUG("MonitorInfo(): s_totalScreenWidth=%d, s_eachScreenWidth=%d, s_numberOfMonitors=%d, s_screenTop=%d, s_screenBottom=%d, s_monitorHeight=%d", s_totalScreenWidth, s_eachScreenWidth, s_numberOfMonitors, s_screenTop, s_screenBottom, s_monitorHeight); };

        if (1 == s_numberOfMonitors && 2 < s_totalScreenWidth / s_monitorHeight)
        {
            for (size_t i = 0; i < n; ++i)
            {
                Resolution& r = resolutions[i];

                if (s_monitorHeight == r.height)
                {
                    if (0 == s_totalScreenWidth % r.width)
                    {
                        s_eachScreenWidth = r.width;
                        s_numberOfMonitors = s_totalScreenWidth / s_eachScreenWidth;

                        if (3 < s_numberOfMonitors)
                        {
                            LOG_DEBUG("MonitorInfo(): found a resolution (%d x %d), but the calculated number of monitor is %d, trying to find a better one", r.width, r.height, s_numberOfMonitors);
                            continue;
                        }

                        return;
                    }
                }
            }

            if (RunParamsEx::isSetAll("EeachScreenWidth", "NumberOfMonitors"))
            {
                s_numberOfMonitors = RunParamsEx::get<size_t>("EeachScreenWidth");
                s_eachScreenWidth = RunParamsEx::get<size_t>("NumberOfMonitors");
            }
            else
            {
                s_numberOfMonitors = s_totalScreenWidth / (s_monitorHeight * 16 / 9);
                s_eachScreenWidth = s_totalScreenWidth / s_numberOfMonitors;
            }
        }
    });

    m_screenTop = s_screenTop;
    m_screenBottom = s_screenBottom;
    m_eachScreenWidth = s_eachScreenWidth;
    m_numberOfMonitors = s_numberOfMonitors;
    m_totalScreenWidth = s_totalScreenWidth;
    m_monitorHeight = s_monitorHeight;

    m_rects[0] = m_rects[-1] = {0, m_screenTop, m_totalScreenWidth, m_screenBottom};

    for (int i = 1, left = 0; i <= m_numberOfMonitors; i++)
    {
        m_rects[i] = {left, m_screenTop, left += m_eachScreenWidth, m_screenBottom};
        LOG_DEBUG("MonitorInfo(): monitor %d, %s", i, m_rects[i]);
    }
}

int MonitorInfo::getScreenNumber(TA_Base_Bus::TA_GenericGui::EScreen targetScreen, int val) const
{
    int res = -1;

    switch (targetScreen)
    {
    case TA_Base_Bus::TA_GenericGui::SCREEN_CURRENT:         // Area within the current monitor that currentX lies on
        res = getWhichScreenCoordinateIsOn(val);
        break;

    case TA_Base_Bus::TA_GenericGui::SCREEN_PREVIOUS:        // Area within the monitor on the left of the one that currentX lies on.  If there is no screen on the left of this, it will just use the current monitor.
        res = getWhichScreenCoordinateIsOn(val) - 1;
        break;

    case TA_Base_Bus::TA_GenericGui::SCREEN_NEXT:            // Area within the monitor on the right of the one that currentX lies on.  If there is no screen on the right of this, it will just use the current monitor.
        res = getWhichScreenCoordinateIsOn(val) + 1;
        break;

    case TA_Base_Bus::TA_GenericGui::SCREEN_SPECIFIC:        // Area within the specified monitor
        res = val;
        break;

    case TA_Base_Bus::TA_GenericGui::SCREEN_FIRST:           // Area within the first monitor (ignores val)
        res = 1;
        break;

    case TA_Base_Bus::TA_GenericGui::SCREEN_LAST:            // Area within the last monitor (ignores val)
        res = m_numberOfMonitors;
        break;

    case TA_Base_Bus::TA_GenericGui::SCREEN_ALL:             // Area within all screens
    default:
        break;
    }

    if (targetScreen != TA_Base_Bus::TA_GenericGui::SCREEN_ALL)
    {
        res = (res < 1 ? 1 : (m_numberOfMonitors < res ? m_numberOfMonitors : res));
    }

    LOG_DEBUG("getScreenNumber(): targetScreen=%s(%d), val=%d, screenNumber=%d", EnumToStr(targetScreen), targetScreen, val, res);
    return res;
}

int MonitorInfo::getWhichScreenCoordinateIsOn(int xCoord) const
{
    for (size_t monitor = 1; monitor <= m_numberOfMonitors; ++monitor)
    {
        if (xCoord < (m_eachScreenWidth * monitor))
        {
            LOG_INFO("getWhichScreenCoordinateIsOn(): X coordinate %lu can be found on monitor %lu", xCoord, monitor);
            return monitor;
        }
    }

    LOG_INFO("getWhichScreenCoordinateIsOn(): X coordinate %lu can not be found on any monitors.", xCoord);
    return -1;
}

RECT MonitorInfo::getRect(EScreen targetScreen, int val)
{
    return m_rects[getScreenNumber(targetScreen, val)];
}
