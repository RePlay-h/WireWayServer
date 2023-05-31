#ifndef WireWayServer_UTILITIES_H
#define WireWayServer_UTILITIES_H

#include <vector>
#include <memory>

#include "log/log.hpp"
#include "config.hpp"

#define CONNECT_BYTE 0x10
#define CONNACK_BYTE 0x20
#define INTERCEPT_BYTE 0x30
#define INTERCAPT_BYTE 0x40
#define SCREEN_BYTE 0x50
#define PUBLISH_BYTE 0x60
#define DISCONNECT_BYTE 0x70

#define MAX_COUNT_OF_SESSIONS 100

typedef unsigned char Uchar8_t;
typedef unsigned short int Uchar16_t;
typedef unsigned int Uchar32_t;

class UserSession;

class ContactSession;

namespace information_about {

    inline std::vector<std::shared_ptr<UserSession>> users_;

    inline std::vector<std::shared_ptr<ContactSession>> contacts_;

} // information_about

inline std::string ResolvePacketType(int type) {

    switch(type) {
        case CONNECT_BYTE:
            return "CONNECT";
        case CONNACK_BYTE:
            return "CONNACK";
        case INTERCEPT_BYTE:
            return "INTERCEPT";
        case INTERCAPT_BYTE:
            return "INTERCAPT";
        case SCREEN_BYTE:
            return "SCREEN";
        case DISCONNECT_BYTE:
            return "DISCONNECT";
        case PUBLISH_BYTE:
            return "PUBLISH";
    }
    return "";
}

#endif