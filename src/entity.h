#pragma once

#include "collisions.h"

#include <string>
#include <map>
#include <glm/glm.hpp>
#include <vector>

class Entity {
public:
	Entity(std::string name);
	~Entity() {}

	std::string name;

	virtual void update();
	
};

class Container {
public:
	std::vector<Entity*> inventory;
};

class Entity3D : public Entity {
public:
	Entity3D(glm::vec3 startPos, unsigned int modelId, std::string name);
	virtual void update() override;
	glm::vec3 position;
	unsigned int modelId;
	std::string name;
};

class Collidable2D {
public:
	Collidable2D(Collision2DAABB collRect, Entity3D* entity);
	Collision2DAABB collision2DAABB;
	Entity3D* entity;
};

class Player : public Entity3D, public Container {
public:
	Player(glm::vec3 startPos, unsigned int modelId, std::string name);
	~Player() {}
	void update() override;
	Entity3D* equipped;
	glm::vec3 direction;
	glm::vec3 camFront;
	glm::vec3 camUp;
	Collision2DAABB collision2DAABB;
};

