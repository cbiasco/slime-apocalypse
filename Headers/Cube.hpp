#ifndef	CUBE_H_GUARD
#define CUBE_H_GUARD

#include "Object.hpp"

class Cube : public Object {
public:
	Cube();
	Cube(float w, float l, float h, float x, float y, float z);
	Cube(glm::vec3 p);
	Cube(float w, float l, float h, glm::vec3 p);
	virtual ~Cube();

    void simulate(double dt);
    void simpleSimulate(double dt);
	void constructStandardMesh(bool override = false);
	void useStandardMesh();
	void useCustomMesh();

	static Mesh * standardMesh;
	bool usingStandardMesh = true;

	// width is params[0]
	// height is params[1]
	// depth is params[2]
};

#endif // RECTANGLE_H_GUARD