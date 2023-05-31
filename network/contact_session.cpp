#include "contact_session.hpp"

ContactSession::ContactSession(tcp::socket sock, Uchar16_t id) : sock_(std::move(sock)), timer_(sock_.get_executor()), id_(std::move(id)) {

        timer_.expires_at(asio::steady_timer::time_point::max());
}

void ContactSession::Start() {

    std::string msg = "New contact started working  [ID: ";
    msg += std::to_string(id_);
    msg += "] [ADDRESS: ";
    msg += sock_.remote_endpoint().address().to_string();
    msg += "] [PORT: ";
    msg += std::to_string(sock_.remote_endpoint().port());
    msg += "]";

    Log::Print(INFO,  msg, std::cout);

    asio::co_spawn(sock_.get_executor(), Reader(), asio::detached);

    asio::co_spawn(sock_.get_executor(), Writer(), asio::detached);;
}

asio::awaitable<void> ContactSession::Reader() {

    boost::system::error_code ec;

    for(;;) {

        buf_.resize(5);

        // get header of packet
        co_await sock_.async_receive(asio::buffer(buf_, 5), asio::redirect_error(asio::use_awaitable, ec));

        if(CheckErrorCode(ec, "An error occurred while receiving the package")) {

            ClearSession();

            if constexpr (PRINT_ERROR) {
                Log::Print(ERROR, ec.what(), std::cout);
            }

            co_return;
        }

        // size of remaining part
        Uchar32_t remaining_len = (buf_[1] << 24);
        remaining_len |= (buf_[2] << 16);
        remaining_len |= (buf_[3] << 8);
        remaining_len |= (buf_[4]);

        if constexpr (PRINT_TYPE_OF_RECEIVED_CONTACT_PACKET) {
            Log::Print(INFO, "CONTACT: " + std::to_string(id_) + " get packet: " + ResolvePacketType(buf_[0] & 0xF0), std::cout);
        }

        if(remaining_len != 0) {
            buf_.resize(remaining_len + 5);

            // get remaining packet
            co_await sock_.async_receive(asio::buffer(buf_.data() + 5, remaining_len), asio::redirect_error(asio::use_awaitable, ec));

            if(CheckErrorCode(ec, "An error occurred while receiving the package")) {

                ClearSession();

                if constexpr (PRINT_ERROR) {
                    Log::Print(ERROR, ec.what(), std::cout);
                }

                co_return;
            }

        }

        PacketHandler();
    }

}

void ContactSession::PacketHandler() {
    switch (buf_[0] & 0xF0)
    {
    case SCREEN_BYTE:
        network::SendToAllUsers(buf_);
    }
}

asio::awaitable<void> ContactSession::Writer() {

    boost::system::error_code ec;

    for(;;) {
        if(buf_.empty() || buf_[0] == 0) {
            system::error_code ec;
            co_await timer_.async_wait(asio::redirect_error(asio::use_awaitable, ec));
        }
        else {

            // send packet
            co_await sock_.async_send(asio::buffer(buf_, buf_.size()), asio::redirect_error(asio::use_awaitable, ec));


            if(CheckErrorCode(ec, "An error occurred while sending the package")) {

                ClearSession();

                if constexpr (PRINT_ERROR) {
                    Log::Print(ERROR, ec.what(), std::cout);
                }

                co_return;
            }

            if constexpr (PRINT_TYPE_OF_SENT_CONTACT_PACKET) {
                Log::Print(INFO, "CONTACT: " + std::to_string(id_) + " send packet: "+ ResolvePacketType(buf_[0] & 0xF0), std::cout);
            }

            buf_.clear();
        }
    }

 }

 void ContactSession::ClearSession() {

    if(sock_.is_open()){

        // clear all session variables

        buf_.clear();

        sock_.shutdown(asio::socket_base::shutdown_both);

        sock_.close();

        timer_.cancel();

        std::vector<Uchar8_t> pkt(7);

        creators::CreatePublish(pkt, false, id_);

        network::SendToAllUsers(pkt);
    }

 }

 void ContactSession::TransferControl(tcp::socket sock, Uchar16_t id) {

    // set new socket
    sock_ = std::move(sock);

    // set new id
    id_ = std::move(id);

    // timer initialization
    timer_.expires_at(std::chrono::steady_clock::time_point::max());

    // spawn corotines
    asio::co_spawn(sock_.get_executor(), Reader(), asio::detached);

    asio::co_spawn(sock_.get_executor(), Writer(), asio::detached);

 }

 bool ContactSession::SessionFree() {
    return !sock_.is_open();
 }


 bool ContactSession::CheckErrorCode(boost::system::error_code ec, std::string &&msg_) {

    if(ec != boost::system::errc::success) {
            std::string msg = "[CONTACT][ID: ";
            msg += std::to_string(id_);
            msg += "] [ADDRESS: ";
            msg += sock_.remote_endpoint().address().to_string();
            msg += "] [PORT: ";
            msg += std::to_string(sock_.remote_endpoint().port());
            msg +=  "] ";

            msg += msg_;

            Log::Print(ERROR,  msg, std::cout);
            
            return true;
    }

    return false;
 }

const Uchar16_t& ContactSession::GetId() {
    return id_;
 }

 const tcp::endpoint ContactSession::GetEndpoint() {
    return sock_.remote_endpoint();
 }