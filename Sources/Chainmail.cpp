#include "Chainmail.hpp"

using std::vector;
using glm::ivec2;
using glm::vec2;
using glm::vec3;
using std::min;
using std::max;

/* Taken from http://stackoverflow.com/questions/11720656/modulo-operation-with-negative-numbers/19288271#19288271 */
int mod(int a, int b)
{
	int r = a % b;
	return r < 0 ? r + b : r;
}

/*
Our chainmail deformation used Sarah Gibson's Paper, "3D ChainMail: a Fast Algorithm for Deforming Volumetric Objects"
As a base idea for implementation.
*/
Chainmail::Chainmail(Mesh * mesh, int stacks, int slices, glm::vec3 worldCenter) {
    worldCoordCenter = worldCenter;
    this->vertexLength = mesh->vertices.size();
    for (Vertex v : mesh->vertices) {
        Element e;
        this->stacks = stacks;
        this->slices = slices;
        e.id = this->elements.size();
        e.origin = v.Position;
        e.pos = v.Position;
        elements.push_back(e);
    }
    // Add a centroid element
    /*
    Element center;
    center.id = this->elements.size();
    center.origin = vec3(0);
    center.pos = worldCoordCenter;
    for (Element & e : this->elements) {
        center.neighbors.insert(e.id);
        e.neighbors.insert(center.id);
    }

    this->elements.push_back(center);
    */

    //Random distribution of points in sphere
    /*
    for (int i = 0; i < 100; i++) {
        double maxDist = glm::distance(worldCenter, elements[0].pos);
        double x = ((float)rand()) / RAND_MAX * maxDist - (maxDist / 2);
        double y = ((float)rand()) / RAND_MAX * maxDist - (maxDist / 2);
        double z = ((float)rand()) / RAND_MAX * maxDist - (maxDist / 2);
        while (glm::distance(vec3(x, y, z), worldCenter) > maxDist) {
            x = ((float)rand()) / RAND_MAX * maxDist - (maxDist / 2);
            y = ((float)rand()) / RAND_MAX * maxDist - (maxDist / 2);
            z = ((float)rand()) / RAND_MAX * maxDist - (maxDist / 2);
        }
        Element e = {};
        e.id = elements.size();
        e.origin = vec3(x, y, z);
        e.pos = e.origin;
        e.updated = false;
        double someDist = maxDist / 2;
        for (int i = 0; i < elements.size(); i++) {
            if (glm::distance(elements[i].pos, e.pos) < someDist) {
                elements[i].neighbors.insert(e.id);
                e.neighbors.insert(i);
            }
        }
        elements.push_back(e);
    }
    */
    //lattice work for internal structure
    /*
    for (int i = 0; i < stacks; i++) {
        for (int j = 0; j < slices; j++) {
            int idx1 = j + ((stacks - i - 1) * slices);
            int idx2 = (slices - j) + (i * slices);
            int idx3 = (slices - j) + ((stacks - i - 1) * slices);
            int me = j + i * slices;
            
            if (idx1 < elements.size() && (glm::distance(elements[idx1].pos, elements[me].pos) > .003) && 
                (idx1 != me && std::find(elements[idx1].neighbors.begin(), elements[idx1].neighbors.end(), me) != elements[idx1].neighbors.end())) {
                elements[idx1].neighbors.insert(me);
                elements[me].neighbors.insert(idx1);
            }
            
            if (idx2 < elements.size() && (glm::distance(elements[idx2].pos, elements[me].pos) > .003) &&
                (idx2 != me && std::find(elements[idx2].neighbors.begin(), elements[idx2].neighbors.end(), me) != elements[idx2].neighbors.end())) {
                elements[idx2].neighbors.insert(me);
                elements[me].neighbors.insert(idx2);
            }
            
            if ((idx3 < elements.size()) && (glm::distance(elements[idx3].pos, elements[me].pos) > .003) &&
                (idx3 != me && std::find(elements[idx3].neighbors.begin(), elements[idx3].neighbors.end(), me) != elements[idx3].neighbors.end())) {
                elements[idx3].neighbors.insert(me);
                elements[me].neighbors.insert(idx3);     
            }
        }        
    }
    */
    generateRegions();
}



Chainmail::~Chainmail() {}

glm::vec3 Chainmail::returnWorldPos() {
    return worldCoordCenter;
}



// Moves an element and readies its neighbors for propogation
void Chainmail::applyMove(vec3 t) {
    applyMove(t, 0);
}

void Chainmail::applyMove(vec3 t, double dt) {
	if (t == vec3(0)) {
		return;
	}

	for (Element &e : this->elements) {
		float mag = glm::dot(e.origin, t);
		if (mag <= 0)
			continue;

		e.pos += (vec3(t[0] * dt, t[1] * dt, t[2] * dt)*mag);
		e.updated = true;

		for (int i : e.neighbors)
			waiting.push_back(ivec2(e.id, i));
	}
}


//updates the center of the object to ensure the 'center' is the origin;
void Chainmail::updateCenter() {
    int subCopies = 0;
    glm::vec3 oldCenter = worldCoordCenter;
    glm::vec3 newCenter = glm::vec3(0);
    newCenter += (worldCoordCenter + elements[0].pos);
    for (int i = 1; i < elements.size()-1; i++) {
        if (i % stacks != 0) {
            newCenter += (worldCoordCenter + elements[i].pos);
        }
        else {
            subCopies++;
        }
    }
    newCenter += (worldCoordCenter + elements[elements.size()-1].pos);
    newCenter /= (elements.size()-subCopies);
    glm::vec3 delta = oldCenter - newCenter;
    worldCoordCenter = newCenter;
    for (int i = 0; i < elements.size(); i++) {
        elements[i].pos += delta;
    }
}

void Chainmail::returnVertices(vector<vec3> &returnTo) {
    vector<Vertex> newVertices;
    for (int i = 0; i < vertexLength; i++) {
        returnTo.push_back(elements[i].pos);
    }
}

// Propagates the changes from elements moving to their neighbors
void Chainmail::propagate() {
	while (!waiting.empty()) {
		ivec2 v = waiting.front();
		waiting.pop_front();
		Element * sponsor = &this->elements[v.x];
		Element * e = &this->elements[v.y];
        
        if (e->updated) continue;

		if (v.x > v.y)
			v = ivec2(v.y, v.x);
		Cuboid c = this->regions[v]; // this should never throw an error if done right
		vec3 minBounds = sponsor->pos + c.min;

		vec3 maxBounds = sponsor->pos + c.max;
		if (e->pos.x < minBounds.x) {
			e->pos.x = minBounds.x;
			e->updated = true;
		}
		else if (e->pos.x > maxBounds.x) {
			e->pos.x = maxBounds.x;
			e->updated = true;
		}
		if (e->pos.y < minBounds.y) {
			e->pos.y = minBounds.y;
			e->updated = true;
		}
		else if (e->pos.y > maxBounds.y) {
			e->pos.y = maxBounds.y;
			e->updated = true;
		}
		if (e->pos.z < minBounds.z) {
			e->pos.z = minBounds.z;
			e->updated = true;
		}
		else if (e->pos.z > maxBounds.z) {
			e->pos.z = maxBounds.z;
			e->updated = true;
		}

		if (e->updated)
			for (int i : this->elements[e->id].neighbors)
                if (!this->elements[i].updated) {
                    waiting.push_back(ivec2(e->id, i));
                }
	}
}

// Brings the state closer to equilibrium
// NOTE: This might be wrong. It's possibly supposed to be sequential,
// but it's currently implemented as a parallel update.
void Chainmail::relax(float dt) {
	// First, generate all relaxation centroids
	vector<vec3> centroids;
	for (Element e : this->elements) {
		vector<vec3> Q;
		for (int i : e.neighbors) {
			Q.push_back(this->elements[i].pos +
						 (e.origin - this->elements[i].origin));
		}
		vec3 c = vec3(0);
		for (vec3 v : Q)
			c += v;
        if (Q.size() != 0)
		    c /= Q.size();
        else {
            c = e.origin;
        }
		centroids.push_back(c);
	}

	// Second, push all elements toward their respective centroid
	for (Element & e : this -> elements) {
		vec3 v = centroids[e.id] - e.pos;
		e.pos += 3*dt*v;
	}
}

void Chainmail::resetMesh() {
    for (int i = 0; i < elements.size(); i++) {
        elements[i].pos = elements[i].origin;
    }
}

void Chainmail::simpleUpdateCenter(glm::vec3 t, double dt) {
    worldCoordCenter += t * (float)dt;

}


void Chainmail::simpleSimStep(int id, glm::vec3 t, double dt) {
    resetMesh();
    simpleUpdateCenter(t, dt);
}
void Chainmail::simStep(double dt, glm::vec3 t) {
	applyMove(t, dt);
    propagate();
    relax(dt);

    updateCenter();

	for (Element & e : this->elements)
		e.updated = false;
	waiting.clear(); // might want a more robust end check than this
}
//Precompute the bounding regions for each element and their neighbors
void Chainmail::generateRegions() {
	for (Element e : this->elements) {
		for (int nId : e.neighbors) {
			int minId = min(e.id, nId);
			int maxId = max(e.id, nId);
			ivec2 key = ivec2(minId, maxId);
			if (this->regions.find(key) == this->regions.end()) {
				Cuboid c;
				Element eN = this->elements[nId];
				double dX, dY, dZ;
				// TODO: test that this logic is correct
                dX = abs(eN.origin.x - e.origin.x);
                dY = abs(eN.origin.x - e.origin.x);
                dZ = abs(eN.origin.x - e.origin.x);
				c.min = vec3(this->aMin*dX - this->b*(dY + dZ),
							 this->aMin*dY - this->b*(dZ + dX),
							 this->aMin*dZ - this->b*(dX + dY));
				c.max = vec3(this->aMax*dX + this->b*(dY + dZ),
							 this->aMax*dY + this->b*(dZ + dX),
							 this->aMax*dZ + this->b*(dX + dY));

				this->regions[key] = c;
			}
		}
	}
}