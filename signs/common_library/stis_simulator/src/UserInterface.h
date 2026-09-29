#pragma once
#include "STISSimulator.h"

using STISSimulatorPtr = TA_IRS_App::STIS_PROTOCOL::SIMULATOR::STISSimulatorPtr;

struct UserInterface
{
    UserInterface(STISSimulatorPtr simulator);

    void run();
    void add_command(std::string cmd, std::string desc, std::function<void(std::vector<std::string>)> f);

    struct Impl;
    std::shared_ptr<Impl> m_impl;
};
