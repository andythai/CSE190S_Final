#include "Player.h"

// Index macros
#define HEAD 0
#define RIGHT_HAND 1
#define SWORD 2

// For frequently used glm types
using glm::mat4;
using glm::vec3;

/*------ CONSTRUCTOR/DESTRUCTOR FUNCTIONS --------*/
Player::Player() { }
Player::Player(Model * head, Model * hand, Model * sword, bool playerType) {
	// Initialize head, hand, and sword representations
	models.push_back(head);
	models.push_back(hand);
	models.push_back(sword);

	// Initialize score and player label
	initialize();
}

/*------------ HELPER FUNCTIONS --------------*/
void Player::initialize() {
	score = 0;
	this->playerType = (playerType) ? 1 : 2;
	
	// Resize and rotate
	models[RIGHT_HAND]->scale(0.5f);
	models[SWORD]->scale(1.4f);
	models[SWORD]->rotate(45.0f, vec3(0, 1.0f, 0));
	models[SWORD]->rotate(20.0f, vec3(1.0f, 0, 0));
	models[SWORD]->translate(vec3(0.11f, 0.04f, -0.15f));	// Coordinate system of sword has been rotated slightly. This is a hacky fix but requires 
															// less code than reorganizing order of transformations (As far as I know...)

	//models[HEAD]->scale(1.5f);
	models[HEAD]->rotate(90.0f, vec3(0, 1.0f, 0));
	models[HEAD]->translate(vec3(0, 0, 0.1f));
}

Player::~Player() {
	// Delete dynamically allocated 3D object representations
	/*
	for (Model * toDel : models) {
		delete toDel;
	}
	*/
}

/*---------- MORE GAME RELATED FUNCTIONS ----------*/
int Player::getScore() {
	return score;
}

void Player::update() { }

void Player::draw(Shader shader, mat4 P, mat4 V, mat4 C) {
	// TODO: Send info to shader to discriminate between players

	// Render player
	models[RIGHT_HAND]->draw(shader, P, V, C);
	models[SWORD]->draw(shader, P, V, C);
}

void Player::draw(Shader shader, mat4 P, mat4 V) {
	// FILL LATER?
}

void Player::drawHead(Shader shader, glm::mat4 P, glm::mat4 V, glm::mat4 C) {
	models[HEAD]->draw(shader, P, V, C);
}