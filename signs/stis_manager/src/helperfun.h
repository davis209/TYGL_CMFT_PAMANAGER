#pragma once
#include <string>
#include <vector>

std::wstring convertUtf8ToUtf16le(const std::string& src);
std::string convertUtf16leToUtf8(const std::wstring& src);
std::vector<std::string> tokenizeString(std::string theString, const std::string& separatorList);

std::string join_4_languages(const std::string& english,
                             const std::string& chinese,
                             const std::string& malay,
                             const std::string& tamil,
                             const std::string& delimiter = "\xFF\xFF");

std::vector<std::string> split_to_4_languages(const std::string& str, const std::string delimiter = "\xFF\xFF");

int stoi_ex(const std::string& str);
