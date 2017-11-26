#ifndef PRM_H_GUARD
#define PRM_H_GUARD

#include "Graph.hpp"
#include "BoundingVolume.hpp"

#pragma warning(push, 0)
#include <random>
#include <chrono>
#include <utility>
#include <queue>
#include <unordered_map>
#pragma warning(disable: 4201)
#include <glm/glm.hpp>
#pragma warning(pop)

typedef std::vector<Node<glm::vec2> *> VecPoint;
typedef std::vector<glm::vec2> VecData;

//TODO dalton: extract CSspace2D into new .c/hpp
class Cspace2D {
private:
public:
    std::vector<BoundingVolume *> c_obs;
	Cspace2D(std::vector<BoundingVolume *> obs, BoundingVolume * agent);
    ~Cspace2D();
	bool is_collision(glm::vec2 a);
	bool line_of_sight(glm::vec2 a, glm::vec2 b);
};

class PRM {
private:
    Cspace2D * c_space;
	VecPoint * sample_nodes();
	VecPoint * find_nearest_neighbours(VecPoint * nodes, int targetIdx);
	Graph<glm::vec2> * connect_roadmap(VecPoint * nodes);

    float threshold;
    float perturb;
    glm::vec2 bin_dim;
    int bin_samp;
    glm::vec2 lo_bound;
    glm::vec2 hi_bound;
    float variance;
public:
	Graph<glm::vec2> * roadmap;
	PRM(Cspace2D * c_space,
        float threshold, 
        float perturb,
        glm::vec2 bin_dim,
        int bin_samp,
        glm::vec2 lo_bound,
        glm::vec2 hi_bound,
        float variance);
    ~PRM();
};

#endif // PRM_H_GUARD
