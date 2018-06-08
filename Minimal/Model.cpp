#include "Model.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

/** Constructors, Destructor **/
Model::Model(string const &path, bool gamma = false) : gammaCorrection(gamma) {
	toWorld = glm::mat4(1.0f);
	loadModel(path);
	centerAndResize();
}

void Model::draw(Shader shader, glm::mat4 P, glm::mat4 V) {
	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i].Draw(shader, toWorld, V, P);
}

void Model::draw(Shader shader, glm::mat4 P, glm::mat4 V, glm::mat4 C) {
	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i].Draw(shader, C, V, P);
}

/** Helper Functions **/
unsigned int TextureFromFile(const char *path, const string &directory)
{
	string filename = string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
void Model::loadModel(string const &path)
{
	// read file via ASSIMP
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);
	// check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
		return;
	}
	// retrieve the directory path of the filepath
	directory = path.substr(0, path.find_last_of('/'));

	// process ASSIMP's root node recursively
	processNode(scene->mRootNode, scene);
}

// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void Model::processNode(aiNode *node, const aiScene *scene)
{
	// process each mesh located at the current node
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// the node object only contains indices to index the actual objects in the scene. 
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}

}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
	// data to fill
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;

	// Walk through each of the mesh's vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
						  // positions
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;
		// normals
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.Normal = vector;
		// texture coordinates
		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		/*
		// tangent
		vector.x = mesh->mTangents[i].x;
		vector.y = mesh->mTangents[i].y;
		vector.z = mesh->mTangents[i].z;
		vertex.Tangent = vector;
		// bitangent
		vector.x = mesh->mBitangents[i].x;
		vector.y = mesh->mBitangents[i].y;
		vector.z = mesh->mBitangents[i].z;
		vertex.Bitangent = vector;
		*/
		vertices.push_back(vertex);
	}
	// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// retrieve all indices of the face and store them in the indices vector
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	// process materials
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
	// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
	// Same applies to other texture as the following list summarizes:
	// diffuse: texture_diffuseN
	// specular: texture_specularN
	// normal: texture_normalN

	// 1. diffuse maps
	vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	// 2. specular maps
	vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	// 3. normal maps
	std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	// 4. height maps
	std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
	textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

	// return a mesh object created from the extracted mesh data
	return Mesh(vertices, indices, textures);
}

void Model::centerAndResize() {
	// Used for centering the mesh 
	GLfloat min_x = FLT_MAX;
	GLfloat max_x = FLT_MIN;
	GLfloat min_y = FLT_MAX;
	GLfloat max_y = FLT_MIN;
	GLfloat min_z = FLT_MAX;
	GLfloat max_z = FLT_MIN;
	GLfloat x, y, z;

	// Find max/min of each coordinate system
	for (Mesh mesh : meshes) {
		for (Vertex vert : mesh.vertices) {
			x = vert.Position.x;
			y = vert.Position.y;
			z = vert.Position.z;
			// Check for min/max bounds for mesh
			if (x > max_x) {
				max_x = x;
			}
			else if (x < min_x) {
				min_x = x;
			}

			if (y > max_y) {
				max_y = y;
			}
			else if (y < min_y) {
				min_y = y;
			}

			if (z > max_z) {
				max_z = z;
			}
			else if (z < min_z) {
				min_z = z;
			}
		}
	}

	// Recenter/rescale
	GLfloat avg_x = (max_x + min_x) / 2;
	GLfloat avg_y = (max_y + min_y) / 2;
	GLfloat avg_z = (max_z + min_z) / 2;
	this->avg_pos = glm::vec3(avg_x, avg_y, avg_z);

	GLfloat size;

	if (max_x - min_x > max_y - min_y)
		size = max_x - min_x;
	else {
		size = max_y - min_y;
	}
	if (max_z - min_z > size) {
		size = max_z - min_z;
	}

	// Bound to a 0.2 x 0.2 x 0.2 cube
	size = size / 0.2f;
	this->scale_factor = 1.0f / size;

	// Move model
	this->translate(-avg_pos);
	this->scale(scale_factor);
}


// checks all material textures of a given type and loads the textures if they're not loaded yet.
// the required info is returned as a Texture struct.
vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
{
	vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++)
		{
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip)
		{   // if texture hasn't been loaded already, load it
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), this->directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}
	}
	return textures;
}

/** Translation Functions **/
void Model::translate(glm::vec3 transVec) {
	toWorld = glm::translate(glm::mat4(1.0f), transVec) * toWorld;
}

void Model::translate(float x, float y, float z) {
	this->toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z)) * toWorld;
}

void Model::reset() {
	toWorld = glm::mat4(1.0f);
}

void Model::scale(float factor) {
	float world_X = this->toWorld[3][0] + avg_pos.x;
	float world_Y = this->toWorld[3][1] + avg_pos.y;
	float world_Z = this->toWorld[3][2] + avg_pos.z;

	this->toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(-world_X, -world_Y, -world_Z)) * toWorld;
	this->toWorld = glm::scale(glm::mat4(1.0f), glm::vec3(factor, factor, factor)) * toWorld;
	this->toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(world_X, world_Y, world_Z)) * toWorld;
}

void Model::rotate(float angle, glm::vec3 axis) {
	float world_X = this->toWorld[3][0];
	float world_Y = this->toWorld[3][1];
	float world_Z = this->toWorld[3][2];

	this->toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(-world_X, -world_Y, -world_Z)) * toWorld;
	this->toWorld = glm::rotate(glm::mat4(1.0f), angle / 180.0f * glm::pi<float>(), axis) * toWorld;
	this->toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(world_X, world_Y, world_Z)) * toWorld;
}

void Model::update() { }