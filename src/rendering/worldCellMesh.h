#pragma once

#include <vector>

struct WorldCellMesh {
	int vertCount;
	int meshStart;
	std::vector<float> vertData;
	
};

extern WorldCellMesh cubeFront;
extern WorldCellMesh cubeBack;
extern WorldCellMesh cubeLeft;
extern WorldCellMesh cubeRight;
extern WorldCellMesh cubeTop;
extern WorldCellMesh cubeBottom;
extern WorldCellMesh spriteMesh;
extern WorldCellMesh playerMesh;


extern std::vector<float> fullVertData;
void initVertData();