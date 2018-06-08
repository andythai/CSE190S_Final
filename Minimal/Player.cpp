#include "Player.h"

// Index macros
#define HEAD 0
#define RIGHT_HAND 1
#define LEFT_HAND 2

// For frequently used glm types
using glm::mat4;
using glm::vec3;

/*------ CONSTRUCTOR/DESTRUCTOR FUNCTIONS --------*/
Player::Player(Model * head, Model * hand, Model * sword, bool playerType) {
	// Initialize head, hand, and sword representations
	models.push_back(head);
	models.push_back(hand);
	models.push_back(sword);

	// Initialize score and player label
	score = 0;
	this->playerType = (playerType) ? 1 : 2;
}

Player::~Player() {
	// Delete dynamically allocated 3D object representations
	for (Model * toDel : models) {
		delete toDel;
	}
}

int Player::getScore() {
	return score;
}

void Player::update() { }

void Player::draw(Shader shader, mat4 P, mat4 V, mat4 C) {
	// TODO: Send info to shader to discriminate between players

	// Render player
	for (Model * toRender : models) {
		toRender->draw(shader, P, V, C);
	}
}