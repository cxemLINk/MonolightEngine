#include "monolight.h"
using namespace Monolight;

#include "GameWindow.h"

#ifdef WINDOWS

#include <stdexcept>
#include <iostream>


LRESULT CALLBACK MessageHandler(HWND wnd, UINT wm, WPARAM wp, LPARAM lp);

GameWindow::GameWindow(ustring name, bool visible)
{
	static const LPCWSTR WNDCLSNAME = L"GameWindow_Windows_Class";
	WNDCLASSEX wc = { };
	HINSTANCE hinst = GetModuleHandle(NULL);
	

	wc.cbSize = sizeof(wc);
	wc.style = CS_CLASSDC;
	wc.lpfnWndProc = MessageHandler;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hinst;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = WNDCLSNAME;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc)) {
		throw std::runtime_error("Could not register window class");
	}

	HWND hwnd = CreateWindow(WNDCLSNAME,
		name.c_str(),
		WS_OVERLAPPEDWINDOW,		  //Style
		CW_USEDEFAULT, CW_USEDEFAULT, //Initial position
		800, 450,					  //Size
		NULL,						  //Parent window
		NULL,						  //Menu handle
		hinst, NULL);

	if (!hwnd) {
		throw std::runtime_error("Could not create window!");
	}

	wnd = hwnd;
	setVisible(visible);

	exitFlag = false;
	ctx = NULL;
}

GameWindow::~GameWindow()
{
	
}

#include "gpuinterface.h"
void GameWindow::makeContextCurrent()
{
	GPU::setContext(wnd);
}

void GameWindow::pollEvents()
{
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		if (msg.message == WM_QUIT) {
			std::cout << "Quit message received: 0x" << std::hex << msg.wParam << std::endl;
			exitFlag = true;
			break;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

LRESULT CALLBACK MessageHandler(HWND wnd, UINT wm, WPARAM wp, LPARAM lp)
{
	switch (wm) {
	case WM_CLOSE:
		std::cout << "Closing window" << std::endl;
		DestroyWindow(wnd);
		return 0;

	case WM_DESTROY:
		std::cout << "Destroying window" << std::endl;
		PostQuitMessage(0);
		return 0;

	default:
		return DefWindowProc(wnd, wm, wp, lp);
	}
}

void GameWindow::setVisible(bool visible) {
	ShowWindow(wnd, visible ? SW_SHOW : SW_HIDE);
	UpdateWindow(wnd);
}

void GameWindow::setSize(int width, int height) {
	SetWindowPos(wnd, NULL, 0, 0, width, height, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
}

void GameWindow::setTitle(std::wstring title) {
	SetWindowText(wnd, title.c_str());
	UpdateWindow(wnd);
}

#endif
