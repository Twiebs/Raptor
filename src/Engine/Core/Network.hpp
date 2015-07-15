#pragma once

#include <assert.h>
#include <Core/Common.hpp>
#include <SDL2/SDL_net.h>

#define NETWORK_SERVER_PORT 1234
#define NETWORK_PACKET_SIZE 512
#define NETWORK_HOSTNAME "localhost"
#define NETWORK_MAX_CLIENTS  4

enum class PacketType {
	CONNECT_REQUEST,	//Server side
	CONNECT_FAIL,
	DISCONECT_REQUEST,
	CONENCT,	//Notify the clients
	DISCONECT,
	CHAT_MESSAGE,
};

struct PacketHeader {
	PacketType type;
};

struct NetworkClient {
	IPaddress serverAddress;
	UDPsocket socket;
	UDPpacket* packet;
};

struct NetworkServer {
	U32 clientCount = 0;
	IPaddress clientAddresses[NETWORK_MAX_CLIENTS];
	IPaddress address;
	UDPsocket listenSocket;
	UDPpacket* sendPacket;
	UDPpacket* receivePacket;
};

bool NetworkInit(NetworkServer* server);
void NetworkTick(NetworkServer* server);
bool NetworkSend(NetworkServer* server, PacketType type, IPaddress address, void* data, size_t dataSize);
bool NetworkReceive(NetworkServer* server, void* data, size_t dataSize);

bool NetworkInit(NetworkClient* client);
//void NetworkTick(NetworkClient* client);
bool NetworkSend(NetworkClient* client, PacketType type, void* data, size_t dataSize);
bool NetworkReceive(NetworkClient* client, void* data, size_t dataSize);
