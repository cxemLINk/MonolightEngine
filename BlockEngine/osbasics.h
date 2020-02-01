#pragma once

#if defined(_WIN32) || defined(_WIN64)
#define WINDOWS
#elif defined(__linux__)
#define LINUX
#elif defined(__APPLE__)
#define MACOS
#else
#error "Unknown OS"
#endif

#include <string>

// Unicode strings
// https://stackoverflow.com/a/402918
#ifdef WINDOWS

#define ustring std::wstring
#define u(s) L##s

#elif defined(LINUX) || defined(MACOS)

#define ustring std::string
#define u(s) s

#endif

ustring GetProgramName();
ustring GetProgramPath();
