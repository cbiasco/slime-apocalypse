#ifndef LOCAL_MOTION_PLANNER_H_GUARD
#define LOCAL_MOTION_PLANNER_H_GUARD

#include "BoundingVolume.hpp"
#include "BVH.hpp"
#include "Object.hpp"//TODO dalton: try to factor away
#include "PRM.hpp"//TODO dalton: replace with CSpace2D

#pragma warning(push, 0)
#pragma warning(disable: 4201)
#include <glm/glm.hpp>
#pragma warning(pop)

namespace LMP {
    float ttc(BoundingVolume * i, glm::vec2 iv, BoundingVolume * j, glm::vec2 jv);
    float ttc_(Circ * i, glm::vec2 iv, Circ * j, glm::vec2 jv);
    float ttc_(Rect * i, glm::vec2 iv, Rect * j, glm::vec2 jv);
    float ttc_(Circ * i, glm::vec2 iv, Rect * j, glm::vec2 jv);

    glm::vec2 lookahead(Object * a, glm::vec2 target);

    glm::vec2 ttc_forces_(double ttc, glm::vec2 dir);
    glm::vec2 ttc_forces(Object * a, Circ * b, float ttc);
    glm::vec2 ttc_forces(Object * a, Object * b, float ttc);
    glm::vec2 calc_sum_force(Object * a,
        BVH * static_bvh,
        BVH * dynamic_bvh,
        std::vector<Object *> statics,//shouldn't be here; temp hack
        std::vector<Object *> dynamics,//shouldn't be here; temp hack
        std::vector<Object *> leaders);//shouldn't be here; temp hack
}

#endif//LOCAL_MOTION_PLANNER_H_GUARD