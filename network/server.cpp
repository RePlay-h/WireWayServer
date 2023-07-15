
#include "server.hpp"

asio::awaitable<void> network::Listener(tcp::acceptor acceptor) {

    for(;;) {

        std::vector<Uchar8_t> pkt(7);

        auto sock = co_await acceptor.async_accept(asio::use_awaitable);

        // get connect packet
        co_await sock.async_receive(asio::buffer(pkt, 7), asio::use_awaitable);

        bool is_contact = pkt[0] & 0x01;
        Uchar16_t id = ((pkt[5] << 8) | pkt[6]);

        CreateConnack(pkt);

        // send header of connack packet
        co_await sock.async_send(asio::buffer(pkt, pkt.size()), asio::use_awaitable);

        // send remaining part of connack packet
        //co_await sock.async_send(asio::buffer(pkt.data() + 5, pkt.size() - 5), asio::use_awaitable);

        if(is_contact) {
            /*
                If it is a contact, we should send a PUBLISH packet to all users, 
                which indicates that we need to add a new contact to the list
            */

            pkt.clear();
            pkt.resize(7);

            creators::CreatePublish(pkt, true, id);

            network::SendToAllUsers(pkt);

            std::shared_ptr<ContactSession> ptr = FindFreeContactSession();

            
            //If a free session is not found, then we create a new one, 
            //else we simply transfer control to the existing session
            if(ptr == nullptr) {

                information_about::contacts_.push_back(std::make_shared<ContactSession>(std::move(sock), id));
                information_about::contacts_.back()->Start();
                

            } else {
                ptr->TransferControl(std::move(sock), std::move(id));
            }


        } else {

            std::shared_ptr<UserSession> ptr = FindFreeUserSession();

            //If a free session is not found, then we create a new one, 
            //else we simply transfer control to the existing session
            if(ptr == nullptr) {

                information_about::users_.push_back(std::make_shared<UserSession>(std::move(sock), id));

                information_about::users_.back()->Start();

            } else {
                ptr->TransferControl(std::move(sock), std::move(id));
            }
            
        }

        pkt.clear();
    }

}


void network::CreateConnack(std::vector<Uchar8_t> &pkt) {

    Uchar16_t id = ((pkt[5] << 8) | pkt[6]);

    bool is_find;
    bool is_contact = pkt[0] & 0x0F;

    


    // Check id in database
    if(pkt[0] & 0x01) {
        is_find = SearchInContactsData(id);
    } else {
        is_find = SearchInUsersData(id);
    }

    if(id == 0) {
        is_find = true;
    }

    // Preliminary handing of packet
    handlers::ConnectHandler(pkt, is_find);


    // If Id was find in database or it's contact, that's all
    if(is_find || is_contact) {
        pkt.resize(5);
        return;
    } 

    // If it's user we should add all ids in packet
    size_t size_of_contacts_data = information_about::contacts_.size();

    size_of_contacts_data *= 2;
    
    pkt.resize(5);

    size_t count_of_free = 0;

    // Iterate over the data and add new id in packet
    for(size_t i = 0; i < size_of_contacts_data / 2; ++i) {

    
        auto con = (information_about::contacts_.begin() + i)->get();

        if(con->SessionFree()) { 
            count_of_free += 2;
            continue; 
        }

        Uchar16_t id = (information_about::contacts_.begin() + i)->get()->GetId();

        pkt.push_back((id >> 8));
        pkt.push_back(id);

    }

    size_of_contacts_data -= count_of_free;

    pkt[1] = (size_of_contacts_data >> 24);
    pkt[2] = (size_of_contacts_data >> 16);
    pkt[3] = (size_of_contacts_data >> 8);
    pkt[4] = size_of_contacts_data;

    pkt.resize(5 + size_of_contacts_data*2);
}

bool network::SearchInUsersData(const Uchar16_t &id) noexcept {

   for(auto& session : information_about::users_) {
        if(session->GetId() == id) {
            return true;
        }
   }
   return false;

}

bool network::SearchInContactsData(const Uchar16_t &id) noexcept {

    for(auto& session : information_about::contacts_) {
        if(session->GetId() == id) {
            return true;
        }
   }
   return false;

}

std::shared_ptr<ContactSession> network::FindFreeContactSession() noexcept {

    for(auto& session : information_about::contacts_) {

        if(session->SessionFree()) {
            return session;
        }
    }

    return nullptr;
}

std::shared_ptr<UserSession> network::FindFreeUserSession() noexcept {

    for(auto& session : information_about::users_) {

        if(session->SessionFree()) {
            return session;
        }
    }
    
    return nullptr;
}

std::shared_ptr<ContactSession> network::FindContact(const Uchar16_t &id) noexcept {

    for(auto &a : information_about::contacts_) {
        if(a->GetId() == id) {
            return a;
        }
    }

    return nullptr;

}

std::shared_ptr<UserSession> network::FindUser(const Uchar16_t &id) noexcept {

    for(auto &a : information_about::users_) {
        if(a->GetId() == id) {
            return a;
        }
    }

    return nullptr;
}

void network::SendToAllUsers(std::vector<Uchar8_t> pkt) {

    for(auto& session: information_about::users_) {

        if(!session->SessionFree())
            session->AddPacket(pkt);

        
    }

}