#include "simpleGUI.h"
#include "../gameState.h"

void renderPlayerInventory();

void renderGUI() {
	debugWindow();

	if (g_gameState.inventoryMode) {
		renderPlayerInventory();
	}
}

void debugWindow() {
	ImGui::Begin("Debug");
	ImVec2 size(300, 100);
	ImVec2 pos(0, 0);
	ImGui::SetWindowSize(size);
	ImGui::SetWindowPos(pos);

	Player* player = g_gameState.player;
	float playerX = player->position.x;
	float playerY = player->position.y;
	float playerZ = player->position.z;

	ImGui::Text("player: %f, %f, %f", playerX, playerY, playerZ);
	
	double dt = g_gameState.dTime;
	
	ImGui::Text("delta: %f", dt);

	int fps = g_gameState.fps;
	ImGui::Text("fps: %d", fps);
	
	ImGui::End();
}

void renderPlayerInventory() {
	ImGui::Begin("Inventory");
	for (auto entity : g_gameState.player->inventory) {
		ImGui::Text(entity->name.c_str());
	}
	ImGui::End();
}