#pragma once

#include <assert.h>
#include <Core/Common.hpp>
#include <SDL2/SDL_net.h>

#define NETWORK_SERVER_PORT 1234
#define NETWORK_PACKET_SIZE 512
#define NETWORK_HOSTNAME "localhost"

struct NetworkClient {
	IPaddress serverAddress;
	UDPsocket socket;
	UDPpacket* packet;
};

struct NetworkServer {
	IPaddress address;
	UDPsocket listenSocket;
	UDPpacket* packet;
};

bool NetworkInit(NetworkServer* server);
void NetworkTick(NetworkServer* server);
bool NetworkSend(NetworkServer* server, void* data, size_t dataSize);
bool NetworkReceive(NetworkServer* server, void* data, size_t dataSize);

bool NetworkInit(NetworkClient* client);
void NetworkTick(NetworkClient* client);
bool NetworkSend(NetworkClient* client, void* data, size_t dataSize);
bool NetworkReceive(NetworkClient* client, void* data, size_t dataSize);
