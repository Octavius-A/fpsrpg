#pragma once

#include <array>

struct WorldCell_Simple {
	int ceilingId;
	int wallId;
	int floorId;
	bool solid;
	bool fullCell;
	bool abyss;
};


enum CellType {OBJECT, TILE};

struct WorldCell {
	bool solid;
	bool fullCell;
	CellType cellType;
	int cellId;
	float temperature;
	float moisture;
};

void loadWorldFromTMJ(const char* path);
bool checkTraversable(int x, int y, int z);

void genWorldSimple(const char* path);