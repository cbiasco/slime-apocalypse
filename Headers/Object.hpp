#ifndef OBJECT_H_GUARD
#define OBJECT_H_GUARD

#include <vector>
#include <algorithm> // min, max
#include "Structs.hpp"
#include "BoundingVolume.hpp"
#include "Mesh.hpp"
#include "Chainmail.hpp"

#pragma warning(push, 0)
#pragma warning(disable: 4055)
#include <glad/glad.h>
#pragma warning(disable: 4201)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#pragma warning(pop)

class Object {
public:
	Object();
    Object(glm::vec2 p);
	Object(float x, float y);
	Object(glm::vec3 p);
    Object(float x, float y, float z);
	virtual ~Object();

	virtual void setParams(float x);
	virtual void setParams(float x, float y);
	virtual void setParams(float x, float y, float z);

	void setColor(float r, float g, float b);
	void setColor(glm::vec3 rgb);

	// Simple movement functions; we'll need to adapt these to however our objects move
	virtual void moveBy(float x, float y, float z);
    virtual void moveBy(glm::vec3 t);
    virtual void moveBy(glm::vec3 t, float dt);
	virtual void moveTo(float x, float y, float z);
	virtual void moveTo(glm::vec3 position);

	virtual void constructStandardMesh(bool override = false) = 0;
	virtual void useStandardMesh() = 0;
	virtual void useCustomMesh() = 0;
    virtual void simulate(double dt) = 0;
    virtual void simpleSimulate(double dt) = 0;

	void draw(Shader * shader);

	// Member variables
    BoundingVolume * bv;
    ai_comp ai;
    dynamics_comp dyn;

	glm::vec3 params;
	glm::vec3 color = glm::vec3(0, 0, 1);

	// Rendering variables
	Mesh * mesh;
    Chainmail * deformer;
};

#endif // OBJECT_H_GUARD