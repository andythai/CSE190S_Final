#include "Curve.h"

#define DEFAULT_NUM_SAMPLES 150.0f

Curve::Curve() {
	g_bez = glm::mat4(1.0f);
	c_bez = g_bez * b_bez;
	toWorld = glm::mat4(1.0f);
	num_samples = DEFAULT_NUM_SAMPLES;
	calc_pnts();
	init_buffers();
}

Curve::Curve(glm::mat4 cont_pts) {
	g_bez = cont_pts;
	c_bez = g_bez * b_bez;
	toWorld = glm::mat4(1.0f);
	num_samples = DEFAULT_NUM_SAMPLES;
	calc_pnts();
	init_buffers();

}

Curve::Curve(glm::mat4 cont_pts, unsigned int n) {
	g_bez = cont_pts;
	c_bez = g_bez * b_bez;
	toWorld = glm::mat4(1.0f);
	num_samples = n;
	calc_pnts();
	init_buffers();

}

Curve::~Curve() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

std::vector<glm::vec3> & Curve::getVertices() {
	return vertices;
}

void Curve::init_buffers() {
	// Create array object & buffers
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// Bind vertex array buffer
	glBindVertexArray(VAO);

	// Bind vertex buffer object
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	// Enable the usage of layout location 0 (check the vertex shader to see what this is)
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	// Bind index buffer object
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);

	// Unbind the currently bound buffer so that we don't accidentally make unwanted changes to it.
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Unbind the VAO now so we don't accidentally tamper with it.
	glBindVertexArray(0);
}

void Curve::draw(GLint shaderProgram, glm::mat4 P, glm::mat4 V) {
	// Calculate the combination of the model and view (camera inverse) matrices
	glm::mat4 modelview = V * toWorld;

	// Get the location of the uniform variables "projection" and "modelview"
	/*
	uProjection = glGetUniformLocation(shaderProgram, "projection");
	uModelview = glGetUniformLocation(shaderProgram, "modelview");
	// Now send these values to the shader program
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &P[0][0]);
	glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);
	*/
	// What should actually be sent to the shader
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &toWorld[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &V[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &P[0][0]);
	// Now draw the cube. We simply need to bind the VAO associated with it.
	glBindVertexArray(VAO);
	// Tell OpenGL to draw with triangles, using 36 indices, the type of the indices, and the offset to start from
	//glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_LINE_STRIP, 0, indices.size());
	// Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
	glBindVertexArray(0);
}

void Curve::calc_pnts() {
	glm::vec4 pnt(1.0f);
	float denominator3 = (num_samples * num_samples * num_samples);
	float denominator2 = num_samples * num_samples;

	// Add calculated points as vertices
	vertices.push_back(glm::vec3(c_bez * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));
	for (unsigned int i = 0; i < num_samples; i++) {
		pnt.x = ((i + 1.0f) * (i + 1.0f) * (i + 1.0f)) / denominator3;
		pnt.y = ((i + 1.0f) * (i + 1.0f)) / denominator2;
		pnt.z = (i + 1.0f) / num_samples;
		vertices.push_back(glm::vec3(c_bez * pnt));
		indices.push_back(i);
	}
	indices.push_back(num_samples);
}