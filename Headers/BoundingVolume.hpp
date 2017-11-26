#ifndef BOUNDING_VOLUME_H_GUARD
#define BOUNDING_VOLUME_H_GUARD

#pragma warning(push, 0)
#include <vector>
#pragma warning(disable: 4201)
#include <glm/glm.hpp>
#pragma warning(pop)

class Circ;
class Rect;

//either a cylindrical BV or an axis-aligned rectangular BV.
class BoundingVolume {
public:
    enum class volume_type { CIRC, RECT };
    //TODO dalton: remove origin, if possible
    glm::vec2 o;//origin
    volume_type vt;//what type this is instantiated as; manual polymorphism
    virtual bool is_collision(glm::vec2 p) = 0;
    virtual bool line_of_sight(glm::vec2 a, glm::vec2 b, glm::vec2 Lab, float len2) = 0;
    virtual float intersect(glm::vec2 bo, glm::vec2 v) = 0;
    virtual std::vector<BoundingVolume *> minkowskiSum(BoundingVolume *) = 0;
};

class Rect : public BoundingVolume {
public:
    Rect();
    Rect(glm::vec2 o, float w, float h);
    float w;//width
    float h;//height
    bool is_collision(glm::vec2 p);
    bool line_of_sight(glm::vec2 a, glm::vec2 b, glm::vec2 Lab, float len2);
    float intersect(glm::vec2 bo, glm::vec2 v);
    std::vector<BoundingVolume *> minkowskiSum(BoundingVolume *);
    std::vector<BoundingVolume *> minkowskiSum_(Rect * b);
    std::vector<BoundingVolume *> minkowskiSum_(Circ * b);
private:
    /* broken : but it asks if the lines cross */
    //deprecated
    bool lineSegCollision(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, glm::vec2 p4);
    /*
    axis = 0: the line is x = val {oValLo < y < oValHi} // vertical
    axis = 1: the line is y = val {oValLo < x < oValHi} // horizontal
    asks if pp1->pp2 crosses the above axial-line
    */
    //deprecated
    bool axialLineSegLineSegCollision(glm::vec2 pp1, glm::vec2 pp2, float val, int axis, float oValLo, float oValHi);
};

class Circ : public BoundingVolume {
public:
    Circ();
    Circ(glm::vec2 o, float r);
    float r;//radius
    bool is_collision(glm::vec2 p);
    bool line_of_sight(glm::vec2 a, glm::vec2 b, glm::vec2 Lab, float len2);
    float intersect(glm::vec2 bo, glm::vec2 v);
    std::vector<BoundingVolume *> minkowskiSum(BoundingVolume *);
    std::vector<BoundingVolume *> minkowskiSum_(Rect * b);
    std::vector<BoundingVolume *> minkowskiSum_(Circ * b);
};

#endif//BOUNDING_VOLUME_H_GUARD