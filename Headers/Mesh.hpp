#ifndef MESH_H_GUARD
#define MESH_H_GUARD

#include <string>
#include <vector>
#include "Shader.hpp"

#pragma warning(push, 0)
#pragma warning(disable: 4055)
#include <glad/glad.h>
#include <glm/glm.hpp>
#pragma warning(pop)

/* Taken from https://learnopengl.com/#!Model-Loading/Mesh */

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

struct Texture {
	GLuint id;
	std::string type;
};

class Mesh
{
public:
	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices);
	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures);
	virtual ~Mesh();

	void draw(Shader * shader);
	Mesh * copy();
	void updateVertices(std::vector<Vertex> vertices);
    void updateNormals();
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;

private:
	void setupMesh();

	GLuint VAO, VBO, IBO;
};

#endif // MESH_H_GUARD