#include "gameState.h"
#include "rendering/rendering.h"
#include "gameWorld.h"

#include "inputs.h"

#include <iostream>

#pragma once

#include <array>

int main(int argc, char* args[]) {
	initWindow("GAME");

	initRendering();

	initTextures("assets/textures.json");
	initModels("assets/models.json");

	initGame();

	Uint64 NOW = SDL_GetPerformanceCounter();
	Uint64 LAST = 0;
	double deltaTime = 0;

	

	while (g_inputState.quit == false) {
		LAST = NOW;
		NOW = SDL_GetPerformanceCounter();

		deltaTime = (double)((NOW - LAST) * 1000 / (double)SDL_GetPerformanceFrequency()) * 0.001;

		handleInputs();

		if (g_inputState.e && g_gameState.inventoryMode == false) {
			g_gameState.inventoryMode = true;
			g_inputState.e = false;
			SDL_SetRelativeMouseMode(SDL_FALSE);
			SDL_CaptureMouse(SDL_FALSE);
		}
		else if (g_gameState.inventoryMode && g_inputState.e) {
			g_gameState.inventoryMode = false;
			g_inputState.e = false;
			SDL_SetRelativeMouseMode(SDL_TRUE);
			SDL_CaptureMouse(SDL_TRUE);
		}
		
		// Don't update the world if the inventory screen  is open
		if (g_gameState.inventoryMode == false) {
			updateGameState(deltaTime);
		}

		renderGame();
	}

	exitRendering();


	return 0;
}