#ifndef DEBUG_H_GUARD
#define DEBUG_H_GUARD
#pragma once

#ifdef DEBUG
#define D(x) x
//http://stackoverflow.com/questions/1486904/how-do-i-best-silence-a-warning-about-unused-variables
template <class... Args> inline void UNUSED(Args&&...) {}//TODO: make sure this compiles in NDEBUG

#include <iostream>
#include <string>
#include <chrono>
#include <thread>
void OK();
void slow_print(int middle, int after, std::string txt); // just for fun
#else
#define D(x)
#endif // DEBUG

#endif // DEBUG_H_GUARD
