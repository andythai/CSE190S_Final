/* By Ronald Allan Baldonado
 * Enemy class for VR app
 */
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
	/* Enemy main constructor
	 * enemy_model - ptr to 3D model object
	 * strong - denotes whether enemy will get 1 or 3 HP (most likely just gonna use 1 HP for simplicity
	 * scale_size - for scaling enemy to correct size
	 */
	Enemy(Model * enemy_model, bool strong, float scale_size);
	~Enemy();

	/* Render function for enemy
	 * shader - glsl shader for rasterization and other gfx
	 * P - projection matrix
	 * V - view matrix
	 * C - transformation matrix
	 */
	void draw(Shader shader, glm::mat4 P, glm::mat4 V, glm::mat4 C);
	// Does not do anything
	void update();
	/* Update hit box by calling hitbox's update method
	 * transform_mat - Transformation matrix
	 */
	void updateHitBox(glm::mat4 transform_mat);
	// Hitbox getter method
	Bound * getHitBox();
	/* Hitbox render method.
	 * shader - glsl shader
	 * P - projection matrix
	 * V - view matrix
	 * C - transformation matrix
	 */
	void drawHitBox(Shader shader, glm::mat4 P, glm::mat4 V, glm::mat4 C);

private:
	/* Private Data */
	unsigned int enemyType;
	float scale_factor;
	Model * enemy;
	Bound * hitbox;

	/* Private Functions */
	void initialize_hitbox();
};

#endif