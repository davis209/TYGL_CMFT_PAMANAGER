#pragma once
#include "MessageTypes.h"
#include "core/utility/src/core/fixed_length_data/all.h"

namespace TA_IRS_App::STIS_PROTOCOL::MESSAGE_DATA_TYPES
{
    using namespace STIS_PROTOCOL::MESSAGE_TYPES;
    using namespace st::fixed_length_data;

    struct DISPLAYEFFECT : EnableFixedLengthUtility<DISPLAYEFFECT>
    {
        Enum<EDisplayMode>      DisplayMode = EDisplayMode::ScrollLeft;
        Enum<ESpeed>            Speed = ESpeed::Medium;
        Integer<4>              RepeatInterval;
        Integer<4>              DisplayTime;
        Enum<EAlignment>        Alignment = EAlignment::Centered;
        Integer<1>              FontSize;
        Integer<1>              FontFamily;
        Integer<1>              FontColor;
        Integer<1>              BackgroundColor;

        auto tied() noexcept
        {
            return std::tie(DisplayMode, Speed, RepeatInterval, DisplayTime, Alignment, FontSize, FontFamily, FontColor, BackgroundColor);
        }

        auto tied() const noexcept
        {
            return const_cast<DISPLAYEFFECT*>(this)->tied();
        }

        DISPLAYEFFECT() = default;

        DISPLAYEFFECT(const DisplayEffect& rhs)
        {
            *this = rhs;
        }

        DISPLAYEFFECT& operator=(const DisplayEffect& rhs)
        {
            clear();
            tied() = rhs.tied();
            return *this;
        }
    };
}
