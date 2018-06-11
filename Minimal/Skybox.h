#pragma once
#ifndef _SKYBOX_H_
#define _SKYBOX_H_
/* Skybox header 
 * By Ronald Allan Baldonado
 */
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

// Default skybox face paths (CHANGE)
#define SKY_PATH_UP "../assets/Skybox_Water222_top.ppm"
#define SKY_PATH_DOWN "../assets/Skybox_Water222_base.ppm"
#define SKY_PATH_LEFT "../assets/Skybox_Water222_left.ppm"
#define SKY_PATH_RIGHT "../assets/Skybox_Water222_right.ppm"
#define SKY_PATH_FRONT "../assets/Skybox_Water222_front.ppm"
#define SKY_PATH_BACK "../assets/Skybox_Water222_back.ppm"

class Skybox
{
private:
	glm::mat4 toWorld;		// Model matrix
	/* Contains file paths to each skybox face
	 * NOTE: Order of faces should be (might be bugged in some manner...):
	 *		 Front, Right, Left, Up, Down, Back
	 */
	std::vector<char *> faces;

	// These variables are needed for the shader program
	GLuint VBO, VAO, EBO;
	GLuint uProjection, uModelview;
	GLuint textureID;

	/* Helper functions to initialize the skybox */
	void initialize();
	unsigned char* loadPPM(const char*, int&, int&);
	void loadTextures();

public:
	Skybox();									// Default initialization
	Skybox(std::vector<char *> & path_names);	// Pass in face paths in initialization
	~Skybox();

	/* Skybox render function
	 * shaderProgram - glsl shader ID
	 * projection - projection matrix
	 * modelview - modelview matrix
	 */
	void draw(GLuint shaderProgram, const glm::mat4 projection, glm::mat4 modelview);
};

// Define the coordinates and indices needed to draw the cube. Note that it is not necessary
// to use a 2-dimensional array, since the layout in memory is the same as a 1-dimensional array.
// This just looks nicer since it's easy to tell what coordinates/indices belong where.
const GLfloat vertices_sky[8][3] = {
	// "Front" vertices
	{ -100.0f, -100.0f,  100.0f },{ 100.0f, -100.0f,  100.0f },{ 100.0f,  100.0f,  100.0f },{ -100.0f,  100.0f, 100.0f },
	// "Back" vertices
	{ -100.0f, -100.0f, -100.0f },{ 100.0f, -100.0f, -100.0f },{ 100.0f,  100.0f, -100.0f },{ -100.0f,  100.0f, -100.0f }
};

// Note that GL_QUADS is deprecated in modern OpenGL (and removed from OSX systems).
// This is why we need to draw each face as 2 triangles instead of 1 quadrilateral
const GLuint indices_sky[6][6] = {
	// Front face
	{ 0, 1, 2, 2, 3, 0 },
	// Right face
	{ 1, 5, 6, 6, 2, 1 },
	// Back face
	{ 7, 6, 5, 5, 4, 7 },
	// Left face
	{ 4, 0, 3, 3, 7, 4 },
	// Bottom face
	{ 4, 5, 1, 1, 0, 4 },
	// Top face
	{ 3, 2, 6, 6, 7, 3 }
};

#endif
