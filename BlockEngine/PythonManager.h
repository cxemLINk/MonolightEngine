#pragma once

#define pystring wchar_t*

#include "osbasics.h"
#ifdef WINDOWS
inline const pystring uToPyString(ustring s) {
	pystring ret = new wchar_t[s.length() + 1];
	s.copy(ret, s.length());
	ret[s.length()] = '\0';
	return ret;
}

inline void pystrFree(const pystring s) {
	delete[] s;
}
#elif
inline const pystring uToPyString(ustring s) {
	return Py_DecodeLocale(s.c_str(), NULL);
}

inline void pystrFree(const pystring s) {
	PyMem_RawFree(s);
}
#endif

namespace PythonManager
{

	void init();
	void destroy();

};

