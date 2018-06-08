#pragma once
#ifndef _ENEMY_H_
#define _ENEMY_H_

#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
// Use of degrees is deprecated. Use radians instead.
#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <vector>

#include "Model.h"
#include "Bound.h"

class Enemy : public Node {
public:
	/* Public functions */
	Enemy();
	~Enemy();

	void spawnMonsters();
	void wasHit();

	void draw(Shader shader, glm::mat4 P, glm::mat4 V, glm::mat4 C);
	void update();

private:
	/* Private Data */
	unsigned int enemyType;
	Model * enemy;
	Bound * hitbox;
};

#endif