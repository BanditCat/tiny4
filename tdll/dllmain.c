// dllmain.cpp : Defines the entry point for the DLL application.
#include "tdll.h"


#define IconSize 32



gstate *state = NULL;


void mexit(void);
void* pmalloc(size_t sz) {
	void* ret = (void*)GlobalAlloc(0, sz);
	if (ret == NULL)
		mexit();
	return ret;
}
#define malloc pmalloc
void pfree(void* p) {
	GlobalFree(p);
}
#define free pfree

u64 nmrand = 0;

void smrand(u64 s) {
	nmrand = s;
}
u32 mrand(void) {
	nmrand = nmrand * 6364136223846793005 + 1442695040888963407;
	return nmrand >> 32;
}

void enter(void) {
	if (state == NULL) {
		state = (gstate*)malloc(sizeof(gstate));
		for (u32 i = 0; i < 256; ++i) {
			state->_ks1[i] = state->_ks2[i] = 0;
			state->keys[i] = state->keysReleased[i] = state->keysPressed[i] = 0;
		}
		state->_keys = state->_ks1;
		state->_keysOld = state->_ks2;
		state->_tobreak = 0;

		state->iconABits = (u8*)malloc(IconSize * IconSize);
		state->iconXBits = (u8*)malloc(IconSize * IconSize * 4);
		LARGE_INTEGER pc, pf;
		QueryPerformanceCounter(&pc);
		QueryPerformanceFrequency(&pf);
		state->_pcBase = pc.QuadPart;
		state->useconds = 0;
		state->usecondDelta = 1;
		state->secondDelta = 0.000001;
		state->seconds = 0.0;
		for (u32 y = 0; y < IconSize; ++y)
			for (u32 x = 0; x < IconSize; ++x) {

				u32 addr = (y * IconSize + x);
				state->iconABits[addr] = 0;
				state->iconXBits[addr * 4 + 2] = 0;
				if( y == IconSize - 1 ){
					state->iconXBits[addr * 4 + 1] = x&1?255:0;
					state->iconXBits[addr * 4 + 0] = state->iconXBits[addr * 4 + 1] / 2;
				}
				else {
					state->iconXBits[addr * 4 + 1] = 0;
					state->iconXBits[addr * 4 + 0] = 0;
				}
				state->iconXBits[addr * 4 + 3] = 255;

			}
	}
	genter(state);
}
void mexit(void) {
	state->_tobreak = 1;
}
void tick(void) {
	// Input
	for (u32 i = 0; i < 256; ++i)
		state->_keys[i] = GetAsyncKeyState(i);
	for (u32 i = 0; i < 256; ++i) {
		int pressed = 0;
		int released = 0;
		int me = 0;
		if (state->_keys[i] < 0) {
			me = 1;
			if (state->_keysOld[i] >= 0)
				pressed = 1;
		}
		else if (state->_keysOld[i] < 0)
			released = 1;
		state->keys[i] = me;
		state->keysPressed[i] = pressed;
		state->keysReleased[i] = released;
	}
	PSHORT t = state->_keys;
	state->_keys = state->_keysOld;
	state->_keysOld = t;

	gtick(state);
	
}
void display(void) {
	{
		// Update time.
		LARGE_INTEGER pc, pf;

		QueryPerformanceCounter(&pc);
		QueryPerformanceFrequency(&pf);
		u64 newval = ((pc.QuadPart - state->_pcBase) * 1000000) / pf.QuadPart;
		if (newval != state->useconds) {
			state->usecondDelta = newval - state->useconds;
			state->useconds = newval;
			state->seconds = state->useconds / 1000000.0;
			state->secondDelta = state->usecondDelta / 1000000.0;
		}
	}

	static u32 framesdone = 0;
	static u64 permillisecond = 0;
	while( permillisecond < (state->useconds / 1000)){
		if (permillisecond % 10 == 0) {
			static u64 rval = 0;
			for (int i = 0; i < 100; ++i) {
				int x2 = mrand() % IconSize;
				int y = mrand() % (IconSize - 1);
				int x = (x2 - 1) + (mrand() % 3);
				int y2 = y + 1;
				int addr = (x + y * IconSize) * 4;
				int addr2 = (x2 + y2 * IconSize) * 4;
				int c = state->iconXBits[addr2 + 1] - mrand() % 19;
				if (c < 0)
					c = 0;

				state->iconXBits[addr + 1] = c;
				state->iconXBits[addr + 0] = c / 2;
			}

			HICON todel = state->wc.hIcon;
			state->wc.hIcon = CreateIcon(NULL, IconSize, IconSize, 4, 8, state->iconABits, state->iconXBits);
			SendMessage(state->hWnd, WM_SETICON, ICON_SMALL, (LPARAM)state->wc.hIcon);
			DestroyIcon(todel);
		}
		
		if (permillisecond % 250 == 0) {

			WCHAR t = state->title[0];
			for (u32 i = 1; i < state->titleSize; ++i)
				state->title[i - 1] = state->title[i];
			state->title[state->titleSize - 1] = t;

			WCHAR* bigt = (WCHAR*)malloc(2050);
			static u32 pframesdone = 0;
			wsprintfW(bigt, L"%s - %d fps", state->title, pframesdone);
			SetWindowTextW(state->hWnd, bigt);
			free(bigt);
			if (permillisecond % 1000 == 0) {
				pframesdone = framesdone;
				framesdone = 0;
			}


		}
		
		++permillisecond;
	}
	++framesdone;

	// Draw
	gdisplay(state);

	tick();
	glFlush();
	SwapBuffers(state->hDC);
}


LONG WINAPI eventLoop(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	static PAINTSTRUCT ps;

	switch (uMsg) {
	case WM_PAINT:
		display();
		BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		return 0;

	case WM_SIZE:
		if (!state->fullscreen) {
			state->width = LOWORD(lParam);
			state->height = HIWORD(lParam);
			RECT r;
			r.left = state->x;
			r.top = state->y;
			r.right = r.left + state->width;
			r.bottom = r.top + state->height;
			AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, FALSE);
			state->width = r.right - r.left;
			state->height = r.bottom - r.top;

		}

		glViewport(0, 0, LOWORD(lParam), HIWORD(lParam));
		PostMessage(hWnd, WM_PAINT, 0, 0);

		return 0;

	case WM_MOVE:
		if (!state->fullscreen) {
			state->x = LOWORD(lParam);
			state->y = HIWORD(lParam);
			RECT r;
			r.left = state->x;
			r.top = state->y;
			r.right = r.left + state->width;
			r.bottom = r.top + state->height;
			AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, FALSE);
			state->x = r.left;
			state->y = r.top;
		}
		PostMessage(hWnd, WM_PAINT, 0, 0);
		return 0;


	case WM_CLOSE:
		mexit();
		return 0;
	}

	return (LONG)DefWindowProc(hWnd, uMsg, wParam, lParam);
}

__declspec(dllexport) HWND setup(HICON hi, HINSTANCE hinst, char* title, int x, int y, int width, int height, BYTE type, DWORD flags) {
	enter();
	state->hInstance = hinst;
	state->fullscreen = 0;
	state->wc.style = CS_OWNDC;
	state->wc.lpfnWndProc = (WNDPROC)eventLoop;
	state->wc.cbClsExtra = 0;
	state->wc.cbWndExtra = 0;
	state->wc.hInstance = hinst;
	state->wc.hIcon = hi;
	state->wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	state->wc.hbrBackground = NULL;
	state->wc.lpszMenuName = NULL;
	state->wc.lpszClassName = L"OpenGL";

	RegisterClassW(&state->wc);

	state->titleSize = MultiByteToWideChar(CP_UTF8, 0, title, -1, state->title, 1020) - 1;
	state->hWnd = CreateWindowExW(WS_EX_WINDOWEDGE, L"OpenGL", state->title, WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, x, y, width, height, NULL, NULL, state->hInstance, NULL);
	state->width = width;
	state->height = height;

	state->hDC = GetDC(state->hWnd);

	/* there is no guarantee that the contents of the stack that become
	   the pfd are zeroed, therefore _make sure_ to clear these bits. */
	memset(&state->pfd, 0, sizeof(state->pfd));
	state->pfd.nSize = sizeof(state->pfd);
	state->pfd.nVersion = 1;
	state->pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | flags;
	state->pfd.iPixelType = type;
	state->pfd.cColorBits = 32;

	state->pf = ChoosePixelFormat(state->hDC, &state->pfd);

	SetPixelFormat(state->hDC, state->pf, &state->pfd);

	DescribePixelFormat(state->hDC, state->pf, sizeof(PIXELFORMATDESCRIPTOR), &state->pfd);

	ReleaseDC(state->hWnd, state->hDC);

		

	state->hDC = GetDC(state->hWnd);
	state->hRC = wglCreateContext(state->hDC);
	wglMakeCurrent(state->hDC, state->hRC);

	BOOL(*pfunc)(int) = (BOOL (*)(int))wglGetProcAddress("wglSwapIntervalEXT");
	pfunc(1);

	ShowWindow(state->hWnd, SW_SHOW);

	while (1) {
		while (PeekMessage(&state->msg, state->hWnd, 0, 0, PM_NOREMOVE)) {
			if (GetMessage(&state->msg, state->hWnd, 0, 0)) {
				TranslateMessage(&state->msg);
				DispatchMessageW(&state->msg);
			}
		}
		display();
		if (state->_tobreak)
			break;
	}
	gexit(state);
	wglMakeCurrent(NULL, NULL);
	ReleaseDC(state->hWnd, state->hDC);
	wglDeleteContext(state->hRC);
	DestroyWindow(state->hWnd);
	//free(state->iconXBits);
	//free(state->iconABits);
	DestroyIcon(state->wc.hIcon);
	free(state);
	return NULL;
}
BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved ){
    switch (ul_reason_for_call){
    case DLL_PROCESS_ATTACH:
		break;
    case DLL_THREAD_ATTACH:
		break;
    case DLL_THREAD_DETACH:
		break;
    case DLL_PROCESS_DETACH:
		break;
    }
    return TRUE;
}

