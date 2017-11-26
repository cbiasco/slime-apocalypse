#ifndef STRUCTS_H_GUARD
#define STRUCTS_H_GUARD

#pragma warning(push, 0)
#pragma warning(disable: 4055)
#include <glad/glad.h>
#include <glm/glm.hpp>
#pragma warning(pop)

#include "graph.hpp"
#include "PRM.hpp"

/*
TODO:
I'm going to split these up into more headers, as sometimes the POD structs/components I make are better
suited with having functions
*/

// Moved here to allow both Scene and main to access it
namespace G {
	const GLint WIN_WIDTH = 1200;
	const GLint WIN_HEIGHT = 800;
    //note: WIN_WIDTH and WIN_HEIGHT must match your screen res for fullscreen; else lag
	const GLboolean WIN_FULLSCREEN = GL_FALSE;
}

//data container Scene's maze creation algorithm.
struct mazeData {
    int height;
    int width;
    float cellSize;
    float chanceGennedAlive;
    int maxEnemies;
    float enemySize;
    glm::vec3 center;
};

struct mazeCell {
    int filled;
    bool active;
};

struct bufferContainer {
	GLuint scene_vao;
	GLuint position_vbo;
	GLuint colors_vbo;
	GLuint faces_ibo;
	GLuint edges_ibo;
	GLuint normals_vbo;
};

//used by any dynamics module (physics & some local motion planning)
struct dynamics_comp {
    glm::vec3 pos;
    glm::vec3 vel;
    glm::vec3 acc;
    glm::vec3 mass;
    glm::vec3 force;
    glm::vec3 gravity;
};

//used by the motion planning module (mostly global MP)
struct ai_comp {
    /* LEADlings attract PACKlings and plan independently
       PACKlings attract PACKlings and BOIDlings
       BOIDlings attract BOIDlings and PACKlings
       INDIE plans independently */
    enum class Planner {LEAD, PACK, BOID, INDY, NONE};
    Planner method;

    //this allows more dynamic path planning
    glm::vec2 final_goal;//global goal (e.g. player)
    glm::vec2 goal;//local goal (e.g. the next node)

    int num_done;
    VecData * plan;

    Cspace2D * cspace;
    PRM * prm;

    bool has_plan() {
        return plan != nullptr && plan->size() > static_cast<size_t>(0);
    };
    bool has_boid_f() {
        return method == ai_comp::Planner::BOID || method == ai_comp::Planner::PACK;
    };
    bool has_indy_f() {
        return method == ai_comp::Planner::LEAD || method == ai_comp::Planner::INDY;
    };
    bool is_agent() {
        return method != ai_comp::Planner::NONE;
    };
};

#endif // STRUCTS_H_GUARD