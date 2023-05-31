
#include "user_session.hpp"

UserSession::UserSession(tcp::socket sock, Uchar16_t id) : sock_(std::move(sock)), timer_(sock_.get_executor()), id_(std::move(id)) {

    timer_.expires_at(asio::steady_timer::time_point::max());
}

void UserSession::Start() {

    std::string msg = "New USER started working  [ID: ";
    msg += std::to_string(id_);
    msg += "] [ADDRESS: ";
    msg += sock_.remote_endpoint().address().to_string();
    msg += "] [PORT: ";
    msg += std::to_string(sock_.remote_endpoint().port());
    msg += "]";

    Log::Print(INFO,  msg, std::cout);

    boost::system::error_code ec;

    asio::co_spawn(sock_.get_executor(), Reader(), asio::detached);

    asio::co_spawn(sock_.get_executor(), Writer(), asio::detached);
}

asio::awaitable<void> UserSession::Reader() {

    boost::system::error_code ec;

    for(;;) {

        buf_.resize(5);
        
        // get header of packet
        co_await sock_.async_receive(asio::buffer(buf_, 5), asio::redirect_error(asio::use_awaitable, ec));

        if(CheckErrorCode(ec, "[" + std::to_string(id_) + "] an error occurred while receiving the package")) {

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

        if(remaining_len != 0) {

            buf_.resize(remaining_len + 5);

            // get remaining part
            co_await sock_.async_receive(asio::buffer(buf_.data() + 5, std::move(remaining_len)), asio::redirect_error(asio::use_awaitable, ec));

            if(CheckErrorCode(ec, "[" + std::to_string(id_) + "] an error occurred while receiving the package")) {

                ClearSession();

                if constexpr (PRINT_ERROR) {
                    Log::Print(ERROR, ec.what(), std::cout);
                }

                co_return;
            }

        }

        if constexpr (PRINT_TYPE_OF_RECEIVED_USER_PACKET) {
            if((buf_[0] & 0xF0) != SCREEN_BYTE) {
                Log::Print(INFO, "USER: " + std::to_string(id_) + " get packet: " + ResolvePacketType(buf_[0] & 0xF0), std::cout);
            }
        }

        PacketHandler();

        buf_.clear();       
    }

}

void UserSession::PacketHandler() {
    switch (buf_[0] & 0xF0)
    {
    case INTERCEPT_BYTE: {

        Uchar16_t id = ((buf_[5] << 8) | buf_[6]);

        std::shared_ptr<ContactSession> session = network::FindContact(id);

        // If a contact with the specified id is not found, then create an intercapt packet with 1 in the least significant bit
        if(session == nullptr) {
            handlers::InterceptHandler(buf_, false);
        } else {
            tcp::endpoint endp = session->GetEndpoint();
            handlers::InterceptHandler(buf_, true, endp.address().to_string());
        }

        packets_.push(std::move(buf_));

        break;
    }

    case DISCONNECT_BYTE: {
        ClearSession();
    }
    }
}

asio::awaitable<void> UserSession::Writer() {

    boost::system::error_code ec;

    for(;;) {
        if(packets_.empty()) {
            system::error_code ec;
            co_await timer_.async_wait(asio::redirect_error(asio::use_awaitable, ec));
        }
        else {
            while(!packets_.empty()) {
                
                //send packet
                co_await sock_.async_send(asio::buffer(packets_.front(), packets_.front().size()), asio::use_awaitable);

                if(CheckErrorCode(ec, "[" + std::to_string(id_) + "] an error occurred while sending the package")) {

                    ClearSession();

                    if constexpr (PRINT_ERROR) {
                        Log::Print(ERROR, ec.what(), std::cout);
                    }

                    co_return;
                }

                if constexpr (PRINT_TYPE_OF_SENT_USER_PACKET) {
                    Log::Print(INFO, "USER: " + std::to_string(id_) + " send packet: " + ResolvePacketType(packets_.front()[0] & 0xF0), std::cout);
                }
                packets_.pop();
            }
        }
    }

 }

void UserSession::ClearSession() {

    if(sock_.is_open()) {
        
        // clear all session variables

        buf_.clear();
    
        while(!packets_.empty()) {
            packets_.pop();
        }

        sock_.shutdown(asio::socket_base::shutdown_both);

        sock_.close();

        timer_.cancel();
    }
 }

 void UserSession::TransferControl(tcp::socket sock, Uchar16_t id) {
 
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

 bool UserSession::SessionFree() {
    return !sock_.is_open();
 }

void UserSession::AddPacket(std::vector<Uchar8_t> pkt) {
    packets_.push(std::move(pkt));
    timer_.cancel_one();
}

 bool UserSession::CheckErrorCode(boost::system::error_code ec, std::string &&msg_) {

    if(ec != boost::system::errc::success) {
            std::string msg = "[USER][ID: ";
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

 const Uchar16_t& UserSession::GetId() {
    return id_;
 }