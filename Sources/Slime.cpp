#include "Slime.hpp"

//the deformable object, based on the sphere.

Slime::Slime()  : Sphere(){
    params[0] = 1.0;
    params[1] = 1.0;
    params[2] = 1.0;
    constructStandardMesh();
}

Slime::Slime(float r, float x, float y, float z) : Sphere(r, x, y, z) {
    useCustomMesh();
    this->dyn.gravity = glm::vec3(0, -1, 0);
    deformer = new Chainmail(mesh, stacks, slices, this->dyn.pos);
    convertMeshToWorldCoords();
}

Slime::Slime(float r) : Sphere(r) {
    useCustomMesh();
    this->dyn.gravity = glm::vec3(0, -1, 0);
    deformer = new Chainmail(mesh, stacks, slices, this->dyn.pos);
    convertMeshToWorldCoords();
}

Slime::Slime(float r, glm::vec3 p) : Sphere(r, p) {
    useCustomMesh();
    this->dyn.gravity = glm::vec3(0, -1, 0);
    deformer = new Chainmail(mesh, stacks, slices, this->dyn.pos);
    convertMeshToWorldCoords();
}

void Slime::simulate(double dt) {
    deformer->simStep(dt, dyn.vel);
	convertMeshToWorldCoords();
}

void Slime::simpleSimulate(double dt) {
}


void Slime::convertMeshToWorldCoords() {
    std::vector<glm::vec3> v;
    deformer->returnVertices(v);
    std::vector<Vertex> newMeshVertices;
    for (int i = 0; i < v.size(); i++) {
        Vertex newVert = {};
        newVert.Position = v[i];
        newMeshVertices.push_back(newVert);
    }
    mesh->updateVertices(newMeshVertices);
    mesh->updateNormals();
}

Slime::~Slime()
{
    delete mesh;
}
