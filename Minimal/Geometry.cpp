#define _CRT_SECURE_NO_DEPRECATE
#include "Geometry.h"

#define BUFFER 128

// Initialize necessary settings to draw object
Geometry::Geometry(const char* filepath) {
	toWorld = glm::mat4(1.0f);
	deg = 0.0f;
	parse(filepath);
	init_buffers();
}

Geometry::~Geometry() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &NBO);
	glDeleteBuffers(1, &EBO);
}

void Geometry::parse(const char* filepath) {
	// For finding max and min dimmensions
	float x_max = FLT_MIN;
	float x_min = FLT_MAX;
	float y_max = FLT_MIN;
	float y_min = FLT_MAX;
	float z_max = FLT_MIN;
	float z_min = FLT_MAX;

	// For finding the center and bounding the object
	float ratio = 1.0f;
	glm::vec3 center;

	FILE* fp;					// file pointer
	fp = fopen(filepath, "r");

	// Error Checking: Valid file path
	if (!fp) { std::cerr << "Invalid file path!" << std::endl; return; }

	// I/O loop
	while (1) {
		char ln[BUFFER];					// Store line

											// Read 1st word of line
		int status = fscanf(fp, "%s", &ln);
		if (status == EOF) { break; }		// EOF reached

											// Process vertices
		if (strcmp(ln, "v") == 0) {
			glm::vec3 vertex;
			fscanf(fp, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			vertices.push_back(vertex);

			// Find max and min coordinates of each dimmension
			if (vertex.x > x_max) { x_max = vertex.x; }
			else if (vertex.x < x_min) { x_min = vertex.x; }
			if (vertex.y > y_max) { y_max = vertex.y; }
			else if (vertex.y < y_min) { y_min = vertex.y; }
			if (vertex.z > z_max) { z_max = vertex.z; }
			else if (vertex.z < z_min) { z_min = vertex.z; }
		}

		// Process vertex normals
		else if (strcmp(ln, "vn") == 0) {
			glm::vec3 v_n;
			fscanf(fp, "%f %f %f\n", &v_n.x, &v_n.y, &v_n.z);
			normals.push_back(v_n);
		}

		// Process faces indices
		else if (strcmp(ln, "f") == 0) {
			unsigned int v_ind[3], n_ind[3];
			int num_matches = fscanf(fp, "%d//%d %d//%d %d//%d\n", &v_ind[0], &n_ind[0], &v_ind[1], &n_ind[1], &v_ind[2], &n_ind[2]);
			if (num_matches != 6) {
				std::cerr << "Cannot parse face info!" << std::endl;
				return;
			}
			// Indices are saved as 1-indexed. Change to 0-indexed
			for (int i = 0; i < 3; i++) {
				v_indices.push_back(v_ind[i] - 1);
				n_indices.push_back(n_ind[i] - 1);
			}
		}
	}
	fclose(fp);		// Closing file

	// Hold the min and max values of each dimmension
	std::vector<float> min_max;
	min_max.push_back(x_min);
	min_max.push_back(x_max);
	min_max.push_back(y_min);
	min_max.push_back(y_max);
	min_max.push_back(z_min);
	min_max.push_back(z_max);
	// Translate and rescale
	centerObjs(min_max);
}

void Geometry::centerObjs(std::vector<float> min_max) {
	if (min_max.size() != 6) {
		std::cout << "Parameter does not contain min max in xyz dimmensions!" << std::endl;
		return;
	}

	glm::vec3 center;

	// Retrieve min and max values of each dimmension
	float x_min = min_max[0];
	float x_max = min_max[1];
	float y_min = min_max[2];
	float y_max = min_max[3];
	float z_min = min_max[4];
	float z_max = min_max[5];

	// Move object's center towards the geomentric center
	center.x = (x_max + x_min) / 2;
	center.y = (y_max + y_min) / 2;
	center.z = (z_max + z_min) / 2;
	for (unsigned int i = 0; i < vertices.size(); i++) { vertices[i] -= center; }
}

void Geometry::init_buffers() {
	// Create array object & buffers
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &NBO);
	glGenBuffers(1, &EBO);

	// Bind vertex array buffer
	glBindVertexArray(VAO);

	// Bind vertex buffer object
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	// Enable the usage of layout location 0 (check the vertex shader to see what this is)
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	// Bind normal buffer object
	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	// Bind index buffer object
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * v_indices.size(), v_indices.data(), GL_STATIC_DRAW);

	// Unbind the currently bound buffer so that we don't accidentally make unwanted changes to it.
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Unbind the VAO now so we don't accidentally tamper with it.
	glBindVertexArray(0);
}

void Geometry::draw(glm::mat4 C) {
	// Establish variables for shader program
	glm::mat4 modelview = Window::V * C;
	uProjection = glGetUniformLocation(robotShader, "projection");
	uModelview = glGetUniformLocation(robotShader, "modelview");
	uModel = glGetUniformLocation(robotShader, "model");
	uView = glGetUniformLocation(robotShader, "view");

	// Now send these values to the shader program
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
	glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);
	glUniformMatrix4fv(uModel, 1, GL_FALSE, &C[0][0]);
	glUniformMatrix4fv(uView, 1, GL_FALSE, &Window::V[0][0]);

	// Now draw the cube. We simply need to bind the VAO associated with it.
	glBindVertexArray(VAO);

	// Tell OpenGL to draw with triangles, using 36 indices, the type of the indices, and the offset to start from
	glDrawElements(GL_TRIANGLES, (GLsizei)v_indices.size(), GL_UNSIGNED_INT, 0);
	// Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
	glBindVertexArray(0);
}

void Geometry::update() {

}