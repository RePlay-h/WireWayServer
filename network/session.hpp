#ifndef WireWayServer_NETWORK_SESSION_H
#define WireWayServer_NETWORK_SESSION_H

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/redirect_error.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/use_future.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>

#include <boost/system/error_code.hpp>

#include "../handlers/handlers.hpp"
#include "../log/log.hpp"

#include <queue>
#include <future>
#include <algorithm>

using namespace boost;
using asio::ip::tcp;

/*
    This file contains the interface for sessions
    and the core files of the boost.asio library are included
*/

class Session {
public:
    virtual asio::awaitable<void> Reader() = 0;

    virtual asio::awaitable<void> Writer() = 0;

    virtual void PacketHandler() = 0;

    virtual void Start() = 0;

    virtual void ClearSession() = 0; // when a user/contact disconnects from the server, its session is released

    virtual void TransferControl(tcp::socket sock, Uchar16_t id) = 0; // transfer control of the current session

    virtual bool SessionFree() = 0; // is the session free

    virtual const Uchar16_t& GetId() = 0;
};

#endif