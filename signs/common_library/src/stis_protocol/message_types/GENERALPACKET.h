#pragma once
#include "core/utility/src/core/fixed_length_data/all.h"

namespace TA_IRS_App::STIS_PROTOCOL::MESSAGE_DATA_TYPES
{
    using namespace st::fixed_length_data;

    struct GENERICPACKET_HEADER
    {
        Byte            STX{0x02};
        Integer<4>      SQN;
        String<14>      Timestamp;
        String<3>       MessageID;

        template <class T>
        bool is_message() const
        {
            return this->MessageID == T::ID;
        }

        template <class Iterator>
        GENERICPACKET_HEADER(Iterator beg, Iterator end)
        {
            std::stringstream ss(std::string(beg, end));
            ss >> STX >> SQN >> Timestamp >> MessageID;
        }
    };

    template <size_t DATA_LENGTH_SIZE>
    struct BASIC_GENERALPACKET : EnableFixedLengthUtility<BASIC_GENERALPACKET<DATA_LENGTH_SIZE>>
    {
        using DATA = Text<Integer<DATA_LENGTH_SIZE>>;

        Byte            STX{0x02};
        Integer<4>      SQN;
        String<14>      Timestamp;
        String<3>       MessageID;
        // Integer<4>   DataLength;
        DATA            Data;
        Byte            ETX{0x03};

        auto tied() noexcept
        {
            return std::tie
            (
                STX,
                SQN,
                Timestamp,
                MessageID,
                // DataLength,
                Data,
                ETX
            );
        }

        auto tied() const noexcept
        {
            return const_cast<BASIC_GENERALPACKET*>(this)->tied();
        }

        size_t datasize() const
        {
            Data.size();
        }

        BASIC_GENERALPACKET() = default;

        template <class Char>
        BASIC_GENERALPACKET(const std::vector<Char>& data)
        {
            std::stringstream ss(data.begin(), data.end());
            ss >> *this;
        }

        BASIC_GENERALPACKET(const std::string& str)
        {
            std::stringstream ss(str);
            ss >> *this;
        }

        template <class Iterator>
        BASIC_GENERALPACKET(Iterator beg, Iterator end)
        {
            std::stringstream ss(std::string(beg, end));
            ss >> *this;
        }

        template <class T>
        bool is_message() const
        {
            return this->MessageID == T::ID;
        }
    };

    using GENERALPACKET = BASIC_GENERALPACKET<4>;
    using GENERALPACKET_4 = BASIC_GENERALPACKET<4>;
    using GENERALPACKET_6 = BASIC_GENERALPACKET<6>;

    template <class T, size_t DATA_LENGTH_SIZE>
    struct EnableOperatorBasicGeneralPacket
    {
        operator BASIC_GENERALPACKET<DATA_LENGTH_SIZE>() const noexcept
        {
            std::stringstream ss;
            ss << static_cast<const T&>(*this);
            auto packet = std::make_shared<BASIC_GENERALPACKET<DATA_LENGTH_SIZE>>();
            ss >> *packet;
            return *packet;
        }
    };

    template <class T>
    using EnableOperatorGeneralPacket = EnableOperatorBasicGeneralPacket<T, 4>;

    template <class T>
    using EnableOperatorGeneralPacket4 = EnableOperatorBasicGeneralPacket<T, 4>;

    template <class T>
    using EnableOperatorGeneralPacket6 = EnableOperatorBasicGeneralPacket<T, 6>;

    template <size_t DATA_LENGTH_SIZE>
    using BASIC_GENERALPACKET_PTR = std::shared_ptr<BASIC_GENERALPACKET<DATA_LENGTH_SIZE>>;

    using GENERALPACKET_PTR = BASIC_GENERALPACKET_PTR<4>;
    using GENERALPACKET_4_PTR = BASIC_GENERALPACKET_PTR<4>;
    using GENERALPACKET_6_PTR = BASIC_GENERALPACKET_PTR<6>;
}
