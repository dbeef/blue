//
// Created by dbeef on 4/20/19.
//

#pragma once

#ifndef NDEBUG

#include <iostream>

static void debug_break()
{
#if defined BLUE_WINDOWS
	__debugbreak;
#else
	std::cin.ignore();
#endif
}

static void logAssert(const char* assert, const char* file, int line)
{
	std::cout << "Assertion failed: " << assert << " at " << file << " line " << line << std::endl;
}

#define BLUE_ASSERT(x) \
	if(!(x)) \
    { \
      logAssert(#x, __FILE__, __LINE__);\
	  debug_break(); \
    }

#else
#define BLUE_ASSERT(x) \
x;
#endif
