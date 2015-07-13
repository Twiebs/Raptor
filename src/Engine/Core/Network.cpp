#include "Network.hpp"


bool NetworkInit(NetworkServer* server) {
	if (SDL_Init(0) == -1) {
		LOG_ERROR("SDL: Failed to Init -> " << SDL_GetError());
		return false;
	}
	if (SDLNet_Init() == -1) {
		LOG_ERROR("SDLNet failed to Init: " << SDLNet_GetError());
		return false;
	}
	server->listenSocket = SDLNet_UDP_Open(NETWORK_SERVER_PORT);
	if (server->listenSocket == NULL) {
		LOG_ERROR("SDLNet: Failed to Open Socket -> " << SDLNet_GetError());
		return false;
	}
	server->packet = SDLNet_AllocPacket(512);
	if (!server->packet) {
		LOG_ERROR("SDLNet: Failed to allocate a packet -> " << SDLNet_GetError());
		return false;
	}

	auto hostResolved = SDLNet_ResolveHost(&server->address, NULL, 1234);
	if (hostResolved == -1) {
		LOG_ERROR("Failed to resolve Host" << SDLNet_GetError());
		return false;
	}

	LOG_INFO("Server is initalized!");
	LOG_INFO("IP: " << server->address.host);
	LOG_INFO("Port: " << server->address.port);
}

bool NetworkInit(NetworkClient* client) {
	if (SDL_Init(0) == -1) {
		LOG_ERROR("SDL: Failed to Init -> " << SDL_GetError());
		return false;
	}
	if (SDLNet_Init() == -1) {
		LOG_ERROR("SDLNet failed to Init: " << SDLNet_GetError());
		return false;
	}
	client->socket = SDLNet_UDP_Open(0);
	if (client->socket == NULL) {
		LOG_ERROR("SDLNet: Failed to Open Socket -> " << SDLNet_GetError());
		return false;
	}
	client->packet = SDLNet_AllocPacket(512);
	if (!client->packet) {
		LOG_ERROR("SDLNet: Failed to allocate a packet -> " << SDLNet_GetError());
		return false;
	}

	auto hostResolved = SDLNet_ResolveHost(&client->serverAddress, "localhost", 1234);
	if (hostResolved == -1) {
		LOG_ERROR("Failed to resolve Host" << SDLNet_GetError());
		return false;
	}

	LOG_INFO("Client is initalized!");
	LOG_INFO("Server IP: " << client->serverAddress.host);
	LOG_INFO("Server Port: " << client->serverAddress.port);
}


void NetworkTick(NetworkServer* server) {
	while (SDLNet_UDP_Recv(server->listenSocket, server->packet)) {
		LOG_INFO("WE got a packet!");
		LOG_INFO("Thank you kind sir!");
		server->packet->channel = -1;
		server->packet->len = 0;
		server->packet->status = 1;
		SDLNet_UDP_Send(server->listenSocket, -1, server->packet);
	}
}

void NetworkTick(NetworkClient* client) {
	const char text[30] = "This is a line of text!";
	NetworkSend(client, (void*)text, 30);
}

bool NetworkSend(NetworkClient* client, void* data, size_t dataSize) {
	assert(dataSize < client->packet->maxlen);
	auto sendPacket = client->packet;
	sendPacket->address = client->serverAddress;
	sendPacket->channel = -1;
	sendPacket->len = dataSize;
	sendPacket->status = 0;
	memcpy(client->packet->data, data, dataSize);
	if (SDLNet_UDP_Send(client->socket, -1, client->packet) == 0) {
		LOG_ERROR("SDLNet: Could not send packet across soccket - " << SDLNet_GetError());
		return false;
	}
	return true;
}

//bool NetworkReceive(NetworkContext* context, void* data, size_t dataSize) {
//	auto sucuess = SDLNet_UDP_Recv(context->socket, context->receivePacket);
//	if (!sucuess) return false;
//	memcpy(data, context->receivePacket->data, context->receivePacket->len);
//	return true;
//}
