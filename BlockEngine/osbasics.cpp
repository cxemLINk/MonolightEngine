#include "monolight.h"
using namespace Monolight;

#include "osbasics.h"
#include <iostream>

static const int BUILD_DEBUG = _DEBUG;

#ifdef WINDOWS
#include <windows.h>
#elif defined(LINUX) || defined(MACOS)
#include <unistd.h>
#endif

ustring GetProgramName()
{
#ifdef WINDOWS
	wchar_t buf[MAX_PATH];
	if (!GetModuleFileName(NULL, buf, MAX_PATH))
		return L"<ERROR>";

	return std::wstring(buf);
#else
	char buf[4096];
	if (readlink("/proc/self/exe/", buf, sizeof(buf)) < 1)
		return "<ERROR>";

	return std::string(buf);
#endif
}

// This smells like memory leaks
ustring GetProgramPath()
{
	ustring s = GetProgramName();
	return ustring(s.data(), 0, s.find_last_of('\\'));
}
