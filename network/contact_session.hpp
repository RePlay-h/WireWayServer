
#ifndef WireWayServer_NETWORK_CONTACT_H
#define WireWayServer_NETWORK_CONTACT_H

#include "session.hpp"
#include "server.hpp"

class ContactSession : public Session {

public:
    ContactSession(tcp::socket sock, Uchar16_t id);

    void Start() override;

    asio::awaitable<void> Reader() override;

    asio::awaitable<void> Writer() override;

    void PacketHandler() override;

    void ClearSession() override;

    void TransferControl(tcp::socket sock, Uchar16_t id) override;

    bool SessionFree() override;

    const Uchar16_t& GetId() override;

    const tcp::endpoint GetEndpoint(); // return tcp::endpoint from sock_

    bool CheckErrorCode(boost::system::error_code ec, std::string &&msg_);


private:
    Uchar16_t id_;

    std::vector<Uchar8_t> buf_;
    tcp::socket sock_;
    asio::steady_timer timer_;
};

#endif