#include "entity.h"
#include "gameState.h"

#include <iostream>

Entity::Entity(std::string name)
	: name(name)
{}

void Entity::update() {
	std::cout << "entity update\n";
}

Player::Player(glm::vec3 startPos, unsigned int modelId, std::string name)
	: Entity3D( startPos, modelId, name)
{
	camFront = glm::vec3(0.0f, 0.0f, -1.0f);
	camUp = glm::vec3(0.0f, 1.0f, 0.0f);
	collision2DAABB = { -0.2,0.2, -0.2, 0.2 };

	isAttacking = false;
	canAttack = true;
	attackCooldown = false;
	attackAnimTimer = 0.0f;
	attackAnimDuration = 0.25f;
	cooldownTimer = 0.0f;
	cooldownDuration = 0.1f;
}

void Player::update() {
	double dTime = g_gameState.dTime;

	// update player look direction

	static float yaw = 0.0f;
	static float pitch = 0.0f;
	static float maxPitch = 89.0f;
	static float minPitch = -89.0f;
	
	float xoffset = g_inputState.mouseState.mouseX * g_gameState.mouseSensitivity * dTime;
	float yoffset = g_inputState.mouseState.mouseY * g_gameState.mouseSensitivity * dTime;
	yaw += xoffset;
	pitch -= yoffset;

	if (pitch > maxPitch) pitch = maxPitch;
	if (pitch < minPitch) pitch = minPitch;

	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction = glm::normalize(direction);

	glm::vec3 moveDir;
	moveDir.x = cos(glm::radians(yaw));
	moveDir.y = 0;
	moveDir.z = sin(glm::radians(yaw));
	moveDir = glm::normalize(moveDir);

	static float playerSpeed = 2.0f;
	float finalSpeed = playerSpeed * dTime;
	float strafeSpeed = (playerSpeed * 0.5) * dTime;
	if (g_inputState.w) {
		position += moveDir * finalSpeed;
	}
	if (g_inputState.s) {
		position -= moveDir * finalSpeed;
	}
	if (g_inputState.a) {
		position -= glm::normalize(glm::cross(direction, glm::vec3(0.0f, 1.0f, 0.0f))) * strafeSpeed;
	}
	if (g_inputState.d) {
		position += glm::normalize(glm::cross(direction, glm::vec3(0.0f, 1.0f, 0.0f))) * strafeSpeed;
	}


	if (g_inputState.mouseState.leftButtonDown && canAttack) {
		isAttacking = true;
		canAttack = false;
	}

	if (isAttacking) {
		if (attackAnimTimer >= attackAnimDuration) {
			attackAnimTimer = 0.0f;
			//canAttack = true;
			attackCooldown = true;
			isAttacking = false;
		}
		else {
			attackAnimTimer += dTime;
		}
	}

	if (attackCooldown) {
		if (cooldownTimer >= cooldownDuration) {
			cooldownTimer = 0.0f;
			attackCooldown = false;
			canAttack = true;
		}
		else {
			cooldownTimer += dTime;
		}
	}

}

Entity3D::Entity3D(glm::vec3 startPos, unsigned int modelId, std::string name)
	: Entity(name)
	, position(startPos)
	, modelId(modelId)
	, name(name)
{}

void Entity3D::update() {
	double dTime = g_gameState.dTime;
	// do nothing :D
}

Collidable2D::Collidable2D(Collision2DAABB collRect, Entity3D* entity)
	: collision2DAABB(collRect)
	, entity(entity){}