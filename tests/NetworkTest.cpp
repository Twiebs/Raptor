//#include <Application/Application.hpp>

#include <SDL2/SDL_net.h>
#undef main

#include <Core/Application.hpp>
#include <Core/Network.hpp>

#include <Graphics/GUI.hpp>

GUIContext context;
NetworkClient client;
NetworkServer server;

std::vector<char*> historyBuffer;

void NetworkTick(NetworkClient* client) {
	while (SDLNet_UDP_Recv(client->socket, client->packet)) {
		PacketHeader* header = (PacketHeader*)client->packet->data;
		switch (header->type) {
		case PacketType::CONNECT_FAIL:
			LOG_ERROR("We failed to connect to the server!");
			break;
		case PacketType::CHAT_MESSAGE:
			char* msg = (char*)malloc(client->packet->len - sizeof(PacketHeader));
			strncpy(msg, (const char*)(client->packet->data + sizeof(PacketHeader)), 
				client->packet->len - sizeof(PacketHeader));
			historyBuffer.push_back(msg);
			break;
		}
	}
}

void KeyCallback(int keycode, bool isDown) {
	ImGuiIO& io = ImGui::GetIO();
	io.KeysDown[keycode] = isDown;
}

void MainLoop(Application* app) {
	//app->BeginFrame();
	app->PollEvents();

	GUIBeginFrame(&context, app);
	ImGui::Begin("Network Status");


	static char username[64];
	static bool networkInit = false;
	static bool isServer = false;

	if (!networkInit) {
		if (ImGui::Button("Start Server")) {
			networkInit = true;
			isServer = true;
			NetworkInit(&server);
			NetworkInit(&client);	//Assume the server is its own seprate thing
		}

		if (ImGui::Button("Join Server")) {
			networkInit = true;
			NetworkInit(&client);
		}
	}
	else {
		if (isServer){
			ImGui::Text("Hosting Server");
			ImGui::Text("Clients Connected : %u / %u", server.clientCount, NETWORK_MAX_CLIENTS);

			// Client
			ImGui::Separator();
			ImGui::Text("Connected to server");
			ImGui::InputText("Username: ", username, 64, ImGuiInputTextFlags_EnterReturnsTrue);
			if (ImGui::Button("Disconnect")) {

			}
			NetworkTick(&client);
			NetworkTick(&server);
		}

		else {
			ImGui::InputText("Username: ", username, 64, ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::Text("Connected To Server");

			if (ImGui::Button("Disconnect")) {
				// TODO disconnect from server
			}

			NetworkTick(&client);
		}


		static bool scrollToBottom = false;
		ImGui::SetWindowSize(ImVec2(520, 600), ImGuiSetCond_FirstUseEver);
		ImGui::Begin("Console");
		ImGui::Separator();
		ImGui::BeginChild("ScrollingRegion", ImVec2(0, -ImGui::GetItemsLineHeightWithSpacing()));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));
		for (auto i = 0; i < historyBuffer.size(); i++) {
			auto line = historyBuffer[i];
			ImGui::Text(line);
		}
		if (scrollToBottom) {
			ImGui::SetScrollPosHere();
			scrollToBottom = false;
		}
		ImGui::PopStyleVar();
		ImGui::EndChild();

		ImGui::Separator();
		#define INPUT_BUFFER_LENGTH 256
		static char inputBuffer[INPUT_BUFFER_LENGTH];
		if (ImGui::InputText("##Chat", inputBuffer, INPUT_BUFFER_LENGTH, ImGuiInputTextFlags_EnterReturnsTrue)) {
			scrollToBottom = true;
			auto usernameLength = strlen(username);
			auto inputLength = strlen(inputBuffer);
			auto textWithUsername = (char*)malloc(usernameLength + inputLength + 3);
			textWithUsername[0] = '[';
			memcpy(textWithUsername + 1, username, usernameLength);
			textWithUsername[1 + usernameLength] = ']';
			textWithUsername[2 + usernameLength] = ' ';
			memcpy(textWithUsername + 3 + usernameLength, inputBuffer, inputLength);
			historyBuffer.push_back(textWithUsername);
			NetworkSend(&client, PacketType::CHAT_MESSAGE, textWithUsername, usernameLength + inputLength + 3);
			strcpy(inputBuffer, "");
		}

		if (ImGui::IsItemHovered() || (ImGui::IsRootWindowOrAnyChildFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0)))
			ImGui::SetKeyboardFocusHere(-1); // Auto focus
		ImGui::End();
	}

	ImGui::End();
	ImGui::ShowTestWindow();
	GUIEndFrame();

	// app->EndFrame();
}

int main() {
	Application app;
	app.SetKeyCallback(KeyCallback);
	app.Create("Network Test", 1280, 720, false);
	GUIContextInit(&context, app.GetWidth(), app.GetHeight());
	app.Run(MainLoop, &app);
	app.Destroy();
	return 0;
}
