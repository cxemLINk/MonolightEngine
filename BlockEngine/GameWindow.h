#pragma once
#include "osbasics.h"
#ifdef WINDOWS
#include <Windows.h>
#endif

#include <string>

#include "monolight.h"

namespace Monolight {

	class GameWindow
	{

	public:
		GameWindow(ustring name) : GameWindow(name, false) {}
		GameWindow(ustring name, bool visible);
		~GameWindow();

		void setSize(int width, int height);
		void setTitle(ustring title);

		void setVisible(bool visible);
		void pollEvents();
		//abstract void event();

		void makeContextCurrent();

		inline bool shouldExit()
		{
			return exitFlag;
		}
	private:
		bool exitFlag;
	#if defined(WINDOWS)
		HWND wnd;
		HGLRC ctx;
	#elif defined(LINUX)

	#elif defined(MACOS)

	#endif

	};

}
