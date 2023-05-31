
#ifndef WireWayServer_NETWORK_USER_H
#define WireWayServer_NETWORK_USER_H

#include "session.hpp"
#include "server.hpp"

class UserSession : public Session {

public:

    UserSession(tcp::socket sock, Uchar16_t id);

    void Start() override;

    asio::awaitable<void> Reader() override;

    asio::awaitable<void> Writer() override;

    void PacketHandler() override;

    void ClearSession() override;

    void TransferControl(tcp::socket sock, Uchar16_t id) override;

    bool SessionFree() override;

    const Uchar16_t& GetId() override;

    void AddPacket(std::vector<Uchar8_t> pkt);

    bool CheckErrorCode(boost::system::error_code ec, std::string &&msg_); // method throws an error message



private:

    Uchar16_t id_;

    std::vector<Uchar8_t> buf_;
    std::queue<std::vector<Uchar8_t>> packets_;
    tcp::socket sock_;
    asio::steady_timer timer_;
};

#endif