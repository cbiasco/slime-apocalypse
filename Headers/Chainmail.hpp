#pragma once

#include "Mesh.hpp"
#include <vector>
#include <unordered_set>
#include <deque>
#include <unordered_map>
#include <algorithm>

#pragma warning(push, 0)
#pragma warning(disable: 4201)
#include <glm/glm.hpp>
#pragma warning(pop)

/* Taken from http://stackoverflow.com/a/9047696 */
/* Allows for hashing using ivec2 */
struct KeyFuncs
{
	size_t operator()(const glm::ivec2& k)const
	{
		return std::hash<int>()(k.x) ^ std::hash<int>()(k.y);
	}

	bool operator()(const glm::ivec2& a, const glm::ivec2& b)const
	{
		return a.x == b.x && a.y == b.y;
	}
};

struct Cuboid {
	glm::vec3 min;
	glm::vec3 max;
};

struct Element {
	int id;
	glm::vec3 origin;
	glm::vec3 pos;
	bool updated = false;
	std::unordered_set<int> neighbors;
};

class Chainmail
{
public:
	Chainmail(Mesh * mesh, int stacks, int slices, glm::vec3 worldCenter);
	virtual ~Chainmail();

    void applyMove(glm::vec3 t);
    void applyMove(glm::vec3 t, double dt);
	void propagate();
	void relax(float dt);
	void generateRegions();
    void updateCenter();
    void simStep(double dt, glm::vec3 t = glm::vec3(0));
    void simpleSimStep(int id, glm::vec3 t, double dt);
    void resetMesh();
    void simpleUpdateCenter(glm::vec3 t, double dt);
    glm::vec3 returnWorldPos();
    void returnVertices(std::vector<glm::vec3> &returnTo);
	/* variables */
	std::vector<Element> elements; // might be able to make this unordered_map with IDs
    std::vector<Element *> surfaceElements; //O
	// Waiting list of elements to be processed.
	// The first value is the sponsor, the second is the element id.
	std::deque<glm::ivec2> waiting;
	std::vector<int> moveIds;
	std::vector<int> invertIds;

	// Each region is shared between two indices, where the vec2
	// is (minIndex, maxIndex) to avoid doubling.
	// TODO: Check performance vs tuples
	std::unordered_map<glm::ivec2,Cuboid,KeyFuncs,KeyFuncs> regions;
    glm::vec3 worldCoordCenter;
    int objectDimension;
	int spaceDimension;
	double aMin = .002; // compression factor
	double aMax = 100;  // stretch factor
	double b = 100;	  // shear factor
    int stacks;//need these to omit values in 'UpdateCenter()' so it doesn't move on its own.
    int slices;
    Mesh * mesh;
	int vertexLength; // used for returning the correct model back
};

