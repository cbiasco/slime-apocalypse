#include "GlobalMotionPlanner.hpp"
#include "AI.hpp"

/* custom uniform cost search (aka Djikstra's search) for a PRM Graph
simplifciation of A*. (h = 0)
*/
VecData * GMP::find_path_UCS(Graph<glm::vec2> * roadmap) {
    return GMP::find_path_Astar(0, roadmap);
}

/* custom A* search for a PRM Graph */
//can't handle non-positive edges
VecData * GMP::find_path_Astar(float e, Graph<glm::vec2> * roadmap) {
    //for readability
    using namespace std;
    typedef Node<glm::vec2> * Vert;
    typedef pair<Vert, float> PQ_item;
    
    //init start/goal
    VecPoint verts = *(roadmap->vertices);
    Vert start = verts[verts.size() - 2];
    Vert goal = verts[verts.size() - 1];

    //init PQ
    auto cmp = [](PQ_item l, PQ_item r) { return l.second  > r.second; }; //normally <
    priority_queue<PQ_item, vector<PQ_item>, decltype(cmp)> pq(cmp);
    pq.push(PQ_item(start, 0.f));

    // init parent and g_cost (also used as a visited set)
    unordered_map<Vert, Vert> parent;
    unordered_map<Vert, float> g_cost;
    parent[start] = nullptr;
    g_cost[start] = 0.f;

    //while there are nodes to process
    while (!pq.empty()) {

        for (Node<glm::vec2> * v : *roadmap->vertices)
            assert(v->edges->size() < 1000);

        PQ_item cur_i = pq.top();
        Vert cur_v = cur_i.first;
        pq.pop();

        //quit if we hit the goal, we have a path
        if (cur_v == goal)
            break;
        if (cur_v->edges->size() < 1000)
        for (Node<glm::vec2> * adj : *(cur_v->edges)) {
            //new potential path = path to here + edge to adjacent
            float g_alt = g_cost[cur_v] + glm::distance(adj->data, cur_v->data);

            //if new path is better than current path to adjacent
            if (!g_cost.count(adj) || g_alt < g_cost[adj]) {
                g_cost[adj] = g_alt;
                float fcost = g_alt + e * glm::distance(goal->data, adj->data);
                //it's okay if we get multiple nodes on the PQ with diff fcosts
                //whichever has the lowest will show up first and the later ones will
                //have no effect as they have the same g_cost
                pq.push(PQ_item(adj, fcost));
                parent[adj] = cur_v;
            }
        }
    }

    //reconstruct path
    VecData * path = new VecData();
    bool complete_path = false;
    for (Vert v = goal; v != nullptr; v = parent[v]) {
        if (v == start)
            complete_path = true;
        path->insert(path->begin(), v->data);
    }
    return complete_path ? path : new VecData();
}

//move to AI/planner (probably GMP)
void GMP::replan(std::vector<Object *> agents) {
    for (Object * a : agents)
        plan_one(a);
}

static void connect_to_all(Graph<glm::vec2> * rm, glm::vec2 p, Cspace2D * cspace) {
    Node<glm::vec2> * v = new Node<glm::vec2>(p, new VecPoint());
    rm->add_vertex(v);
    for (Node<glm::vec2> * u : *rm->vertices)
        if (v != u && cspace->line_of_sight(v->data, u->data))
            rm->add_edge(v, u);
}

static void remove_last_vertex(Graph<glm::vec2> * rm) {
    Node<glm::vec2> * v = rm->vertices->back();
    for (Node<glm::vec2> * adj : *(v->edges))
        adj->edges->pop_back();//we added last from connect_to_all
    rm->vertices->pop_back();
}

void GMP::plan_one(Object * agent) {
    if (agent->ai.has_indy_f()) {
        //this is to fix the one hack in A*
        //temporarily add to a->ai.prm->roadmap the start/goal nodes
        Graph<glm::vec2> * rm_with_goal = agent->ai.prm->roadmap;


        //just do an LoS over every node for both
        connect_to_all(rm_with_goal, agent->bv->o, agent->ai.cspace);
        connect_to_all(rm_with_goal, agent->ai.final_goal, agent->ai.cspace);

        /* PATH PLANNING METHOD */
        agent->ai.plan = GMP::find_path_Astar(1.f, rm_with_goal);
        remove_last_vertex(rm_with_goal);
        remove_last_vertex(rm_with_goal);

        agent->ai.num_done = 0;
    }
}

bool GMP::invalid(Object * agent)
{
    return (agent->ai.cspace->line_of_sight(agent->bv->o, agent->ai.final_goal)
        || (agent->ai.num_done != 0 && agent->ai.plan->size() - 1 <= static_cast<size_t>(agent->ai.num_done)));
}
