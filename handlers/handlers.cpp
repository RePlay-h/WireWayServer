

#include "handlers.hpp"

void handlers::ConnectHandler(std::vector<Uchar8_t> &pkt, bool is_find) {

    pkt[0] = CONNACK_BYTE;

    pkt[1] = 0x00;
    pkt[2] = 0x00;
    pkt[3] = 0x00;
    pkt[4] = 0x00;

    if(is_find) {
        pkt[0] |= 0x01;
    }

}

 void handlers::InterceptHandler(std::vector<Uchar8_t> &pkt, bool is_find, std::string address) {

    size_t address_len = address.size();

    pkt[0] = INTERCAPT_BYTE;

    pkt[1] = (address_len >> 24);
    pkt[2] = (address_len >> 16);
    pkt[3] = (address_len >> 8);
    pkt[4] = address_len;

    // If the contact is not found, then add one bit to the beginning
    if(!is_find) {
        pkt[0] |= 0x01;
        pkt.resize(5);
        return;
    }

    address_len += 5;

    pkt.resize(address_len);

    address_len -= 5;

    for(size_t i = 0; i < address_len; ++i) {
        pkt[5+i] = address[i];
    }
}


void creators::CreatePublish(std::vector<Uchar8_t> &pkt, bool is_new_contact, Uchar16_t id) {

    pkt[0] = is_new_contact ? PUBLISH_BYTE : (PUBLISH_BYTE | 0x01);
    pkt[1] = 0x00;
    pkt[2] = 0x00;
    pkt[3] = 0x00;
    pkt[4] = 0x02;

    pkt[5] = (id >> 8);
    pkt[6] = id;

    pkt.resize(7);
}