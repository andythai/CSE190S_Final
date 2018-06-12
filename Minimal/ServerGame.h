#pragma once
#include "ServerNetwork.h"
#include "NetworkData.h"

class ServerGame
{

public:
	/* Data for the server game to keep track of from client */
	glm::mat4 receivedHandTransform;
	glm::mat4 receivedHeadTransform;

    ServerGame(void);
    ~ServerGame(void);

    void update();

	void receiveFromClients();

	void sendActionPackets();

	/* Send player 2 and enemies' location data to clients
	 * hand_transform - hand transformation matrix
	 * head_transform - head transformation matrix
	 * path_inds - contains all 4 path indices of each enemy
	 */
	void sendPackets(glm::mat4 hand_transform, glm::mat4 head_transform, std::vector<unsigned int> path_inds);

private:

   // IDs for the clients connecting for table in ServerNetwork 
    static unsigned int client_id;

   // The ServerNetwork object 
    ServerNetwork* network;

	// data buffer
   char network_data[MAX_PACKET_SIZE];
};