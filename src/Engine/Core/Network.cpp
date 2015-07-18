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
	server->sendPacket = SDLNet_AllocPacket(NETWORK_PACKET_SIZE);
	if (!server->sendPacket) {
		LOG_ERROR("SDLNet: Failed to allocate send packet -> " << SDLNet_GetError());
		return false;
	}	
	server->receivePacket = SDLNet_AllocPacket(NETWORK_PACKET_SIZE);
	if (!server->receivePacket) {
		LOG_ERROR("SDLNet: Failed to allocate recieve packet -> " << SDLNet_GetError());
		return false;
	}

	auto hostResolved = SDLNet_ResolveHost(&server->address, NULL, NETWORK_SERVER_PORT);
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
	LOG_INFO("Connecting to server...");
	NetworkSend(client, PacketType::CONNECT_REQUEST, nullptr, 0);
}

bool NetworkSend(NetworkServer* server, PacketType type, IPaddress address, void* data, size_t dataSize) {
	ASSERT((dataSize + sizeof(PacketHeader)) < server->sendPacket->maxlen);
	auto sendPacket = server->sendPacket;
	sendPacket->address = address;
	sendPacket->channel = -1;
	sendPacket->len = sizeof(PacketHeader) + dataSize;
	sendPacket->status = 0;
	PacketHeader header = { type };
	memcpy(sendPacket->data, &header, sizeof(PacketHeader));
	memcpy(sizeof(PacketHeader) + sendPacket->data, data, dataSize);
	if (SDLNet_UDP_Send(server->listenSocket, -1, sendPacket) == 0) {
		LOG_ERROR("SDLNet: Could not send packet across soccket - " << SDLNet_GetError());
		return false;
	}
	ASSERT(sendPacket->status = (sizeof(PacketHeader) + dataSize));
	return true;
}

void NetworkTick(NetworkServer* server) {
	while (SDLNet_UDP_Recv(server->listenSocket, server->receivePacket)) {
		auto recievedPacket = server->receivePacket;
		auto header = (PacketHeader*)recievedPacket->data;
		switch (header->type) {
		case PacketType::CONNECT_REQUEST:
			if (server->clientCount == NETWORK_MAX_CLIENTS) {
				NetworkSend(server, PacketType::CONNECT_FAIL, recievedPacket->address, 0, 0);
			}
			server->clientAddresses[server->clientCount++] = recievedPacket->address;
			break;
		}
		//The packet is fowared to the clients
		for (auto i = 0; i < server->clientCount; i++) {
			if (server->clientAddresses[i].host == recievedPacket->address.host &&
				server->clientAddresses[i].port == recievedPacket->address.port) {
				continue;	//Dont send the recived packet pack to the client that it orginated from
			}
			NetworkSend(server, header->type, server->clientAddresses[i], (server->receivePacket->data + sizeof(PacketHeader)), server->receivePacket->len - sizeof(PacketHeader));
		}
	}
}


bool NetworkSend(NetworkClient* client, PacketType type, void* data, size_t dataSize) {
	ASSERT((dataSize + sizeof(PacketHeader)) < client->packet->maxlen);
	auto sendPacket = client->packet;
	sendPacket->address = client->serverAddress;
	sendPacket->channel = -1;
	sendPacket->len = sizeof(PacketHeader) + dataSize;
	sendPacket->status = 0;
	PacketHeader header{ type };
	memcpy(sendPacket->data, &header, sizeof(PacketHeader));
	memcpy((sendPacket->data + sizeof(PacketHeader)), data, dataSize);
	if (SDLNet_UDP_Send(client->socket, -1, client->packet) == 0) {
		LOG_ERROR("SDLNet: Could not send packet across soccket - " << SDLNet_GetError());
		return false;
	}
	ASSERT(sendPacket->status == sizeof(PacketHeader) + dataSize);
	return true;
}
