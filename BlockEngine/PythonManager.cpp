#include "monolight.h"
using namespace Monolight;

#include "PythonManager.h"

#include "osbasics.h"
#include <Python.h>
#include <iostream>

static PyObject* CreateGameModule();
void PythonManager::init()
{
	Py_DontWriteBytecodeFlag = 1;
	Py_IgnoreEnvironmentFlag = 1;
	Py_IsolatedFlag = 1;
	Py_OptimizeFlag = 1;
	Py_NoSiteFlag = 1;
	Py_OptimizeFlag = 2;

	ustring path = GetProgramPath();
	path += u("\\pymodules\\");
	//std::cout << path << std::endl;

	const pystring wpath = uToPyString(path);
	Py_SetPath(wpath);
	pystrFree(wpath);

	PyImport_AppendInittab("game", &CreateGameModule);

	Py_InitializeEx(0);
	std::cout << "Python initialized" << std::endl;

	PyRun_SimpleString(
		"import test as t\n"
		"print(t.func())\n"
		"import game\n"
		"print( game.kill('arg 1', 2, [3], (4,)) )\n"
		"from _io import _IOBase as IOBase\n"
		"print('Imported', IOBase, 'from _io')\n"
		"print(IOBase.__doc__)\n"
	);
}

void PythonManager::destroy()
{
	int ret = Py_FinalizeEx();
	std::cout << "Python finalized " << ret << std::endl;
}


static const char* GameModuleDoc = "The main modding API";

static PyObject* game_kill(PyObject* self, PyObject* const* args, Py_ssize_t nargs)
{
	std::cout << "Killed yaself, self type = " << Py_TYPE(self)->tp_name << std::endl;
	for (int i = 0; i < nargs; i++)
		std::cout << "    Arg " << i << ": " << Py_TYPE(args[i])->tp_name << std::endl;

	Py_RETURN_NONE;
}

static PyMethodDef GameMethods[] = {
	{"kill", (PyCFunction)game_kill, METH_FASTCALL, "Kill yoself"},
	{NULL, NULL, 0, NULL}
};

static PyModuleDef GameModule = {
	PyModuleDef_HEAD_INIT, "game", GameModuleDoc, -1, GameMethods,
	NULL, NULL, NULL, NULL
};

static PyObject* CreateGameModule()
{
	return PyModule_Create(&GameModule);
}


