#include "Cube.hpp"

using glm::vec3;
using std::vector;
using std::min;
using std::max;

Mesh * Cube::standardMesh;

Cube::Cube() : Object() {
	params[0] = 1.0;
	params[1] = 1.0;
	params[2] = 1.0;
	constructStandardMesh();
}

Cube::Cube(float w, float l, float h, float x, float y, float z) : Object(vec3(x, y, z)) {
	params[0] = w;
	params[1] = h;
	params[2] = l;
	constructStandardMesh();
}

Cube::Cube(vec3 p) : Object(p) {
	params[0] = 1.0;
	params[1] = 1.0;
	params[2] = 1.0;
	constructStandardMesh();
}

Cube::Cube(float w, float l, float h, vec3 p) : Object(p) {
	params[0] = w;
	params[1] = h;
	params[2] = l;
	constructStandardMesh();
}

Cube::~Cube() {
	if (mesh && mesh != standardMesh)
		delete mesh;
    //if (deformer != nullptr)
    //    delete deformer;
    if (bv != nullptr)
        delete bv;
}

void Cube::useStandardMesh() {
	if (usingStandardMesh)
		return;

	if (mesh)
		delete mesh;

	usingStandardMesh = true;
	mesh = standardMesh;
}

void Cube::useCustomMesh() {
	if (!usingStandardMesh)
		return; // might want to make it re-copy the standard in this case?

	usingStandardMesh = false;
	mesh = standardMesh->copy();
}

void Cube::constructStandardMesh(bool override) {
    this->bv = new Rect(glm::vec2(dyn.pos[0], dyn.pos[2]), params[0], params[2]);

	if (!override && standardMesh != NULL) {
		mesh = standardMesh;
		return;
	}

	vector<Vertex> vertices;
	for (int i = 0; i < 24; i++) {
		vertices.push_back(Vertex());
	}

	int m = 0;
	// Front face
	vertices[m*4 + 0].Position = vec3(0.5, -0.5, 0.5);
	vertices[m*4 + 1].Position = vec3(-0.5, -0.5, 0.5);
	vertices[m*4 + 2].Position = vec3(0.5, 0.5, 0.5);
	vertices[m*4 + 3].Position = vec3(-0.5, 0.5, 0.5);

	for (int i = 0; i < 4; i++)
		vertices[m*4 + i].Normal = vec3(0, 0, 1);

	m = 1;
	// Left face
	vertices[m*4 + 0].Position = vec3(-0.5, -0.5, 0.5);
	vertices[m*4 + 1].Position = vec3(-0.5, -0.5, -0.5);
	vertices[m*4 + 2].Position = vec3(-0.5, 0.5, 0.5);
	vertices[m*4 + 3].Position = vec3(-0.5, 0.5, -0.5);

	for (int i = 0; i < 4; i++)
		vertices[m*4 + i].Normal = vec3(-1, 0, 0);

	m = 2;
	// Back face
	vertices[m*4 + 0].Position = vec3(-0.5, -0.5, -0.5);
	vertices[m*4 + 1].Position = vec3(0.5, -0.5, -0.5);
	vertices[m*4 + 2].Position = vec3(-0.5, 0.5, -0.5);
	vertices[m*4 + 3].Position = vec3(0.5, 0.5, -0.5);

	for (int i = 0; i < 4; i++)
		vertices[m*4 + i].Normal = vec3(0, 0, -1);

	m = 3;
	// Right face
	vertices[m*4 + 0].Position = vec3(0.5, -0.5, -0.5);
	vertices[m*4 + 1].Position = vec3(0.5, -0.5, 0.5);
	vertices[m*4 + 2].Position = vec3(0.5, 0.5, -0.5);
	vertices[m*4 + 3].Position = vec3(0.5, 0.5, 0.5);

	for (int i = 0; i < 4; i++)
		vertices[m*4 + i].Normal = vec3(1, 0, 0);

	m = 4;
	// Top face
	vertices[m*4 + 0].Position = vec3(0.5, 0.5, 0.5);
	vertices[m*4 + 1].Position = vec3(-0.5, 0.5, 0.5);
	vertices[m*4 + 2].Position = vec3(0.5, 0.5, -0.5);
	vertices[m*4 + 3].Position = vec3(-0.5, 0.5, -0.5);

	for (int i = 0; i < 4; i++)
		vertices[m*4 + i].Normal = vec3(0, 1, 0);

	m = 5;
	// Top face
	vertices[m*4 + 0].Position = vec3(0.5, -0.5, -0.5);
	vertices[m*4 + 1].Position = vec3(-0.5, -0.5, -0.5);
	vertices[m*4 + 2].Position = vec3(0.5, -0.5, 0.5);
	vertices[m*4 + 3].Position = vec3(-0.5, -0.5, 0.5);

	for (int i = 0; i < 4; i++)
		vertices[m*4 + i].Normal = vec3(0, -1, 0);

	vector<GLuint> indices;
	for (int i = 0; i < 6; i++) {
		indices.push_back(i*4);
		indices.push_back(i*4 + 2);
		indices.push_back(i*4 + 3);

		indices.push_back(i*4);
		indices.push_back(i*4 + 3);
		indices.push_back(i*4 + 1);
	}

	if (override) {
		standardMesh->vertices = vertices;
		standardMesh->indices = indices;
	}
	else {
		standardMesh = new Mesh(vertices, indices);
	}
	mesh = standardMesh;
}


void Cube::simulate(double dt) {
    return;
}
void Cube::simpleSimulate(double dt) {
    return;
}
