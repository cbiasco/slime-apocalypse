/*** LOCALS ***/
#include "Timer.hpp"

using namespace Gtime;
using namespace std;

vector<Timer> Gtime::stack;

void Gtime::init_stack(GLuint layers) {
    stack = vector<Timer>(layers);
}
void Gtime::add_now() {
    stack.push_back(Timer());
}
GLfloat Gtime::del_top() {
    GLfloat elapsed = stack.back().frame();
    stack.pop_back();
    return elapsed;
}

Timer::Timer() {
    this->frame_abs = (GLfloat)glfwGetTime();
    this->last_delta = 0.f;
    this->paused_time = 0.f;
}
GLfloat Timer::frame() {
    GLfloat prev = this->frame_abs;
    this->frame_abs = (GLfloat)glfwGetTime();
    this->last_delta = this->frame_abs - prev;
    return this->last_delta;
}
GLfloat Timer::abs() {
    return this->frame_abs;
}
GLfloat Timer::delta() {
    return this->last_delta;
}
GLfloat Timer::pause() {
    this->paused_time = (GLfloat)glfwGetTime();
    return  this->paused_time - this->frame_abs;
}
GLfloat Timer::play() {
    GLfloat elapsed = (GLfloat)glfwGetTime() - this->paused_time;
    this->frame_abs += elapsed;
    return elapsed;
}
