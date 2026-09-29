#pragma once
#include <vector>
#include <string>

namespace TA_IRS_App
{
    struct ITabPage
    {
        virtual void windowShown() = 0;
        virtual bool findAndSelectStationMessage(const std::string& messageName) = 0;
        virtual ~ITabPage() = default;
    };
}
