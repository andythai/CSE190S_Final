/* For drawing a cubic bezier curve using uniform sampling */
#pragma once
#ifndef CURVE_H
#define CURVE_H
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

#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#endif

class Curve {
private:
	/* Data */
	glm::mat4 b_bez = {
		-1.0f, 3.0f, -3.0f, 1.0f,
		3.0f, -6.0f, 3.0f, 0.0f,
		-3.0f, 3.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f, 0.0f
	};

	glm::mat4 g_bez;	// Column vectors make up control points
	glm::mat4 c_bez;	// Multiply g_bez by b_bez
	float num_samples;
	
	glm::mat4 toWorld;
	std::vector<glm::vec3> vertices;
	std::vector<unsigned int> indices;
	// These variables are needed for the shader program
	GLuint VBO, VAO, EBO;
	GLuint uProjection, uModelview;

	/* Private functions */
	// Initialize buffers for debug drawing the curve
	void init_buffers();
	// Calculates points on curve based on number of samples and fills vertices and indices
	void calc_pnts();
public:

	Curve();
	Curve(glm::mat4);								// Default number of samples is 150
	Curve(glm::mat4, unsigned int num_samples);		// Pick your own number of samples
	~Curve();

	std::vector<glm::vec3> & getVertices();			

	void draw(GLint shaderProgram, glm::mat4 P, glm::mat4 V);
};