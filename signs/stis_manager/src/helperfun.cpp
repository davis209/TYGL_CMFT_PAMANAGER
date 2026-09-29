#include "stdafx.h"
#include "helperfun.h"
#include <Windows.h>
#include "core/utility/src/core/algorithm/strings.h"
#include "boost/tokenizer.hpp"

std::wstring convertUtf8ToUtf16le(const std::string& src)
{
    const char* srcStr = src.c_str();
    int dstLen = MultiByteToWideChar(CP_UTF8, 0, srcStr, -1, NULL, 0);
    wchar_t* dst = new wchar_t[dstLen];
    MultiByteToWideChar(CP_UTF8, 0, srcStr, -1, dst, dstLen);
    std::wstring result(dst);
    delete[]dst;
    return result;
}

std::string convertUtf16leToUtf8(const std::wstring& src)
{
    const wchar_t* srcStr = src.c_str();
    int dstLen = WideCharToMultiByte(CP_UTF8, 0, srcStr, -1, NULL, 0, NULL, NULL);
    char* dst = new char[dstLen];
    ZeroMemory(dst, dstLen);
    WideCharToMultiByte(CP_UTF8, 0, srcStr, -1, dst, dstLen, NULL, NULL);
    std::string result(dst);
    delete[]dst;
    return result;
}

std::wstring convertBig5ToUtf16le(const std::string& src)
{
    if (src.empty())
    {
        return std::wstring();
    }

    const int dstLen = MultiByteToWideChar(950, 0,
                                            src.data(), static_cast<int>(src.size()),
                                            NULL, 0);
    if (dstLen <= 0)
    {
        return std::wstring();
    }

    std::wstring result(dstLen, L'\0');
    MultiByteToWideChar(950, 0,
                        src.data(), static_cast<int>(src.size()),
                        &result[0], dstLen);
    return result;
}

std::string convertBig5ToUtf8(const std::string& src)
{
    return convertUtf16leToUtf8(convertBig5ToUtf16le(src));
}

std::string convertUtf8ToBig5(const std::string& src)
{
    if (src.empty())
    {
        return std::string();
    }

    const std::wstring unicode = convertUtf8ToUtf16le(src);
    const int dstLen = WideCharToMultiByte(950, 0,
                                           unicode.data(), static_cast<int>(unicode.size()),
                                           NULL, 0, NULL, NULL);
    if (dstLen <= 0)
    {
        return std::string();
    }

    std::string result(dstLen, '\0');
    WideCharToMultiByte(950, 0,
                        unicode.data(), static_cast<int>(unicode.size()),
                        &result[0], dstLen, NULL, NULL);
    return result;
}

std::vector<std::string> tokenizeString(std::string theString, const std::string& separatorList)
{
    std::vector<std::string> parts;
    typedef boost::tokenizer< boost::char_separator<char> > tokenizer;

    boost::char_separator<char> sep(separatorList.c_str());
    tokenizer tokens(theString, sep);

    for (tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter)
    {
        parts.push_back(*tok_iter);
    }

    // if parts is empty, then this should return the entire string
    if (parts.size() == 0)
    {
        parts.push_back(theString);
    }

    return parts;
}

std::wstring utf8_to_utf16(const std::string& src)
{
    if (src.empty()) return std::wstring();
    int len = MultiByteToWideChar(CP_UTF8, 0, src.c_str(), -1, nullptr, 0);
    if (len <= 1) return std::wstring();
    std::wstring result(len - 1, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, src.c_str(), -1, &result[0], len);
    return result;
}

std::string join_4_languages(const std::string& english,
                             const std::string& chinese,
                             const std::string& malay,
                             const std::string& tamil,
                             const std::string& delimiter)
{
    return
        st2::as_string(st2::utf8_to_utf16le(english)) + delimiter +
        st2::as_string(st2::utf8_to_utf16le(chinese)) + delimiter +
        st2::as_string(st2::utf8_to_utf16le(malay)) + delimiter +
        st2::as_string(st2::utf8_to_utf16le(tamil));
}

std::vector<std::string>
split_to_4_languages(const std::string& str, const std::string delimiter)
{
    std::vector<std::string> vs;
    auto ws = st2::as_wstring(str);

#if 0
    std::wstringstream wss(ws);

    for (std::wstring line; std::getline(wss, line, L'\xFFFF');)
    {
        vs.emplace_back(st2::utf16le_to_utf8(line));
    }
#else
    auto begin = 0;
    auto pos = ws.find(L'\xFFFF');

    while (pos != std::wstring::npos)
    {
        auto line = ws.substr(begin, pos - begin);
        begin = pos + 1;
        vs.emplace_back(st2::utf16le_to_utf8(line));
        pos = ws.find(L'\xFFFF', begin);
    }

    if (begin)
    {
        auto line = ws.substr(begin);
        vs.emplace_back(st2::utf16le_to_utf8(line));
    }
#endif

    return vs;
}

int stoi_ex(const std::string& str)
{
    int ret = 0;

    try
    {
        ret = stoi(str);
    }
    catch (...) {}

    return ret;
}
