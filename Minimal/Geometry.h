/* Derived from base class Node. Deals with Robot parts */
#pragma once
#ifndef GEOMETRY_H
#define GEOMETRY_H
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
#include "Window.h"

class Geometry : public Node {
	glm::mat4 toWorld;
	std::vector<unsigned int> v_indices, n_indices;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;

	GLuint VBO, VAO, EBO, NBO;
	GLuint uProjection, uModelview, uModel, uView;
	float deg;

public:
	static GLint robotShader;

	Geometry(const char* filepath);
	~Geometry();

	void parse(const char* filepath);
	void centerObjs(std::vector<float>);
	void init_buffers();
	void draw(glm::mat4);
	void update();
};

#endif