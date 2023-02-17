#include "collisions.h"

#include "gameState.h"
#include <iostream>


bool intersectAABB(const Collision2DAABB& a, const Collision2DAABB& b, Collision2DAABB& inter) {
	float x1 = std::max(a.x1, b.x1);
	float y1 = std::max(a.y1, b.y1);
	float x2 = std::min(a.x2, b.x2);
	float y2 = std::min(a.y2, b.y2);

	if (x1 >= x2 || y1 >= y2) {
		return false;
	}
	else {
		inter = {x1, x2, y1, y2};
		return true;
	}
}

void resolveCollisions2D() {
	float playerX = g_gameState.player->position.x;
	float playerY = g_gameState.player->position.z;
	Collision2DAABB pCol = g_gameState.player->collision2DAABB;

	Collision2DAABB pColFinal = { playerX + pCol.x1, playerX + pCol.x2, playerY + pCol.y1, playerY + pCol.y2 };

	for (int y = 0; y < g_gameState.worldYMax; ++y) {
		for (int x = 0; x < g_gameState.worldXMax; ++x) {
			WorldCell_Simple wc = g_gameState.worldArray_simple[y][x];
			if (wc.solid) {
				float x1 = x - 0.5;
				float x2 = x + 0.5;
				float y1 = y - 0.5;
				float y2 = y + 0.5;

				Collision2DAABB wcRect = { x - 0.5, x + 0.5, y - 0.5, y + 0.5 };

				Collision2DAABB intersection;

				if (intersectAABB(pColFinal, wcRect, intersection)) {
					float width = intersection.x2 - intersection.x1;
					float height = intersection.y2 - intersection.y1;
					if (width < height) {
						if (playerX > x) {
							g_gameState.player->position.x += width;
						}
						else {
							g_gameState.player->position.x -= width;
						}
					}
					else {
						if (playerY > y) {
							g_gameState.player->position.z += height;
						}
						else {
							g_gameState.player->position.z -= height;
						}
					}
				}
			}
		}
	}

	for (auto collision2D : g_gameState.collisionList) {
		float objX = collision2D->entity->position.x;
		float objY = collision2D->entity->position.z;
		Collision2DAABB objCol = collision2D->collision2DAABB;
		Collision2DAABB objColFinal = { objX + objCol.x1, objX + objCol.x2 , objY + objCol.y1 , objY + objCol.y2 };
		Collision2DAABB intersection;
		
		if (intersectAABB(pColFinal, objColFinal, intersection)) {
			float width = intersection.x2 - intersection.x1;
			float height = intersection.y2 - intersection.y1;
			if (width < height) {
				if (playerX > objX) {
					g_gameState.player->position.x += width;
				}
				else {
					g_gameState.player->position.x -= width;
				}
			}
			else {
				if (playerY > objY) {
					g_gameState.player->position.z += height;
				}
				else {
					g_gameState.player->position.z -= height;
				}
			}
		}
	}
}