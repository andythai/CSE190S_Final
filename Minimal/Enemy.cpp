#include "Enemy.h"

// MACROS
#define WEAK 0
#define STRONG 1

using glm::mat4;
using glm::vec3;
using glm::vec4;

/*----------------------- CONSTRUCTORS/DESTRUCTOR --------------------------*/
Enemy::Enemy() {

}

Enemy::Enemy(Model * enemy_model, bool strong, float scale_size) {
	enemy = enemy_model;

	// Decide how much hp the enemy should get (NO TIME TO IMPLEMENT)
	enemyType = (strong) ? STRONG : WEAK;

	enemy->scale(scale_size);
	scale_factor = scale_size;
	initialize_hitbox();
}

Enemy::~Enemy() { 
	delete hitbox;
}

/*----------------- MORE GAME-RELATED FUNCTIONS -----------------*/
void Enemy::update() {

}

void Enemy::updateHitBox(glm::mat4 transform_mat) {
	hitbox->update(transform_mat);
}

Bound * Enemy::getHitBox() {
	return hitbox;
}

void Enemy::initialize_hitbox() {
	vec3 box_size = glm::scale(mat4(1.0f), vec3(scale_factor)) * vec4(enemy->getBoxDimensions(), 1.0f);
	hitbox = new Bound(box_size.x / 4.0f, box_size.y / 4.0f, box_size.z / 4.0f);
}

void Enemy::draw(Shader shader, mat4 P, mat4 V, mat4 C) {
	enemy->draw(shader, P, V, C);
}

void Enemy::drawHitBox(Shader shader, mat4 P, mat4 V, mat4 C) {
	hitbox->draw(shader.ID, P, V, C);
}