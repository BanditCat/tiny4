#pragma once

extern "C" __declspec(dllexport) int _fltused;
extern "C" __declspec(dllexport) HWND setup(HICON, HINSTANCE hinst, char* title, int x, int y, int width, int height, BYTE type, DWORD flags);