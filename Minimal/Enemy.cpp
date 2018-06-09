#include "Enemy.h"

// MACROS
#define WEAK 0
#define STRONG 1

using glm::mat4;

/*----------------------- CONSTRUCTORS/DESTRUCTOR --------------------------*/
Enemy::Enemy() {

}

Enemy::Enemy(Model * enemy_model, bool strong, float scale_size) {
	enemy = enemy_model;

	// Decide how much hp the enemy should get
	enemyType = (strong) ? STRONG : WEAK;
	if (enemyType == STRONG) {
		hp = 3;
	}
	else {
		hp = 1;
	}

	enemy->scale(scale_size);
}

Enemy::~Enemy() { }

/*----------------- MORE GAME-RELATED FUNCTIONS -----------------*/
void Enemy::update() {

}


void Enemy::draw(Shader shader, mat4 P, mat4 V, mat4 C) {
	enemy->draw(shader, P, V, C);
}