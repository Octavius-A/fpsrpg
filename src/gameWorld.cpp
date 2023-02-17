#include "gameWorld.h"
#include "gameState.h"

#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

void genWorldSimple(const char* path) {
	std::ifstream f(path);
	json data = json::parse(f);

	int worldWidth = data["width"];
	int worldHeight = data["height"];

	assert(worldWidth <= g_gameState.worldXMax);
	assert(worldHeight <= g_gameState.worldYMax);
	// assert on the len of the layers array
	json floorData = data["layers"][0];
	json wallData = data["layers"][1];
	json ceilingData = data["layers"][2];

	for (int i = 0; i < worldWidth * worldHeight; ++i) {
		int worldY = i / worldHeight;
		int worldX = i % worldWidth;

		int ceilingId = ceilingData["data"].at(i);
		int wallId = wallData["data"].at(i);
		int floorId = floorData["data"].at(i);
		
		bool fullCell = true;
		bool solid = true;
		if (wallId == 0) {
			fullCell = false;
			solid = false;
		}

		bool abyss = wallId == 0 && ceilingId == 0 && floorId == 0;

		g_gameState.worldArray_simple[worldY][worldX] = {
			ceilingId,
			wallId,
			floorId,
			solid,
			fullCell,
			abyss
		};
	}
}

//void loadWorldFromTMJ(const char* path) {
//	std::ifstream f(path);
//	json data = json::parse(f);
//
//	int worldWidth = data["width"];
//	int worldHeight = data["height"];
//	int worldLayers = data["layers"].size();
//
//	assert(worldWidth <= g_gameState.worldXMax);
//	assert(worldHeight <= g_gameState.worldYMax);
//	assert(worldLayers  <= g_gameState.worldZMax);
//
//	int layer = 0;
//
//	for (json::iterator it = data["layers"].begin(); it != data["layers"].end(); ++it) {
//		json layerData = *it;
//		for (int i = 0; i < worldWidth * worldHeight; ++i) {
//			int cellId = layerData["data"].at(i);
//			int worldY = i / worldHeight;
//			int worldX = i % worldWidth;
//			int worldZ = layer;
//			if (cellId == 36) {
//				// skip
//			}
//			else {
//				bool fullCell = true;
//				if (cellId == 0 || cellId == 11) fullCell = false;
//
//				g_gameState.worldArray[worldZ][worldY][worldX] = {
//					false,
//					fullCell,
//					TILE,
//					cellId,
//					12.0f,
//					0.0f
//				};
//			}
//		}
//		layer += 1;
//	}
//}
//
//bool checkTraversable(int x, int y, int z) {
//	bool traversable = false;
//
//	// check if the cell itself is inside the world and solid
//	// and if there is solid ground beneath it
//
//	if (x > 0 && x < g_gameState.worldXMax &&
//		y > 0 && y < g_gameState.worldYMax &&
//		z > 0 && z < g_gameState.worldZMax) {
//		if (g_gameState.worldArray[z][y][x].solid == false &&
//			g_gameState.worldArray[z-1][y][x].fullCell == true) {
//			traversable = true;
//		}
//	}
//
//	return traversable;
//}