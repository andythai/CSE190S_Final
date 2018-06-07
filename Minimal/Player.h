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

class Player {
public:
	/* Public functions */
	/* Constructor for player object
	 * fileNames - Contains list of paths of 3D objects to represent the player
	 *			   Index 0 = Head, Index 1 = right hand, Index 2 = sword
	 * playerType - true = Player 1, false = Player 2
	 */
	Player(std::vector<const char *> fileNames, bool playerType);
	~Player();

	int getScore();
	// TODO: Add function to handle sword hits here

	void Draw(Shader shader, glm::mat4 P, glm::mat4 V);


private:
	/* Private Data */
	unsigned int score;
	unsigned int playerType;
	std::vector<Model *> models;	// Will store pointers to head and hand(s)
	Bound * attack_box;
};

#endif