#include "Player.h"

// Index macros
#define HEAD 0
#define RIGHT_HAND 1
#define SWORD 2

// For frequently used glm types
using glm::mat4;
using glm::vec3;
using glm::vec4;

/*------ CONSTRUCTOR/DESTRUCTOR FUNCTIONS --------*/
Player::Player() { }
Player::Player(Model * head, Model * hand, Model * sword, bool playerType) {
	// Initialize head, hand, and sword representations
	models.push_back(head);
	models.push_back(hand);
	models.push_back(sword);

	// Initialize score and player label
	score = 0;
	this->playerType = (playerType) ? 1 : 2;

	initialize();
}

/*------------ HELPER FUNCTIONS --------------*/
void Player::initialize() {
	// Resize and rotate
	models[RIGHT_HAND]->scale(0.5f);
	models[SWORD]->scale(1.4f);
	sword_scale_factor = 1.4f;
	models[SWORD]->rotate(45.0f, vec3(0, 1.0f, 0));
	models[SWORD]->rotate(20.0f, vec3(1.0f, 0, 0));
	models[SWORD]->translate(vec3(0.09f, 0.06f, -0.13f));	// Coordinate system of sword has been rotated slightly. This is a hacky fix but requires 
															// less code than reorganizing order of transformations (As far as I know...)

	models[HEAD]->rotate(90.0f, vec3(0, 1.0f, 0));
	models[HEAD]->translate(vec3(0, 0, 0.15f));
	
	// Create bounding box
	
	vec3 box_size = glm::scale(mat4(1.0f), vec3(sword_scale_factor)) * vec4(models[SWORD]->getBoxDimensions(), 1.0f);
	//vec3 box_size = models[SWORD]->getBoxDimensions();
	attack_box = new Bound(box_size.x /4.0f, box_size.y /4.0f, box_size.z / 4.0f);
	attack_box->toWorld = glm::translate(mat4(1.0f), vec3(0.002f, 0.27f, -0.39f)) *
							glm::rotate(glm::mat4(1.0f), 33.0f / 180.0f * glm::pi<float>(), vec3(1.0f, 0, 0)) 
							* glm::rotate(glm::mat4(1.0f), 90.0f / 180.0f * glm::pi<float>(), vec3(0, 1.0f, 0));
	
	//attack_box = new Bound(0.05f, 0.05f, 0.05f);
	//attack_box->toWorld = glm::translate(mat4(1.0f), vec3(0.0f, 0.2f, -0.3f)) * glm::rotate(glm::mat4(1.0f), 120.0f / 180.0f * glm::pi<float>(), vec3(1.0f, 0, 0));

}

Player::~Player() {
	// Delete dynamically allocated 3D object representations
	/*
	for (Model * toDel : models) {
		delete toDel;
	}
	*/
	delete attack_box;
}

/*---------- MORE GAME RELATED FUNCTIONS ----------*/
int Player::getScore() {
	return score;
}

float Player::getSwordScaleFactor() {
	return sword_scale_factor;
}

void Player::updateBoundingBox(mat4 transform_mat) {
	attack_box->update(transform_mat);
}

bool Player::checkHit(Bound * toCompare) {
	return attack_box->check_collision(toCompare);
}

void Player::drawPlayer(Shader shader, glm::mat4 P, glm::mat4 V, glm::mat4 handTransform, glm::mat4 headTransform) {
	// Send info to shader to discriminate between players
	shader.setInt(std::string("which_player"), playerType);
	// Render player 
	models[RIGHT_HAND]->draw(shader, P, V, handTransform);
	models[SWORD]->draw(shader, P, V, handTransform);
	models[HEAD]->draw(shader, P, V, headTransform);
}

void Player::drawBoundingBox(Shader shader, glm::mat4 P, glm::mat4 V, glm::mat4 handTransform) {
	// Check if sword bounding box exists
	if (attack_box == NULL) {
		std::cout << "Player " << playerType << "'s bounding box has not been initialize!" << std::endl;
		return;
	}

	attack_box->draw(shader.ID, P, V, handTransform);
}