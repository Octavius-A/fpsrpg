#include "worldCellMesh.h"

WorldCellMesh cubeFront{
	6,0, {
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,    0.0f, 0.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,     1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,    1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,    1.0f, 1.0f,
	 -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,   0.0f, 0.0f,
	 -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,   0.0f, 1.0f,
	}
};

WorldCellMesh cubeBack{
	6,0, {
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   0.0f, 0.0f,
	}
};

WorldCellMesh cubeLeft{
	6,0, {
	-0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
	}
};

WorldCellMesh cubeRight{
	6,0, {
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
	}
};

WorldCellMesh cubeBottom{
	6,0, {
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,   0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,   1.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,   1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,   1.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,   0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,   0.0f, 0.0f,
	}
};

WorldCellMesh cubeTop{
	6,0, {
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,   0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,   1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,   1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,   1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,   0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,   0.0f, 1.0f,
	}
};

WorldCellMesh spriteMesh{
	6,0, {
	-0.5f, -0.5f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.0f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.0f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
	 0.5f,	0.5f,  0.0f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
	-0.5f,	0.5f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
	}
};


std::vector<float> fullVertData = {};

void initVertData() {
	fullVertData.insert(fullVertData.end(), cubeFront.vertData.begin(), cubeFront.vertData.end());
	cubeFront.meshStart = 6 * 0;
	fullVertData.insert(fullVertData.end(), cubeBack.vertData.begin(), cubeBack.vertData.end());
	cubeBack.meshStart = 6 * 1;
	fullVertData.insert(fullVertData.end(), cubeLeft.vertData.begin(), cubeLeft.vertData.end());
	cubeLeft.meshStart = 6 * 2;
	fullVertData.insert(fullVertData.end(), cubeRight.vertData.begin(), cubeRight.vertData.end());
	cubeRight.meshStart = 6 * 3;
	fullVertData.insert(fullVertData.end(), cubeTop.vertData.begin(), cubeTop.vertData.end());
	cubeTop.meshStart = 6 * 4;
	fullVertData.insert(fullVertData.end(), cubeBottom.vertData.begin(), cubeBottom.vertData.end());
	cubeBottom.meshStart = 6 * 5;
	fullVertData.insert(fullVertData.end(), spriteMesh.vertData.begin(), spriteMesh.vertData.end());
	spriteMesh.meshStart = 6 * 6;
}