#pragma once
#include <string>
#include <memory>
#include <atomic>

namespace TA_IRS_App
{
    struct DisplayPageInfo
    {
        struct Predefined
        {
            static inline std::atomic_bool normal_selected = false;
            static inline std::atomic_bool emergency_selected = false;
        };

        struct AdHoc
        {
        };

        struct Template
        {
            static inline std::atomic_bool emergency_checked = false;
            static inline std::atomic_bool lcd_selected = false;
            static inline std::atomic_bool led_selected = false;
        };
    };
}
