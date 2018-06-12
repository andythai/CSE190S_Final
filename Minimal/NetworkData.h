#pragma once
#include <string.h>

#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
// Use of degrees is deprecated. Use radians instead.
#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#define MAX_PACKET_SIZE 1000000

enum PacketTypes {
	// Send/Receive packets to indicate connection
	INIT_CONNECTION = 0,
	// Maybe change?
	ACTION_EVENT = 1,
	// Contains two mat4 data
	HEAD_HAND_TRANSFORMS = 2,
	// Contains path indices as well as two mat4 data
	TRANSFORMS_AND_INDICES = 3,

};

struct Packet {
    unsigned int packet_type;
	glm::mat4 hand_transform;
	glm::mat4 head_transform;
	unsigned int indices[4];

    void serialize(char * data) {
        memcpy(data, this, sizeof(Packet));
    }

    void deserialize(char * data) {
        memcpy(this, data, sizeof(Packet));
    }
};