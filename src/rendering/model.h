#pragma once
#include "shader.h"

#include <glm/glm.hpp>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

class Mesh {
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	unsigned int texture;

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, unsigned int texture);
	void draw(Shader& shader, unsigned int depthCubemap, bool depth);
private:
	unsigned int VAO, VBO, EBO;
	
	void initMesh();
};

class Model {
public:
	Model(const char* path);
	void draw(Shader& shader, unsigned int depthCubemap, bool depth);
private:
	std::vector<Mesh> meshes;
	std::string directory;
	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	//std::vector<unsigned int> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};
