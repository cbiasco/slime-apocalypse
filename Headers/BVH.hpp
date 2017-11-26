#ifndef BVH_H_GUARD
#define BVH_H_GUARD

#include "Object.hpp"
#include "BoundingVolume.hpp"
#include <vector>

class BVH {
public:
    BVH();
    BVH(std::vector<Object *> objects);
    ~BVH();

    std::vector<Object *> query(BoundingVolume * q);
private:
    //terrible assumptions
    void query_(Circ * q, std::vector<Object *> * NN);
    bool circ_rect_collider_(Circ * c, Rect * r);
    bool circ_circ_collider_(Circ * c, Circ * r);
public:
    bool is_leaf() {
        return right == nullptr;
    }
    size_t size() {
        return this->size_;
    }
    
    union {
        BVH * left;
        Object * o;
    };
    BVH * right;
    Rect aabb;

private:
    size_t size_;

    typedef unsigned int uint;
    struct Index {
        uint obj;
        uint oth;
        Index() { obj = 0; oth = 0; };
        Index(uint obj, uint oth) { this->obj = obj; this->oth = oth; }
    };
    void construct_(
        std::vector<Object *> objects,
        std::vector<Index> sorted_x,
        std::vector<Index> sorted_z);
    void split_(
        std::vector<Index> & sorted_a,
        std::vector<Index> & sorted_b,
        std::vector<Index> & a_lhs,
        std::vector<Index> & b_lhs,
        std::vector<Index> & a_rhs,
        std::vector<Index> & b_rhs);
};

//http://www.randygaul.net/2013/08/06/dynamic-aabb-tree/
//to avoid excessive use of the heap and indirection
//also, he mentions the use of an AVL tree to maintain balance
/*
struct BVH_node {
    static const int Null = -1;
    bool is_leaf() {
        return right == Null;
    }
    Rect aabb;
    union {
        int parent;
        int next;
    };
    union {
        struct {
            int left;
            int right;
        };
        Object * o;
    };
    int height;
};
*/
#endif BVH_H_GUARD