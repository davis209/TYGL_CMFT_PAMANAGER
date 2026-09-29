#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include <type_traits>
#include <tuple>

namespace TA_IRS_App::STIS_UTILITY::detail
{
    struct PID
    {
        std::string station;
        std::string asset;  // entity-name
        std::string name;
        std::string type;
        std::string id;
        std::string level;

        struct
        {
            bool has_occ_prefix = false;
            size_t entity = 0;
            size_t location = 0;
            size_t location_order = 0;
            size_t id = 0;
        } extra;

        friend bool operator==(const PID& lhs, const PID& rhs)
        {
            return lhs.extra.entity == rhs.extra.entity;
        }

        friend bool operator<(const PID& lhs, const PID& rhs)
        {
            return std::tie(lhs.extra.location_order, lhs.extra.id) < std::tie(rhs.extra.location_order, rhs.extra.id);
        }

        bool empty() const;
        bool has_value() const;
        bool is_occ() const;
        bool is_lcd() const;
        bool is_led() const;
        explicit operator bool() const;

        static const PID& from_entity_name(const std::string& entity_name);
        static const PID& from_entity_key(size_t entity_key);
        static const PID& from_station_and_id(const std::string& station, const std::string& id);
        static std::vector<PID> from_entity_keys(const std::vector<size_t>& entity_keys);
        static std::vector<PID> from_entity_names(const std::vector<std::string>& entity_names);
        static const PID& npid();

        static const std::vector<PID>& get_all();
        static const std::vector<PID>& get_all(size_t location);
        static const std::vector<PID>& get_all(const std::string& location);
        static const std::vector<PID>& get_all_occ(size_t location);
        static const std::vector<PID>& get_all_occ(const std::string& location);
        static const std::vector<PID>& get_all_station(size_t location);
        static const std::vector<PID>& get_all_station(const std::string& location);
        static const std::vector<PID>& get_all_stations();

        static const std::vector<std::string>& get_areas();
        static const std::vector<std::string>& get_areas(const std::string& location);
        static const std::vector<std::string>& get_areas(const std::vector<std::string>& locations);

        static std::vector<PID> get_all_copy();

        template <class T>
        static std::vector<PID> from_entity_keys(const std::vector<T>& entity_keys)
        {
            return from_entity_keys(std::vector<size_t>{entity_keys.begin(), entity_keys.end()});
        }
    };

    using PIDList = std::vector<PID>;

    struct Version
    {
        template <class T, class = std::enable_if_t<std::is_integral_v<T>>>
        Version(T ver)
            : version(std::to_string(ver))
        {
            normalize(version);
        }

        Version(std::string ver)
            : version(std::move(ver))
        {
            normalize(version);
        }

        operator std::string& ()
        {
            return version;
        }

        operator const std::string& () const
        {
            return version;
        }

        operator int() const
        {
            return std::stoi(version);
        }

        std::string& value()
        {
            return version;
        }

        const std::string& value() const
        {
            return version;
        }

        static std::string& normalize(std::string& version)
        {
            if (version.size() != 3)
            {
                std::string v(3, '0');

                if (3 < version.size())
                {
                    version = version.substr(0, 3);
                }

                std::copy_backward(version.begin(), version.end(), v.end());
                version = std::move(v);
            }

            return version;
        }

        static std::string normalize_copy(std::string version)
        {
            return normalize(version);
        }

        std::string version;
    };
}

namespace TA_IRS_App::STIS_UTILITY::detail
{
    std::string message_timestamp();
    int next_message_sequence();
    std::string join_4_languages(std::vector<std::string> languages, const std::string& delimiter = "\xFF\xFF");
    std::vector<std::string> split_to_4_languages(const std::string& str, const std::string delimiter = "\xFF\xFF");
    std::string join_4_languages_utf8(std::vector<std::string> languages, const std::string& delimiter = "\xFF\xFF");
    std::vector<unsigned char> join_4_languages_utf16(std::vector<std::vector<unsigned char>> languages, const std::string& delimiter = "\xFF\xFF");
    std::vector<std::string> split_to_4_languages_utf8(const std::string& str, const std::string delimiter = "\xFF\xFF");
    std::vector<std::vector<unsigned char>> split_to_4_languages_utf16(const std::vector<unsigned char>& utf16, const std::string delimiter = "\xFF\xFF");
    std::string join_4_utf16_languages_to_utf8(std::vector<std::vector<unsigned char>> languages, const std::string& delimiter = "\xFF\xFF");
    std::vector<unsigned char> transform_4_languages_from_utf8_to_utf16(const std::string& str, const std::string delimiter = "\xFF\xFF");
    std::string transform_4_languages_from_utf16_to_utf8(std::vector<unsigned char> languages, const std::string delimiter = "\xFF\xFF");
}

namespace TA_IRS_App::STIS_UTILITY
{
    using detail::PID;
    using detail::PIDList;
    using detail::Version;
    using detail::message_timestamp;
    using detail::next_message_sequence;
    using detail::join_4_languages;
    using detail::split_to_4_languages;
    using detail::join_4_languages_utf8;
    using detail::join_4_languages_utf16;
    using detail::split_to_4_languages_utf8;
    using detail::split_to_4_languages_utf16;
    using detail::join_4_utf16_languages_to_utf8;
    using detail::transform_4_languages_from_utf8_to_utf16;
    using detail::transform_4_languages_from_utf16_to_utf8;
}
