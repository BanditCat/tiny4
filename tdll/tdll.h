#pragma once
#include <windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <math.h>

#include "..\\tiny.h"

__declspec(dllexport) int _fltused;
__declspec(dllexport) HWND setup(HICON, HINSTANCE hinst, char* title, int x, int y, int width, int height, BYTE type, DWORD flags);


#define say(X) MessageBoxA(NULL,X,"test",0)

void mexit(void);



typedef struct gstate {
	// Wininfo
	s32 width, height, x, y;
	int fullscreen;
	HINSTANCE hInstance;
	WNDCLASSW wc;
	HWND hWnd;
	HDC hDC;
	HGLRC hRC;
	HMODULE ll;
	int pf;
	PIXELFORMATDESCRIPTOR pfd;

	// Current message
	MSG   msg;

	// Icon
	u8* iconABits;
	u8* iconXBits;

	// time
	u64 useconds;
	u64 usecondDelta;
	f64 seconds;
	f64 secondDelta;

	// Title
	WCHAR title[1024];
	u32 titleSize;

	// Input
	int keys[256];
	int keysPressed[256];
	int keysReleased[256];

	// Private
	u64 _pcBase;
	LPCWSTR _delname;
	SHORT _ks1[256];
	SHORT _ks2[256];
	PSHORT _keys;
	PSHORT _keysOld;
	int _tobreak;
} gstate;

void gtick(gstate*);
void gdisplay(gstate*);
void genter(gstate*);
void gexit(gstate*);
