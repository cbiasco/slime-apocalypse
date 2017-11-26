#ifndef MAIN
#define MAIN
#pragma once

/*
libraries used:
glad: an OpenGL function loader: https://github.com/Dav1dde/glad
glfw: a library for I/O and OpenGL context creation: http://www.glfw.org/
glm: OpenGL mathematics: http://glm.g-truc.net/0.9.8/index.html
stb: image loading: https://github.com/nothings/stb/blob/master/stb_image.h
(maybe) assimp: model loading: https://github.com/assimp/assimp
*/

/* LOCALS */
#include "debug.hpp"
#include "UI.hpp"
#include "Shader.hpp"
#include "Timer.hpp"
#include "AI.hpp"
#include "Scene.hpp"

/* GLOBALS */
/* GL */
#pragma warning(push, 0)
#pragma warning(disable: 4055)
#include <glad/glad.h>
#include <GLFW/glfw3.h>
//#ifndef STB_IMAGE_IMPLEMENTATION
//#define STB_IMAGE_IMPLEMENTATION
//#endif // STB_IMAGE_IMPLEMENTATION
//#include <stb_image.h>
//#include <assimp/Importer.hpp>
//#include <assimp/postprocess.h>
//#include <assimp/scene.h>
/* STL */
#include <string>
#pragma warning(pop)

GLFWwindow * init_window_context();
int kill_app(int retVal);

void handle_input(Gtime::Timer * clock, Scene * scene);

/* Globals */
Scene * scene;

#endif //MAIN
