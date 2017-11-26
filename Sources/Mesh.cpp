#include "Mesh.hpp"

/* Taken from https://learnopengl.com/#!Model-Loading/Mesh */

using std::vector;

Mesh::Mesh(vector<Vertex> vertices, vector<GLuint> indices)
{
	this->vertices = vertices;
	this->indices = indices;
	this->setupMesh();
}

Mesh::Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;
	this->setupMesh();
}

Mesh::~Mesh() {}

void Mesh::draw(Shader * shader) {
	// Draw mesh
	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLES, +this->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

Mesh * Mesh::copy() {
	Mesh * copy = new Mesh(this->vertices, this->indices); // TODO: add textures sometime too
	copy->setupMesh();
	return copy;
}
void Mesh::updateNormals() {
    for (int i = 0; i < vertices.size(); i++) {
        vertices[i].Normal = glm::vec3(0, 0, 0);
    }
    for (int i = 0; i < indices.size(); i += 3) {
        glm::vec3 normal;
        glm::vec3 vertex1 = vertices[indices[i]].Position;
        glm::vec3 vertex2 = vertices[indices[i+1]].Position;
        glm::vec3 vertex3 = vertices[indices[i+2]].Position;
        glm::vec3 v1 = vertices[indices[i+1]].Position - vertices[indices[i]].Position;
        glm::vec3 v2 = vertices[indices[i + 2]].Position - vertices[indices[i]].Position;
        double a = glm::length(v1);
        double b = glm::length(v2);
        double c = glm::length(v1 - v2);
		if (a <= 0 || b <= 0) {
			normal = glm::vec3(0, 0, 0);
		}
		else {
			normal = glm::normalize(glm::cross(v2, v1));
		}
        vertices[indices[i]].Normal += normal;
        vertices[indices[i+1]].Normal += normal;
        vertices[indices[i+2]].Normal += normal;
    }
    for (int i = 0; i < vertices.size(); i++) {
        vertices[i].Normal = glm::normalize(vertices[i].Normal);
    }

    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_DYNAMIC_DRAW);

}

// Careful when using on Mesh that is shared by multiple Objects!
void Mesh::updateVertices(vector<Vertex> vertices) {
	this->vertices = vertices;

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_DYNAMIC_DRAW);
}

void Mesh::setupMesh() {
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glGenBuffers(1, &this->IBO);

	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*) 0);

	// Vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*) offsetof(Vertex, Normal));

	// Vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*) offsetof(Vertex, TexCoords));

	glBindVertexArray(0);
}