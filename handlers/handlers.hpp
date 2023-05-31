#ifndef WireWayServer__HANDLERS_H
#define WireWayServer__HANDLERS_H

#include "../utilities.hpp"

#include <vector>

/*
    The file contains functions for processing packets
*/

namespace handlers {

    void ConnectHandler(std::vector<Uchar8_t> &pkt, bool is_find);

    void InterceptHandler(std::vector<Uchar8_t> &pkt, bool is_find, std::string address = std::string());

} // handlers namespace

namespace creators {

    void CreatePublish(std::vector<Uchar8_t> &pkt, bool is_new_contact, Uchar16_t id);

} // creators namespace

#endif