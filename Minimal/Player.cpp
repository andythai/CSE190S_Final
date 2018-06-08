#include "Player.h"

// Index macros
#define HEAD 0
#define RIGHT_HAND 1
#define LEFT_HAND 2

// For frequently used glm types
using glm::mat4;
using glm::vec3;

/*------ CONSTRUCTOR/DESTRUCTOR FUNCTIONS --------*/
Player::Player(std::vector<const char *> fileNames, bool playerType) {
	// Initialize models for hand names
	for (const char * name : fileNames) {
		models.push_back(new Model(std::string(name), false));
	}

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

void Player::Draw(Shader shader, mat4 P, mat4 V) {
	// TODO: Send info to shader to discriminate between players

	// Render player
	for (Model * toRender : models) {
		toRender->draw(shader, P, V);
	}
}