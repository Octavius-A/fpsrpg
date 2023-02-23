#include "gameState.h"
#include "collisions.h"

#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

extern const int SCREEN_WIDTH = 640;
extern const int SCREEN_HEIGHT = 480;

extern const float c_defaultFOV = 70.0f;

GameState g_gameState;
InputState g_inputState;

void initGame() {
	g_gameState.running = true;
	g_gameState.cameraFov = c_defaultFOV;
	
	genWorldSimple("maps/small_test.tmj");

	g_gameState.player = new Player(glm::vec3(1.5f, 1.0f, 1.5f), 101, "wizard");

	//g_gameState.entityList.push_back(new Entity3D(glm::vec3(3.0f, 1.0f, 7.0f), 2, "test cube :D"));
	//g_gameState.entityList.push_back(new Entity3D(glm::vec3(3.0f, 1.0f, 3.0f), 3, "two test cubes xd"));
	spawnCollidable(-0.12, 0.12, -0.3, 0.3, spawnEntity(glm::vec3(3.0f, 0.5f, 3.0f), 4, "Wooden chest"));
	spawnCollidable(-0.15, -0.1, -0.1, 0.1, spawnEntity(glm::vec3(1.0f, 0.5f, 1.0f), 5, "up ladder"));
	spawnCollidable(-0.05, 0.05, -0.05, 0.05, spawnEntity(glm::vec3(2.0f, 0.5f, 2.0f), 7, "wooden support"));
	//Entity3D* shortsword = spawnEntity(, 6, "short sword");
	// TODO: Consider how I'm handling this
	g_gameState.player->equipped = new Entity3D(glm::vec3(2.0f, 0.5f, 2.0f), 6, "short sword");

	Entity* sword = new Entity("Sword");
	Entity* clothTunic = new Entity("Cloth tunic");
	g_gameState.player->inventory.push_back(sword);
	g_gameState.player->inventory.push_back(clothTunic);
	//Entity* backpack = new Entity("Backpack");
	//backpack->inventory.push_back(new Entity("Minor potion of healing"));
	//g_gameState.player->inventory.push_back(backpack);
}

void updateGameState(double dTime) {
	static double seconds = 0;
	static int frameCount = 0;

	g_gameState.dTime = dTime;
	g_gameState.player->update();
	g_gameState.fps = (int)(1.0f / dTime);

	for (auto entity : g_gameState.entityList) {
		entity->update();
	}

	// make this toggleable
	//resolveCollisions2D();
}


void log(std::string str) {
	static const int maxLog = 200;
	g_gameState.gameLog.push_back(str);

	if (g_gameState.gameLog.size() > maxLog) {
		g_gameState.gameLog.pop_front();
	}
}

Entity3D* spawnEntity(glm::vec3 position, uint32_t modelId, std::string name) {
	Entity3D* en = new Entity3D(position, modelId, name);
	g_gameState.entityList.push_back(en);
	return en;
}

Collidable2D* spawnCollidable(float x1, float x2, float y1, float y2, Entity3D* entity) {
	Collidable2D* cl = new Collidable2D({ x1,x2,y1,y2 }, entity);
	g_gameState.collisionList.push_back(cl);
	return cl;
}