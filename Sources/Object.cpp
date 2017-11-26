#include "Object.hpp"

using glm::vec2;
using glm::vec3;
using glm::mat4;

using std::vector;
using std::min;
using std::max;

Object::Object() {
	dyn.pos = vec3();
}

Object::Object(vec2 p) {
    dyn.pos = vec3(p.x, p.y, 0.);
    dyn.gravity = vec3(0, 0, 0);
}

Object::Object(float x, float y) {
	dyn.pos = vec3(x, y, 0.0);
    dyn.gravity = vec3(0, 0, 0);
}

Object::Object(vec3 p) {
	dyn.pos = p;
    dyn.gravity = vec3(0, 0, 0);
}

Object::Object(float x, float y, float z) {
    
vec3(x, y, z);
    dyn.gravity = vec3(0, 0, 0);
}

Object::~Object() {
}

void Object::setParams(float x) {
	params[0] = x;
	params[1] = x;
	params[2] = x;

	constructStandardMesh(true);
}

void Object::setParams(float x, float y) {
	params[0] = x;
	params[1] = y;
	params[2] = 1;

	constructStandardMesh(true);
}

void Object::setParams(float x, float y, float z) {
	params[0] = x;
	params[1] = y;
	params[2] = z;

	constructStandardMesh(true);
}

void Object::setColor(float r, float g, float b) {
	setColor(vec3(r, g, b));
}

void Object::setColor(vec3 rgb) {
	color = vec3(max(0.0f, min(1.0f, rgb[0])), max(0.0f, min(1.0f, rgb[1])), max(0.0f, min(1.0f, rgb[2])));
}

// Simple movement functions; we'll need to adapt these to however our objects move
void Object::moveBy(float x, float y, float z) {
	moveBy(vec3(x, y, z));
}

// Simple movement functions; we'll need to adapt these to however our objects move
void Object::moveBy(vec3 t) {
	dyn.pos += t;
    bv->o += vec2(t.x, t.z);
}

// Simple movement functions; we'll need to adapt these to however our objects move
void Object::moveTo(float x, float y, float z) {
	moveTo(vec3(x, y, z));
}


void Object::moveBy(glm::vec3 t, float dt) {
    moveBy(t * dt);
}

// Simple movement functions; we'll need to adapt these to however our objects move
void Object::moveTo(vec3 position) {
	vec3 p = position - dyn.pos;
    moveBy(p);
}

void Object::draw(Shader * shader) {
	mat4 model;
	model = mat4();
	model = glm::translate(model, dyn.pos);
	model = glm::scale(model, params);
	glUniformMatrix4fv(shader->uniform("model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(shader->uniform("material.diffuse"), 1, glm::value_ptr(this->color));
	glUniform3fv(shader->uniform("material.specular"), 1, glm::value_ptr(this->color));
	glUniform3fv(shader->uniform("material.shine"), 1, glm::value_ptr(this->color));

	mesh->draw(shader);
}
