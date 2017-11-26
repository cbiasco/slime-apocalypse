#include "LocalMotionPlanner.hpp"
#include "debug.hpp"
#include <limits>

//TODO: properly polymorph for all BoundingVolumes
float LMP::ttc(BoundingVolume * i, glm::vec2 iv, BoundingVolume * j, glm::vec2 jv) {
    //I wish there was a way I didn't have to check the types..
    Circ* c = dynamic_cast<Circ*>(i);
    if (c != nullptr) {
        Circ * c2 = dynamic_cast<Circ *>(j);
        if (c2 != nullptr)
            return ttc_(c, iv, c2, jv);

        Rect * r2 = dynamic_cast<Rect *>(j);
        if (r2 != nullptr)
            return ttc_(c, iv, r2, jv);
    }
    else {
        Rect * r = dynamic_cast<Rect*>(i);
        if (r != nullptr) {
            Rect * r2 = dynamic_cast<Rect *>(j);
            if (r2 != nullptr)
                return LMP::ttc_(r, iv, r2, jv);

            Circ * c1 = dynamic_cast<Circ *>(j);
            if (c1 != nullptr)
                return LMP::ttc_(c1, iv, r, jv);
        }
    }
    return std::numeric_limits<float>::max();
}

float LMP::ttc_(Circ * i, glm::vec2 iv, Circ * j, glm::vec2 jv) {
    /*    float r = i->r + j->r;
    glm::vec2 w = j->o - i->o;
    float w2 = glm::dot(w, w);
    float c = w2 - r * r;
    if (c < 0) {// agents are colliding
    return 0; //original

    //as per Stephen Guy's suggestion; halve the radii when colliding
    //as per Caleb Biasco's suggestion; use the smallest radii, not both
    //float smaller = (i->r > j->r ? j->r : i->r);
    //r -= smaller;
    //c = w2 - r * r;
    }    */
    Circ cspace_circ = Circ(i->o, i->r + j->r);
    return cspace_circ.intersect(j->o, jv - iv);
}

//finds the ttc via a component analysis of the velocity vectors
float LMP::ttc_(Rect * i, glm::vec2 iv, Rect * j, glm::vec2 jv) {
    Rect r = Rect(i->o, i->w + j->w, i->h + j->h);
    glm::vec2 dv = jv - iv;
    return r.intersect(j->o, dv);
}

float LMP::ttc_(Circ * i, glm::vec2 iv, Rect * j, glm::vec2 jv) {
    std::vector<BoundingVolume *> ms = j->minkowskiSum(i);
    float min_t = std::numeric_limits<float>::max();
    glm::vec2 dv = jv - iv;
    for (BoundingVolume * bv : ms) {
        float t = bv->intersect(j->o, dv);
        if (t < min_t)
            min_t = t;
        delete bv;
    }
    return min_t;
}

glm::vec2 LMP::lookahead(Object * a, glm::vec2 target) {
    glm::vec2 t_new = target;
    if (static_cast<size_t>(a->ai.num_done) < a->ai.plan->size()) {
        t_new = (*(a->ai.plan))[a->ai.num_done];

        while (static_cast<size_t>(a->ai.num_done) + 1 < a->ai.plan->size()
            && a->ai.cspace->line_of_sight(a->bv->o, (*(a->ai.plan))[a->ai.num_done + 1])) {
            a->ai.num_done++;
            t_new = (*(a->ai.plan))[a->ai.num_done];
        }
    }
    else
        t_new = (*(a->ai.plan))[a->ai.plan->size() - 1];
    return t_new;
}

glm::vec2 LMP::ttc_forces_(double ttc, glm::vec2 dir) {
    float len = glm::length(dir);
    if (len != 0)
        dir /= len;

    double t_h = 5.0;//seconds
    double mag = 0;
    if (ttc >= 0 && ttc <= t_h)
        mag = (t_h - ttc) / (ttc + 0.001);
    mag = mag > 20 ? 20 : mag;
    return glm::vec2(mag * dir.x, mag * dir.y);
}
glm::vec2 LMP::ttc_forces(Object * a, Circ * b, float ttc) {
    glm::vec2 V_dt(a->dyn.vel);
    glm::vec2 dir = (a->bv->o + V_dt - b->o);
    return ttc_forces_(ttc, dir);
}
glm::vec2 LMP::ttc_forces(Object * a, Object * b, float ttc) {
    glm::vec2 V_dt(a->dyn.vel * ttc);
    glm::vec2 bV_dt(b->dyn.vel * ttc);
    //glm::vec2 perturb(0.000001, 0.000001);
    glm::vec2 dir = (a->bv->o + V_dt - b->bv->o - bV_dt);// +perturb);
    return ttc_forces_(ttc, dir);
}

//distance to leader, weak close, strong far
glm::vec2 follow_force(Object * lead, Object * a) {
    GLfloat ff0_r = 2.0f;//radius of following force of 0
    GLfloat ff1_r = 10.0f;//radius of following force of 1 towards leader

    glm::vec2 ff;
    //todo dalton: del o ref
    glm::vec2 toLeader = lead->bv->o - a->bv->o;
    float dist2_to_leader = glm::dot(toLeader, toLeader);
    if (dist2_to_leader < ff0_r * ff0_r)
        ff = glm::vec2(0);
    else
        ff = toLeader * (dist2_to_leader - ff0_r) / (ff1_r - ff0_r);

    return ff;
}

glm::vec2 boid_force(Object * a, BVH * dynamic_bvh, std::vector<Object *> dynamics) {
    const float boid_speed = 1.2f;

    glm::vec2 avg_vel(0, 0), avg_pos(0, 0), avg_dir(0, 0);
    GLfloat cohes_r_look = 1.0f, align_r_look = 1.0f, separ_r_look = .5f;//limit to search for forces for boidlings
    glm::vec2 align_force, cohesion_force, follow_force, spread_force;

    std::vector<Object *> NNdynamic = 
        dynamic_bvh->query(new Circ(a->bv->o, 1.1f));
    for (size_t i = 0; i < NNdynamic.size(); i++) { 
        Object * boid = NNdynamic[i];
        if (!boid->ai.has_boid_f() || boid == a)
            continue;

        /* gather metrics for later forces */
        glm::vec2 dist = boid->bv->o - a->bv->o;
        float fi = static_cast<float>(i);
        if (glm::dot(dist, dist) < align_r_look * align_r_look)
            avg_vel = (avg_vel * fi + glm::vec2(boid->dyn.vel.x, boid->dyn.vel.z)) / (fi + 1.f);
        if (glm::dot(dist, dist) < cohes_r_look * cohes_r_look)
            avg_pos = (avg_pos * fi + glm::vec2(boid->bv->o)) / (fi + 1.f);


        /* Seperation force*/
        //force from inverted direction of nearest neighbours
        glm::vec2 toBoid = boid->bv->o - a->bv->o;
        float dist2_to_boid = glm::dot(toBoid, toBoid);

        if (dist2_to_boid < separ_r_look * separ_r_look)
            spread_force += -toBoid / (10 * sqrt(dist2_to_boid));
    }

    /* alignnment force */
    //average velocity; pull towards that
    float norm = glm::length(avg_vel);
    if (norm != 0)
        avg_vel /= norm;
    align_force = (avg_vel - glm::vec2(a->dyn.vel.x, a->dyn.vel.z)) * boid_speed;

    /* cohesion force */
    //average cohesion; pull towards that
    cohesion_force = avg_pos - a->bv->o;

    glm::vec2 boid_force = align_force + cohesion_force + spread_force;
    if (glm::dot(boid_force, boid_force) > 20 * 20) {
        boid_force /= glm::length(boid_force);
        boid_force *= 20.f;
    }
    return boid_force;
}

glm::vec2 LMP::calc_sum_force(
        Object * a,
        BVH * static_bvh,
        BVH * dynamic_bvh,
        std::vector<Object *> statics,
        std::vector<Object *> dynamics,
        std::vector<Object *> leaders) {
    float speed = 6.0f; // x m/s
    glm::vec2 goal_vel;
    glm::vec2 goal_F(0);

    //if there is a plan, follow it
    if (a->ai.has_plan()) {
        a->ai.goal = LMP::lookahead(a, a->ai.goal);
        goal_vel = (a->ai.goal - a->bv->o) / glm::distance(a->ai.goal, a->bv->o) * (speed /* * dt */);
        goal_F = 2.0f*(goal_vel - glm::vec2(a->dyn.vel.x, a->dyn.vel.z));
    }
    else {
        a->ai.goal = a->bv->o;
        goal_vel = glm::vec2(0);
        goal_F = glm::vec2(0);
    }

    float real_speed = glm::length(goal_vel);

    /* ttc - approximate */
    glm::vec2 ttc_F(0);
    Circ q(a->bv->o, real_speed * 5);
    std::vector<Object *> NNdynamic = dynamic_bvh->query(&q);
    for (Object * b : NNdynamic) {
        if (a == b)
            continue;
        double ttc = LMP::ttc(a->bv, glm::vec2(a->dyn.vel.x, a->dyn.vel.z), b->bv, glm::vec2(b->dyn.vel.x, b->dyn.vel.z));
        if (ttc > 4)//seconds
            continue;
        ttc_F += LMP::ttc_forces(a, b, static_cast<float>(ttc));
    }

    
    std::vector<Object *> NNstatic = static_bvh->query(&q);
    for (Object * s : NNstatic) {
        double ttc = LMP::ttc(a->bv, glm::vec2(a->dyn.vel.x, a->dyn.vel.z), s->bv, glm::vec2(0));

        if (ttc > 4)//seconds
            continue;

        ttc_F += ttc_forces(a, s, static_cast<float>(ttc));
    }

    /*
    glm::vec2 boid_F(0);
    if (a->ai.has_boid_f())
        boid_F += boid_force(a, dynamic_bvh, dynamics);

    glm::vec2 follow_F(0);
    if (a->ai.method == ai_comp::Planner::PACK) {
        for (Object * leader : leaders) {
            follow_F += follow_force(leader, a);
        }
    }
    */

    return  goal_F + ttc_F;// +boid_F + follow_F;
}
