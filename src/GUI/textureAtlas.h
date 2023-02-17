#pragma once
#include "../rendering/rendering.h"

#include <nlohmann/json.hpp>
#include <map>

using json = nlohmann::json;

class TextureAtlas {
public:
	TextureAtlas(const char* path);
	~TextureAtlas();
	SDL_Surface* spriteSheet;
	SDL_Rect getSpriteRectFromId(int id);

	int spriteWidth;
	int spriteHeight;
private:
	int spriteSheetWidth;
	int spriteSheetHeight;
	int spriteCount;
	std::map<int, std::pair<int, int>> spriteCoordLookup;
};

