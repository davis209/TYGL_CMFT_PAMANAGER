#pragma once
#include "core/utility/src/core/algorithm/strings.h"

namespace TA_IRS_App::STIS_PROTOCOL::SIMULATOR::stissimulator::detail
{
    using namespace st2::string_literals;

    const auto ALL_M_MESSAGES_TYPES = "M10,M11,M20,M21,M22,M23,M24,M30,M31,M32,M33,M50,M70"_csv;
    const auto ALL_A_MESSAGE_TYPES = "A10,A20,A21,A22,A24,A30,A31,A32,A33,A50,A70,A99"_csv;
    const auto ALL_MESSAGE_TYPES = "M10,M11,M20,M21,M22,M23,M24,M30,M31,M32,M33,M50,M70,A10,A20,A21,A22,A24,A30,A31,A32,A33,A50,A70,A99"_csv;

    const int OCC_SIMULATOR_CORBA_PORT = 25285;
    const std::string SIMULATOR_SERVANT_KEY = "STIS-SIMULATOR";
}
