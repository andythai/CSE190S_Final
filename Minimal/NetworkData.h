#pragma once
#include <string.h>

#define MAX_PACKET_SIZE 1000000

enum PacketTypes {
	// Send/Receive packets to indicate connection
	INIT_CONNECTION = 0,
	// Maybe change?
	ACTION_EVENT = 1,
	// Contains two mat4 data
	HEAD_AND_HAND_TRANSFORMS = 2,
	// Contains path indices
	PATH_INDICES = 3,

};

struct Packet {

    unsigned int packet_type;

    void serialize(char * data) {
        memcpy(data, this, sizeof(Packet));
    }

    void deserialize(char * data) {
        memcpy(this, data, sizeof(Packet));
    }
};