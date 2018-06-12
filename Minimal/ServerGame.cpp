#include "StdAfx.h"
#include "ServerGame.h"

unsigned int ServerGame::client_id; 

ServerGame::ServerGame(void)
{
    // id's to assign clients for our table
    client_id = 0;

    // set up the server network to listen 
    network = new ServerNetwork(); 
}

ServerGame::~ServerGame(void)
{
}

void ServerGame::update()
{
    // get new clients
   if(network->acceptNewClient(client_id))
   {
        printf("client %d has been connected to the server\n",client_id);

        client_id++;
   }

   receiveFromClients();
}

void ServerGame::receiveFromClients()
{

    Packet packet;

    // go through all clients
    std::map<unsigned int, SOCKET>::iterator iter;

    for(iter = network->sessions.begin(); iter != network->sessions.end(); iter++)
    {
        int data_length = network->receiveData(iter->first, network_data);

        if (data_length <= 0) 
        {
            //no data received
            continue;
        }

        int i = 0;
        while (i < (unsigned int)data_length) 
        {
            packet.deserialize(&(network_data[i]));
            i += sizeof(Packet);

            switch (packet.packet_type) {

                case INIT_CONNECTION:

                    printf("server received init packet from client\n");

                    sendActionPackets();

                    break;

                case ACTION_EVENT:

                    printf("server received action event packet from client\n");

                    sendActionPackets();

                    break;

				case HEAD_HAND_TRANSFORMS:
					printf("server received client's matrix transforms\n");
					// Populate transform matrices
					receivedHandTransform = packet.hand_transform;
					receivedHeadTransform = packet.head_transform;
					break;

				case TRANSFORMS_AND_INDICES:
					printf("Server is not supposed to receive index data!\n");
					break;

                default:

                    printf("error in packet types\n");

                    break;
            }
        }
    }
}


void ServerGame::sendActionPackets()
{
    // send action packet
    const unsigned int packet_size = sizeof(Packet);
    char packet_data[packet_size];

    Packet packet;
    packet.packet_type = ACTION_EVENT;

    packet.serialize(packet_data);

    network->sendToAll(packet_data,packet_size);
}

void ServerGame::sendPackets(glm::mat4 hand_transform, glm::mat4 head_transform, std::vector<unsigned int> path_inds) {
	// Get packet size
	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];
	
	// Send head and hand packets to clients
	Packet packet;
	packet.packet_type = TRANSFORMS_AND_INDICES;
	packet.hand_transform = hand_transform;
	packet.head_transform = head_transform;
	for (unsigned int i = 0; i < 4; i++) {
		packet.indices[i] = path_inds[i];
	}

	packet.serialize(packet_data);

	network->sendToAll(packet_data, packet_size);
}