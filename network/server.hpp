
#ifndef WireWayServer_NETWORK_SERVER_H
#define WireWayServer_NETWORK_SERVER_H

#include "user_session.hpp"
#include "contact_session.hpp"

#include <memory>
#include <chrono>

using namespace std::chrono_literals;

/*
    This file contains functions for the initial processing of the connected user. 
    And also there are functions for working with lists of sessions
*/

namespace network {

    asio::awaitable<void> Listener(tcp::acceptor acceptor); // add new contacts and users

    bool SearchInUsersData(const Uchar16_t &id) noexcept; // checks if there is a user with the specified id in the database

    bool SearchInContactsData(const Uchar16_t &id) noexcept; // checks if there is a contact with the specified id in the database

    void CreateConnack(std::vector<Uchar8_t> &pkt); // create connack packet

    std::shared_ptr<ContactSession> FindFreeContactSession() noexcept; // returns a pointer to a free session from the list of contacts, if any

    std::shared_ptr<UserSession> FindFreeUserSession() noexcept; // returns a pointer to a free session from the list of users, if any

    std::shared_ptr<ContactSession> FindContact(const Uchar16_t &id) noexcept; // search for a contact session with the specified id

    std::shared_ptr<UserSession> FindUser(const Uchar16_t &id) noexcept; // search for a user session with the specified id

    void SendToAllUsers(std::vector<Uchar8_t> &pkt);  // sends the specified packet to all users

} // network


#endif