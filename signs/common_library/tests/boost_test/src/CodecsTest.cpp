// test.cpp : Defines the entry point for the console application.
//

#include "pch.h"
#include "app/signs/common_library/src/Codecs.h"
#include "core/utility/src/base_ex/DebugUtilEx.h"
#include <boost/test/unit_test.hpp>

using namespace TA_IRS_App;
using namespace std::string_literals;
using boost::filesystem::path;

namespace
{
}

BOOST_AUTO_TEST_SUITE(SIGNS)
BOOST_AUTO_TEST_SUITE(COMMON)
BOOST_AUTO_TEST_SUITE(CodecsTest)

BOOST_AUTO_TEST_CASE(test_toChar)
{
    BOOST_TEST(Codecs::toChar(false) == '0');
    BOOST_TEST(Codecs::toChar(true) == '1');
    BOOST_TEST(Codecs::toChar(123) == '3');

    BOOST_TEST(Codecs::toChar(0) == '0');
    BOOST_TEST(Codecs::toChar(1) == '1');
    BOOST_TEST(Codecs::toChar(2) == '2');
}

BOOST_AUTO_TEST_CASE(test_toBool)
{
    BOOST_TEST(Codecs::toBool('0') == false);
    BOOST_TEST(Codecs::toBool('1') == true);

    BOOST_TEST(Codecs::toBool('8') == true);
    BOOST_TEST(Codecs::toBool('9') == true);
}

BOOST_AUTO_TEST_CASE(test_toByte)
{
    BOOST_TEST(Codecs::toByte('0') == 0);
    BOOST_TEST(Codecs::toByte('1') == 1);
    BOOST_TEST(Codecs::toByte('2') == 2);
}

BOOST_AUTO_TEST_CASE(test_padStr)
{
    BOOST_TEST(Codecs::padStr("A", 5, '0', Codecs::AlignType::LeftAligned)      == "A0000"s);
    BOOST_TEST(Codecs::padStr("A", 5, '0', Codecs::AlignType::Centered)         == "00A00"s);
    BOOST_TEST(Codecs::padStr("A", 5, '0', Codecs::AlignType::RightAligned)     == "0000A"s);

    BOOST_TEST(Codecs::padStr("A", 5, '-', Codecs::AlignType::LeftAligned)      == "A----"s);
    BOOST_TEST(Codecs::padStr("A", 5, '-', Codecs::AlignType::RightAligned)     == "----A"s);
    BOOST_TEST(Codecs::padStr("A", 5, '-', Codecs::AlignType::Centered)         == "--A--"s);

    BOOST_TEST(Codecs::padStr("AAAAAAAAAA", 5) == "AAAAA"s);

    {
        std::string str = "A";
        BOOST_TEST(Codecs::padStr(str, 1) == "A");
        BOOST_TEST(str == "A");
    }

    {
        std::string str = "A";
        BOOST_TEST(Codecs::padStr(std::move(str), 1) == "A");
        BOOST_TEST(str != "A");
    }
}

BOOST_AUTO_TEST_CASE(test_encode)
{
    std::vector<unsigned char> vc = { 'h', 'e', 'l', 'l', 'o', ',', ' ', 'w', 'o', 'l', 'd', '!' };
    auto res = Codecs::encode(vc);
    LOG_INFO(res);
    BOOST_TEST((Codecs::decode(res) == vc));
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
