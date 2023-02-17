#include "textureAtlas.h"

#include <fstream>
#include <iostream>

TextureAtlas::TextureAtlas(const char* path) {
	std::ifstream f(path);
	json data = json::parse(f);

	std::string texturePath = data["path"];
	spriteSheet = loadSDLSurface(texturePath.c_str());
	spriteSheetWidth = data["width"];
	spriteSheetHeight = data["height"];
	spriteWidth = data["spriteWidth"];
	spriteHeight = data["spriteHeight"];
	spriteCount = data["spriteCount"];

	for (json::iterator it = data["sprites"].begin(); it != data["sprites"].end(); ++it) {
		json sprite = *it;
		int spriteId = sprite["id"];
		int spriteX = sprite["x"];
		int spriteY = sprite["y"];
		spriteCoordLookup[spriteId] = std::pair<int,int>(spriteX, spriteY);
	}
}

TextureAtlas::~TextureAtlas() {
	//SDL_DestroyTexture(spriteSheet);
	SDL_FreeSurface(spriteSheet);
}

SDL_Rect TextureAtlas::getSpriteRectFromId(int id) {
	SDL_Rect rect = { 0,0, spriteWidth, spriteHeight };
	auto it = spriteCoordLookup.find(id);
	if (it != spriteCoordLookup.end()) {
		rect.x = it->second.first * spriteWidth;
		rect.y = it->second.second * spriteHeight;
	}
	return rect;
}
