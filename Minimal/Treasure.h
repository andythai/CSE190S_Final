#pragma once
/* By Ronald Allan Baldonado
* Player class derived from Node.h
* Compatible with Scene Graph
*/
#pragma once
#ifndef _TREASURE_H_
#define _TREASURE_H_

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

class Treasure : public Node {
public:
	/* Public functions */
	Treasure(Model * pedestal, Model * treasure);
	~Treasure();

	void update();
	int getHealth();
	void checkIfHit();

	void draw(Shader shader, glm::mat4 P, glm::mat4 V, glm::mat4 C);	// Use if object if its going into scene graph
	void draw(Shader shader, glm::mat4 P, glm::mat4 V);					// Use if object is not going into scene graph


private:
	/* Private Data */
	int health;
	unsigned int playerType;
	std::vector<Model *> models;	// Will store pointers to head and hand(s)
	Bound * attack_box;

	/* Private Functions */
	void initialize();	// Move Treasure and pedestal together correctly
};

#endif