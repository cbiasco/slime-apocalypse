#ifndef TIMER_H_GUARD
#define TIMER_H_GUARD

/*** GLOBALS ***/
/* STL */
#include <vector>

/* GLOBALS */
/* GL */
#pragma warning(push, 0)
#pragma warning(disable: 4055)
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#pragma warning(pop)

//TODO dalton: template to use either single or double point precision 
//also, I may potentially want to use chrono for ns precision.
//still playing around with this system. not sure how I feel about it yet
//With regards to namespaces, I'm still reading about what the best practices are.
namespace Gtime {
class Timer;
extern std::vector<Timer> stack;

//initialize stack to have (layers) amount of timers.
void init_stack(GLuint layers);
//create new timer on the top of the stack
void add_now();
//remove top of time stack; return top.frame
//(useful with add_now for timing a section of code in real-time)
GLfloat del_top();

class Timer {
public:
    Timer();
    //renews the time to 0, returning the time since last frame
    GLfloat frame();
    //returns the absolute time at the last frame
    GLfloat abs();
    //returns the last difference between two frames
    GLfloat delta();
    //stall timer, returns time since the last frame
    GLfloat pause();
    //unpauses, returns time elapsed over pause
    GLfloat play();
private:
    GLfloat frame_abs;
    GLfloat last_delta;
    GLfloat paused_time;
};
}
#endif // TIMER_H_GUARD
