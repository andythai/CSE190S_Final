#include "StdAfx.h"
#include "ClientGame.h"


ClientGame::ClientGame(void)
{

    network = new ClientNetwork();

    // send init packet
    const unsigned int packet_size = sizeof(Packet);
    char packet_data[packet_size];

    Packet packet;
    packet.packet_type = INIT_CONNECTION;

    packet.serialize(packet_data);

    NetworkServices::sendMessage(network->ConnectSocket, packet_data, packet_size);
}


ClientGame::~ClientGame(void)
{
}

void ClientGame::sendActionPackets()
{
    // send action packet
    const unsigned int packet_size = sizeof(Packet);
    char packet_data[packet_size];

    Packet packet;
    packet.packet_type = ACTION_EVENT;

    packet.serialize(packet_data);

    NetworkServices::sendMessage(network->ConnectSocket, packet_data, packet_size);
}

void ClientGame::sendPackets(glm::mat4 hand_transform, glm::mat4 head_transform) {
	// Initialize buffer
	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];

	// Fill data to send with player 2 data
	Packet packet;
	packet.packet_type = HEAD_HAND_TRANSFORMS;
	packet.hand_transform = hand_transform;
	packet.head_transform = head_transform;

	packet.serialize(packet_data);

	NetworkServices::sendMessage(network->ConnectSocket, packet_data, packet_size);
}

void ClientGame::update()
{
    Packet packet;
    int data_length = network->receivePackets(network_data);

    if (data_length <= 0) 
    {
        //no data recieved
        return;
    }

    int i = 0;
    while (i < (unsigned int)data_length) 
    {
        packet.deserialize(&(network_data[i]));
        i += sizeof(Packet);

        switch (packet.packet_type) {

            case ACTION_EVENT:

                printf("client received action event packet from server. Successful connection!\n");

				player1Found = true;
                //sendActionPackets();

                break;

			case HEAD_HAND_TRANSFORMS:
				printf("client received HEAD_HAND_TRANSFORMS only! This is incorrect!\n");
				break;

			case TRANSFORMS_AND_INDICES:
				printf("client received transforms and path index data from server\n");
				// Fill data
				receivedHandTransform = packet.hand_transform;
				receivedHeadTransform = packet.head_transform;
				for (unsigned int i = 0; i < 4; i++) {
					receivedPathInds[i] = packet.indices[i];
				}
				break;

            default:

                printf("error in packet types\n");

                break;
        }
    }
}
