#ifndef AI_H_GUARD
#define AI_H_GUARD

#include "Object.hpp"
#include "PRM.hpp"
#include "BoundingVolume.hpp"
#include "GlobalMotionPlanner.hpp"
#include "LocalMotionPlanner.hpp"
#include "Structs.hpp"
#include "BVH.hpp"

namespace ai {
    extern Cspace2D * std_cspace;
    extern PRM * std_prm;
    //todo dalton: deprecate the following three in favor of spatial struct
    extern BVH * static_bvh;
    extern BVH * dynamic_bvh;
    void init(std::vector<Object *> dynamics, std::vector<Object *> statics, mazeData map);
    void update_agents(std::vector<Object *> statics, std::vector<Object *> dynamics, Object * player);
};

#endif // AI_H_GUARD