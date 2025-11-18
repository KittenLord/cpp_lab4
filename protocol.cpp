#ifndef __PROTOCOL
#define __PROTOCOL

#include <stdint.h>

typedef uint8_t PacketMode;

#define PACKET_1 1
#define PACKET_2 2
#define PACKET_3 3

struct Packet1 {
    uint8_t mode; // ALWAYS 1
    uint32_t ramSize;
    uint32_t diskAvailable;
    uint32_t screenWidth;
};

struct Packet2 {
    uint8_t mode; // ALWAYS 2
    uint32_t lineHeight;
    uint32_t notifWidth;
    uint32_t dpiHorizontal;
};

struct Packet3 {
    uint8_t mode; // ALWAYS 3
    uint64_t length;
    // After sending Packet3, we MUST send `length` bytes
};

union PacketMax {
    Packet1 p1;
    Packet2 p2;
    Packet3 p3;
};

#endif // __PROTOCOL
