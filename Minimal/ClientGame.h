#pragma once
#include <winsock2.h>
#include <Windows.h>
#include "ClientNetwork.h"
#include "NetworkData.h"


class ClientGame
{
public:
	ClientGame(void);
	~ClientGame(void);

	ClientNetwork* network;

	/* Data the ClientGame receives from the server */
	glm::mat4 receivedHandTransform;
	glm::mat4 receivedHeadTransform;
	unsigned int receivedPathInds[4];

	// Check if player 1 has been found
	bool player1Found = false;

	void sendActionPackets();
	/* Send packet containing head and hand transformation data to server
	 * hand_transform - hand rotation and translation
	 * head_transform - head rotation and translation
	 */
	void sendPackets(glm::mat4 hand_transform, glm::mat4 head_transform);

    char network_data[MAX_PACKET_SIZE];

    void update();
};

