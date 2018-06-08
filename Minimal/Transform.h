/* Transform class derived from Node class. This class is used to deal with robot transformations */
#pragma once
#ifndef TRANSFORM_H
#define TRANSFORM_H
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
#include "Node.h"
#include <list>

class Transform : public Node {
	/* Data */
	glm::mat4 M;					// Matrix to add to chain of object transformatons
	std::list<Node*> child_ptrs;	// Children to pass obj transformations down to

public:
	/* Functions */
	Transform();			// Initialize transformation node with identity matrix
	Transform(glm::mat4);	// Initialize transformation with passed in matrix
	~Transform();

	void addChild(Node*);												// Add child to list
	void removeChild(Node*);											// Remove a child from the list
	void draw(Shader shader, glm::mat4 P, glm::mat4 V, glm::mat4 C);		// Pass transformation down and draw leaf node

	glm::mat4 get_transform();				// Grab current transformations (return M)
	void change_transform(glm::mat4);		// Change M directly
	void update();							// Empty for now
};

#endif