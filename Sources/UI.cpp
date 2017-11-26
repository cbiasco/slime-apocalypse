#include "UI.hpp"
#include "debug.hpp"
#include <iostream>

namespace UI {
   GLfloat cursor_sensitivity = 1.f;
   GLfloat cursor_edx = 0.f;
   GLfloat cursor_edy = 0.f;
   GLfloat d_scroll = 0.f;
   GLboolean keys[1024];
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    UNUSED(window, scancode, mode);
    //Press ESC to close Application
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (action == GLFW_PRESS) {
		if (key < 0) {
			D(std::cout << "Invalid Key Press!" << std::endl);
			return;
		}
        D(std::cout << "Key Pressed: " << /*glfwGetKeyName(key, key)*/ key << std::endl);
        UI::keys[key] = true;
    }
    else if (action == GLFW_RELEASE) {
		if (key >= 0)
			UI::keys[key] = false;
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    UNUSED(window);
    static GLboolean focus = false;
    static GLfloat last_x = 0.f;
    static GLfloat last_y = 0.f;
    UI::cursor_edx = 0.f;
    UI::cursor_edy = 0.f;
    if (!focus) {
		int height, width;
		glfwGetWindowSize(window, &width, &height);
		glfwSetCursorPos(window, width/2, height/2);
		last_x = (GLfloat)width/2;
		last_y = (GLfloat)height/2;
        focus = true;
		return;
    }
    UI::cursor_edx = (GLfloat)xpos - last_x;
    UI::cursor_edy = (GLfloat)ypos - last_y;
    UI::cursor_edx *= UI::cursor_sensitivity;
    UI::cursor_edy *= UI::cursor_sensitivity;
    last_x = (GLfloat)xpos;
    last_y = (GLfloat)ypos;

    int height, width;
    glfwGetWindowSize(window, &width, &height);
    glfwSetCursorPos(window, width/2, height/2);
}

void scroll_callback(GLFWwindow * window, double xoffset, double yoffset) {
    UNUSED(window, xoffset);
    UI::d_scroll = (GLfloat)yoffset;
}
