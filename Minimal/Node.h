/* By Ronald Allan Baldonado
 * Base class that will drive the Object Graph
 * Derived Classes:
 *	Transform
 *	Geometry
 *  More to add soon
 */
#pragma once
#ifndef NODE_H
#define NODE_H
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

#include "Shader.h"

class Node {
public:
	// Will add transformations down the object graph path
	virtual void draw(Shader shader, glm::mat4 P, glm::mat4 V, glm::mat4 C) = 0;

	// Do not know of a function for this quite yet
	virtual void update() = 0;
};


#endif // !NODE_H