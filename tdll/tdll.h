#pragma once
#include "..\\tiny.h"
extern "C" __declspec(dllexport) int _fltused;
extern "C" __declspec(dllexport) HWND setup(HMODULE, HICON, HINSTANCE hinst, char* title, int x, int y, int width, int height, BYTE type, DWORD flags);
