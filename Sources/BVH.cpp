#include "BVH.hpp"
#include "debug.hpp"

using namespace std;

BVH::BVH() {
    left = nullptr;
    o = nullptr;
    right = nullptr;
}

BVH::BVH(vector<Object *> objects) {
    this->size_ = objects.size();
    if (objects.size() == 0) {
        this->o = nullptr;
        this->left = nullptr;
        this->right = nullptr;
        return;
    }

    //create two index arrays
    vector<Index> sorted_x = vector<Index>(objects.size());
    vector<Index> sorted_z = vector<Index>(objects.size());
    for (size_t i = 0; i < objects.size(); i++)
        //for sorted_a Index is (idx of obj for x, idx of zidx)
        //for sorted_b Index is (idx of obj for z, idx of xidx)
        //so .obj will always grab the object,
        //.oth the other table's index to the same object
        sorted_x[i] = sorted_z[i] = Index(static_cast<uint>(i), 0);

    //sort by dimension
    std::sort(sorted_x.begin(), sorted_x.end(), [objects](Index a, Index b) {
        return objects[a.obj]->bv->o.x < objects[b.obj]->bv->o.x;
    });
    for (size_t i = 0; i < sorted_x.size(); i++)//connect index tables
        sorted_z[sorted_x[i].obj].oth = static_cast<uint>(i);

    std::sort(sorted_z.begin(), sorted_z.end(), [objects](Index a, Index b) {
        return objects[a.obj]->bv->o.y < objects[b.obj]->bv->o.y;
    });
    for (size_t i = 0; i < sorted_z.size(); i++)//connect index tables
        sorted_x[sorted_z[i].oth].oth = static_cast<uint>(i);

    construct_(objects, sorted_x, sorted_z);
    ;
}

void BVH::construct_(vector<Object *> objects, vector<Index> sorted_x, vector<Index> sorted_z) {
    D(assert(sorted_x.size() == sorted_z.size()));
    this->size_ = sorted_x.size();
    D(assert(sorted_x.size() >= 1));
    if (sorted_x.size() == static_cast<size_t>(1)) {
        D(assert(sorted_x[0].obj == sorted_z[0].obj));
        this->right = nullptr;
        o = objects[sorted_x[0].obj];
        return;
    }

    //create bounding volume for this level, remembering the min/max in each dim
    float min_x, max_x, min_z, max_z;
    min_x = min_z = std::numeric_limits<float>::max();
    max_x = max_z = -std::numeric_limits<float>::max();
    //tightly fit min_x/max_x/min_z/max_z
    for (size_t i = 0; i < sorted_x.size(); i++) {
        BoundingVolume * bv = objects[sorted_x[i].obj]->bv;
        float dim_x, dim_z;
        if (bv->vt == BoundingVolume::volume_type::RECT) {
            //add a nudge if you want fat BVs
            dim_x = static_cast<Rect *>(bv)->w/2;
            dim_z = static_cast<Rect *>(bv)->h/2;
        }
        else {
            dim_x = dim_z = static_cast<Circ *>(bv)->r;
        }

        float x_lo = bv->o.x - dim_x;
        float x_hi = bv->o.x + dim_x;
        float z_lo = bv->o.y - dim_z;
        float z_hi = bv->o.y + dim_z;
        if (x_lo < min_x) min_x = x_lo;
        if (x_hi > max_x) max_x = x_hi;
        if (z_lo < min_z) min_z = z_lo;
        if (z_hi > max_z) max_z = z_hi;
    }

    float dx = max_x - min_x;
    float dz = max_z - min_z;
    aabb = Rect(glm::vec2(min_x + dx/2, min_z + dz/2), dx, dz);

    //partition along longer axis, splitting indices equally
    vector<Index> x_rhs, x_lhs;
    vector<Index> z_rhs, z_lhs;
    if (dx > dz)
        split_(sorted_x, sorted_z, x_lhs, z_lhs, x_rhs, z_rhs);
    else
        split_(sorted_z, sorted_x, z_lhs, x_lhs, z_rhs, x_rhs);

    //probably the slowest part is the new, tbh XD
    left = new BVH();
    left->construct_(objects, x_lhs, z_lhs);
    right = new BVH();
    right->construct_(objects, x_rhs, z_rhs);
}

void BVH::split_(
        vector<Index> & sorted_a,
        vector<Index> & sorted_b,
        vector<Index> & a_lhs,
        vector<Index> & b_lhs,
        vector<Index> & a_rhs,
        vector<Index> & b_rhs) {
    D(assert(sorted_a.size() == sorted_b.size()));
    size_t half = static_cast<size_t>(sorted_a.size() / 2.);
    size_t half_ceil = static_cast<size_t>((sorted_a.size() / 2.) + .5);
    a_lhs = vector<Index>(half);
    b_lhs = vector<Index>(half);
    a_rhs = vector<Index>(half_ceil);
    b_rhs = vector<Index>(half_ceil);

    
    for (size_t i = 0; i < sorted_a.size(); i++) {
        if (i < half)
            a_lhs[i] = Index(sorted_a[i].obj, -1);
        else
            a_rhs[i - half] = Index(sorted_a[i].obj, -1);
    }
    for (size_t i = 0, lhs = 0, rhs = 0; i < sorted_b.size(); i++) {
        size_t oth_i = sorted_b[i].oth;
        if (oth_i < half) {
            b_lhs[lhs] = Index(sorted_b[i].obj, oth_i);
            D(assert(oth_i < a_lhs.size()));
            a_lhs[oth_i] = Index(a_lhs[oth_i].obj, lhs);
            lhs++;
        }
        else {
            b_rhs[rhs] = Index(sorted_b[i].obj, oth_i - half);
            D(assert(oth_i - half < a_rhs.size()));
            a_rhs[oth_i - half] = Index(a_rhs[oth_i - half].obj, rhs);
            rhs++;
        }
    }
}

BVH::~BVH() {
    if (!is_leaf()) {
        delete left;
        delete right;
    }
}

//only use Circ queries please...
vector<Object *> BVH::query(BoundingVolume * q) {
    vector<Object *> NN;
    if (left == nullptr)
        return vector<Object *>();
    //we are making terrible assumptions
    query_(dynamic_cast<Circ *>(q), &NN);
    return NN;
}
void BVH::query_(Circ * q, vector<Object *> * NN) {
    if (is_leaf()) {
        if (o != nullptr && o->bv != nullptr) {
            if (o->bv->vt == BoundingVolume::volume_type::CIRC) {
                if (circ_circ_collider_(q, static_cast<Circ *>(o->bv)))
                    NN->push_back(this->o);
            }
            else {
                if (circ_rect_collider_(q, static_cast<Rect *>(o->bv)))
                    NN->push_back(this->o);
            }
        }
    }
    else {
        if (circ_rect_collider_(q, &aabb)) {
            this->left->query_(q, NN);
            this->right->query_(q, NN);
        }
    }
}

//don't use, test later as the sqrt will probably make this slower
//this would be useful if it is faster, though, since circ/rect is common
//that would also mean I need to update my BV code.
bool BVH::circ_rect_collider_(Circ * q, Rect * r) {
    glm::vec2 L = r->o - q->o;
    L /= sqrt(glm::dot(L, L));
    L *= q->r;
    L += q->o;
    return q->is_collision(r->o) || r->is_collision(L);
}
bool BVH::circ_circ_collider_(Circ * q, Circ * c) {
    glm::vec2 diff = q->o - c->o;
    float r = q->r + c->r;
    return glm::dot(diff, diff) < r*r;
}
