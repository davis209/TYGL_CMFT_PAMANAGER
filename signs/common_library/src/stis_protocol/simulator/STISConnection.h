#pragma once
#include "STISStatus.h"
#include "StationManager.h"
#include "core/sockets/src/ITcpSocketObserver.h"
#include "core/sockets/src/TcpObservedSocket.h"
#include "core/sockets/src/TcpNonblockingSocket.h"
#include "app/signs/common_library/src/stis_protocol/message_types/MessageDataTypes.h"
#include "core/utility/src/core/SimpleConditionVariable.h"

namespace TA_IRS_App::STIS_PROTOCOL::SIMULATOR::stissimulator::detail
{
    using namespace TA_Base_Core;
    using namespace STIS_PROTOCOL::MESSAGE_DATA_TYPES;
    using Blob = std::vector<unsigned char>;
    using stdex::SimpleConditionVariable;
    using SocketClient = TcpObservedSocket<TcpNonblockingSocket>;
    using SocketClientPtr = std::shared_ptr<SocketClient>;

    struct STISConnection : ITcpSocketObserver
    {
        STISConnection(SocketClient* socket, STISStatusPtr status, StationManagerPtr station_manager);

        ~STISConnection();

        void start();
        void stop();

        void send_response(GENERALPACKET a);
        void dispatch_message(const GENERALPACKET& m);
        void on_M10(const M10& m);
        void on_M11(const M11& m);
        void on_M20(const M20& m);
        void on_M21(const M21& m);
        void on_M22(const M22& m);
        void on_M23(const M23& m);
        void on_M24(const M24& m);
        void on_M30(const M30& m);
        void on_M31(const M31& m);
        void on_M32(const M32& m);
        void on_M50(const M50& m);
        void on_M70(const M70& m);

        virtual void processReceivedData(Blob& data, ISocket*) override;
        virtual void writeFailed() override;
        virtual bool connectionLost() override;
        virtual void connectionEstablished() override;

        static void enable_output(bool b = true);

        virtual GENERALPACKET& pre_send_response(GENERALPACKET& a);
        virtual GENERALPACKET& post_recieve_message(GENERALPACKET& a);

        Blob m_buffer;
        STISStatusPtr m_status;
        StationManagerPtr m_station_manager;
        SocketClientPtr m_socket;
        SimpleConditionVariable m_connected;
    };

    using STISConnectionPtr = std::shared_ptr<STISConnection>;
    using STISConnectionWeakPtr = std::weak_ptr<STISConnection>;
    using STISConnectionWeakPtrList = std::vector<STISConnectionWeakPtr>;
}
