#pragma once

#include "gameWorld.h"
#include "entity.h"

#include <SDL.h>
#include <array>
#include <vector>
#include <deque>

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;

extern const float c_defaultFOV;

struct GameState {
	bool running;

	float cameraFov;
	float mouseSensitivity = 100.0f;
	bool inventoryMode = false;
	bool showDebug = false;
	bool tcl = false;

	static constexpr int worldXMax = 100;
	static constexpr int worldYMax = 100;
	static constexpr int worldZMax = 5;

	double dTime;
	int fps;

	//std::array<std::array<std::array<WorldCell, worldXMax>, worldYMax>, worldZMax> worldArray;
	std::array<std::array<WorldCell_Simple, worldXMax>, worldYMax> worldArray_simple;

	//std::vector<Entity*> entityList;
	std::vector<Entity3D*> entityList;
	std::vector<Collidable2D*> collisionList;

	std::deque<std::string> gameLog;

	Player* player;
};

extern GameState g_gameState;

struct InputState {
	struct MouseState {
		int mouseX = 0;
		int mouseY = 0;
		bool leftButtonDown = false;
		bool rightButtonDown = false;
	};
	MouseState mouseState;

	bool quit = false;
	bool spaceBar = false;

	bool w = false;
	bool a = false;
	bool s = false;
	bool d = false;
	bool e = false;
	bool f3 = false;
	bool f4 = false;
};

extern InputState g_inputState;

void initGame();

void updateGameState(double dTime);

void log(std::string str);

Entity3D* spawnEntity(glm::vec3 position, uint32_t modelId, std::string name);
Collidable2D* spawnCollidable(float x1, float x2, float y1, float y2, Entity3D* entity);

//void updateEntities();