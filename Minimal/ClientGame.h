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

	void sendActionPackets();
	/* Send packet of data to server
	 * packet_type - type of packet to send 
	 */
	void sendPackets(unsigned int packet_type);

    char network_data[MAX_PACKET_SIZE];

    void update();
};

