/* Model class that uses assimp to load models (special thanks to learnopengl.com for main code!)
 * By Ronald Allan Baldonado
 */
#pragma once
#ifndef MODEL_H
#define MODEL_H

#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Shader.h"
#include "Node.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

class Model : public Node
{
public:
	/*  Model Data */
	glm::mat4 toWorld;
	float scale_factor;
	glm::vec3 avg_pos;
	glm::vec3 xyz_dimensions;	// Contains the max rectangular box dimensions of the model (taking in only the vertex data)

	vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
	vector<Mesh> meshes;
	string directory;
	bool gammaCorrection;

	/*  Functions   */
	// constructor, expects a filepath to a 3D model.
	Model(string const &path, bool gamma);

	// draws the model, and thus all its meshes
	void draw(Shader shader, glm::mat4 P, glm::mat4 V);													
	void draw(Shader shader, glm::mat4 P, glm::mat4 V, glm::mat4 C);									// C is some transformation to add to the model matrix (C * toWorld)
	void draw(Shader shader, glm::mat4 P, glm::mat4 V, glm::mat4 rot_scale_mat, glm::mat4 trans_mat);	// if C consists of both rotation/scaling as well as translation and will be a problem,
																										// deal with this case (trans_mat * toWorld * rot_scale_mat)

	// Transformation functions
	void translate(float x, float y, float z);
	void translate(glm::vec3 transVec);
	void reset();
	void scale(float factor);
	void scale(glm::vec3 factor_vec);
	void scaleRHD(float factor);				// Multiply the scale matrix from the right hand side of the toWorld matrix
	void rotate(float angle, glm::vec3 axis);	// angle passed should be in degrees
	void update();

	// Getter functions
	glm::mat4 getToWorld();
	glm::vec3 getBoxDimensions();

private:
	/*  Functions   */
	// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
	void loadModel(string const &path);

	// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
	void processNode(aiNode *node, const aiScene *scene);

	Mesh processMesh(aiMesh *mesh, const aiScene *scene);

	// checks all material textures of a given type and loads the textures if they're not loaded yet.
	// the required info is returned as a Texture struct.
	vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);

	// Center and resize object
	void centerAndResize();
};

#endif