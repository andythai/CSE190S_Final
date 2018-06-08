/* By Ronald Allan Baldonado
 * Player class derived from Node.h
 * Compatible with Scene Graph
 */
#pragma once
#ifndef _PLAYER_H_
#define _PLAYER_H_

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

class Player : public Node {
public:
	/* Public functions */
	Player();
	/* Constructor for player object
	 * Model *'s - Points to 3D objects to keep track of
	 * playerType - true = Player 1, false = Player 2
	 */
	Player(Model * head, Model * hand, Model * sword, bool playerType);
	~Player();

	int getScore();
	// TODO: Add function to handle sword hits here
	void update();

	void draw(Shader shader, glm::mat4 P, glm::mat4 V, glm::mat4 C);


private:
	/* Private Data */
	unsigned int score;
	unsigned int playerType;
	std::vector<Model *> models;	// Will store pointers to head and hand(s)
	Bound * attack_box;
};

#endif