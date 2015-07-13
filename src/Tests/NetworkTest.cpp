//#include <Application/Application.hpp>

#include <SDL2/SDL_net.h>
#undef main

#include <Core/Network.hpp>

int main() {
	static bool isServer = false;
	std::cin >> isServer;

	if (isServer) {
		NetworkServer server;
		NetworkInit(&server);
		while (true) {
			NetworkTick(&server);
		}
	}
	else {
		NetworkClient client;
		NetworkInit(&client);
		NetworkTick(&client);
		while (true) {
			while (SDLNet_UDP_Recv(client.socket, client.packet)) {
				LOG_INFO("WE got a packet!");
				LOG_INFO("Its working diggiyyy Dawggg!");
			}
		}
	}

	SDLNet_Quit();
	SDL_Quit();
	return 0;
}
