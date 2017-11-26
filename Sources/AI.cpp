#include "AI.hpp"

Cspace2D * ai::std_cspace;
PRM * ai::std_prm;
BVH * ai::static_bvh;
BVH * ai::dynamic_bvh;

//todo: figure out how to handle varying size Cspace
void ai::init(std::vector<Object *> dynamics, std::vector<Object *> statics, mazeData map) {
    static const float root2 = static_cast<float>(sqrt(2));

    ai::static_bvh = new BVH(statics);
    ai::dynamic_bvh = new BVH(dynamics);
    
    for (Object * o : statics) {
        o->ai.method = ai_comp::Planner::NONE;
    }
    if (dynamics.size() > static_cast<size_t>(0)) {
        Gtime::add_now();
        std::vector<BoundingVolume *> obs_bv;
        for (Object * s : statics)
            obs_bv.push_back(s->bv);
        ai::std_cspace = new Cspace2D(obs_bv, dynamics[0]->bv);
        std::cout << "cspace:" << Gtime::del_top() << std::endl;

        Gtime::add_now();
        glm::vec2 center_2d(map.center.x, map.center.z);
        glm::vec2 dim(map.width/2, map.height/2);
        dim *= map.cellSize;
        ai::std_prm = new PRM(
            ai::std_cspace,
            2 * root2 * map.cellSize,
            0.f,
            glm::vec2(map.cellSize, map.cellSize),
            1,
            center_2d - dim,
            center_2d + dim,
            /*1.f*/.1f);
        std::cout << "PRM:" << Gtime::del_top() << std::endl;

        for (Object * o : dynamics) {
            o->ai.method = ai_comp::Planner::INDY;
            o->ai.plan = new VecData();
            o->ai.num_done = 0;
            o->ai.cspace = ai::std_cspace;
            o->ai.prm = ai::std_prm;
            o->ai.final_goal = glm::vec2(0, 0);
        }
    }
}

//move to AI/planner --- this is a force-based LMP
void ai::update_agents(std::vector<Object *> statics, std::vector<Object *> dynamics, Object * player) {
    //if (ai::static_bvh->size() != statics.size()) {
        delete ai::static_bvh;//probably very slow
        ai::static_bvh = new BVH(statics);
        //TODO: update PRM
    //}
    delete ai::dynamic_bvh;
    ai::dynamic_bvh = new BVH(dynamics);
    //std::vector<Object *> leaders;
    //for (Object * d : dynamics)
    //    if (d->ai.method == ai_comp::Planner::LEAD)
    //        leaders.push_back(d);
    int replanned = 0;
    for (Object * d : dynamics) {
        if (d->ai.is_agent()) {
            d->ai.final_goal = glm::vec2(player->dyn.pos.x, player->dyn.pos.z);
            if(d->ai.has_indy_f() && GMP::invalid(d)) {
                if (d->ai.cspace->line_of_sight(d->bv->o, d->ai.final_goal)) {
                    //delete d->ai.plan;
                    //d->ai.plan = new VecData();
                    //std::cout << "[--GMP] ";
                    d->ai.num_done = d->ai.plan->size() - 1;
                    d->ai.goal = d->ai.final_goal;
                }
                else if (replanned++ < 2) {
                    std::cout << "[GMP] ";
                    GMP::plan_one(d);
                }
            }
            glm::vec2 f2d = LMP::calc_sum_force(d, ai::static_bvh, ai::dynamic_bvh, statics, dynamics, std::vector<Object *>());
            d->dyn.force += glm::vec3(f2d.x, 0, f2d.y);
        }
    }
}
