#include "tdll.h"


void gtick(gstate*state) {
	if (state->keys[VK_ESCAPE]) {
		mexit();
	}
	if (state->keysPressed[VK_RETURN]) {
		if (state->fullscreen) {
			state->fullscreen = 0;
			SetWindowLongPtrW(state->hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
			SetWindowPos(state->hWnd, HWND_TOP, state->x, state->y, state->width, state->height, 0);
			ShowWindow(state->hWnd, SW_SHOW);
		}
		else {
			state->fullscreen = 1;
			SetWindowLongPtrW(state->hWnd, GWL_STYLE, WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
			SetWindowPos(state->hWnd, HWND_TOPMOST, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), 0);
			ShowWindow(state->hWnd, SW_SHOW);
		}
	}
}
void gdisplay(gstate* state) {
	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_TRIANGLES);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f((f32)sin(state->seconds), (f32)cos(state->seconds), 0);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-1, -1, 0);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(1, -1, 0);
	glEnd();
}
void genter(gstate* state) {

}
void gexit(gstate* state){

}