#include "PRM.hpp"

/* uniformly bin_samps the configuration space to generate nodes for the PRM;
   All sampled points will be non-colliding with the static environment 
*/
VecPoint * PRM::sample_nodes() {
	typedef std::chrono::high_resolution_clock hrclock;
	hrclock::time_point first = hrclock::now();

	std::default_random_engine gen;
	std::uniform_real_distribution<float> std(-variance/2.f, variance/2.f);

	VecPoint * sample = new VecPoint();
	for (int i = 0; i < bin_samp; i++) {
		for (float x = lo_bound.x; x < hi_bound.x; x += bin_dim.x) {
            hrclock::duration seed = hrclock::now() - first;
            gen.seed(static_cast<unsigned int>(seed.count()));
			for (float y = lo_bound.y; y < hi_bound.y; y += bin_dim.y) {
				glm::vec2 p(
                    std(gen)*bin_dim.x + x + bin_dim.x / 2.f,
                    std(gen)*bin_dim.y + y + bin_dim.y / 2.f
                );

                bool colliding = c_space->is_collision(p);
                if (perturb != 0) {
                    while (colliding) {
                        p.x += std(gen) * 2 * perturb;
                        p.y += std(gen) * 2 * perturb;
                        p.x = std::min(std::max(p.x, lo_bound.x), hi_bound.x);
                        p.y = std::min(std::max(p.y, lo_bound.y), hi_bound.y);
                        colliding = c_space->is_collision(p);
                    }
                }

                if (!colliding)
                    sample->push_back(new Node<glm::vec2> (p, new VecPoint()));
			}
		}
	}

	return sample;
}

/* threshold search to find nearby */
VecPoint * PRM::find_nearest_neighbours(VecPoint * nodes, int targetIdx) {

	VecPoint * neighbours = new VecPoint();

	glm::vec2 t = (*nodes)[targetIdx]->data;
	for (int i = 0; i < static_cast<int>(nodes->size()); i++) {
		glm::vec2 n = (*nodes)[i]->data;
		// don't consider this node we're looking from

		if (i != targetIdx)
			if (glm::distance(t, n) < threshold)
				// push the close enough node onto the neighbours list
				neighbours->push_back((*nodes)[i]);
	}

	return neighbours;
}

/* connects nearby of each node by Graph edges */
Graph<glm::vec2> * PRM::connect_roadmap(VecPoint * nodes) {
	Graph<glm::vec2> * rm = new Graph<glm::vec2>();
	for (int i = 0; i < static_cast<int>(nodes->size()); i++)
		rm->add_vertex((*nodes)[i]);

	for (int i = 0; i < static_cast<int>(nodes->size()); i++) {
		VecPoint * nearby = find_nearest_neighbours(nodes, i);
		for (int n = 0; n < static_cast<int>(nearby->size()); n++) {
			if (this->c_space->line_of_sight((*nearby)[n]->data, (*nodes)[i]->data)) {
				// we want directed because we'll be passing over the other side during
				// the course of the outer loop
				rm->add_directed_edge((*nearby)[n], (*nodes)[i]);
			}
		}
	}

	return rm;
}

/* bin_samps and connects a Pobabilistic Road Map */
PRM::PRM(Cspace2D * c_space,
        float threshold,
        float perturb,
        glm::vec2 bin_dim,
        int bin_samp,
        glm::vec2 lo_bound,
        glm::vec2 hi_bound,
        float variance) :
    c_space(c_space),
    threshold(threshold),
    perturb(perturb),
    bin_dim(bin_dim),
    bin_samp(bin_samp),
    lo_bound(lo_bound),
    hi_bound(hi_bound),
    variance(variance){
    this->roadmap = connect_roadmap(sample_nodes());
}

PRM::~PRM() {
    delete c_space;
}

/* generates a configuartion space given a list of obstacles and agent */
Cspace2D::Cspace2D(std::vector<BoundingVolume *> obs, BoundingVolume * agent) {
    this->c_obs = std::vector<BoundingVolume *>();

    for (BoundingVolume * o : obs) {
        std::vector<BoundingVolume *> ms = agent->minkowskiSum(o);
        this->c_obs.insert(c_obs.end(), ms.begin(), ms.end());
    }
}

Cspace2D::~Cspace2D() {
    for (BoundingVolume * bv : this->c_obs)
        delete bv;
}

/* detects if a point collides with anything in the configuration space */
bool Cspace2D::is_collision(glm::vec2 p) {
    for (BoundingVolume * bv : this->c_obs)
        if (bv->is_collision(p))
            return true;//HIT
    return false;//MISS
}

/* detects if a line segment between glm::vec2 a and glm::vec2 bin_dim collides with the C-space 
 */
bool Cspace2D::line_of_sight(glm::vec2 a, glm::vec2 b) {
	glm::vec2 Lab;
	Lab.x = b.x - a.x;
	Lab.y = b.y - a.y;
	float len2 = glm::dot(Lab, Lab);

	for (BoundingVolume * bv : this->c_obs)
        if (!bv->line_of_sight(a, b, Lab, len2))
            return false;//HIT
    return true;//MISS
}
