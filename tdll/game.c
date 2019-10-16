#include "tdll.h"


typedef struct {
	GLuint screenTex;
	GLuint pipeline;
} sgame;
sgame game;

void genter(gstate* state) {
	// dimensions of the image
	int tex_w = 256, tex_h = 256;

	checkGlErrors("sab");
	game.screenTex = makeTax(tex_w, tex_h, GL_LINEAR, GL_RGBA, GL_FLOAT);
	checkGlErrors("ww22222b");
	glBindImageTexture(2, game.screenTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

	checkGlErrors("ascdb");


	GLuint prog = compileComputeShader(getResource(ID_COMPUTE_SHADER));
	glUseProgram(prog);
	checkGlErrors("b");
	glUniform1i(glGetUniformLocation(prog, "destTex"), 2);
	checkGlErrors("after");
	glDispatchCompute(1, 256, 1);
	checkGlErrors("after2");
	float ret[4];	glGetTextureSubImage(game.screenTex, 0, 10, 0, 0, 1, 1, 1, GL_RGBA, GL_FLOAT, 16, ret);
	checkGlErrors("aftersd2");
	printf("%d %d %d %d is a bignummmmmmm", (int)(ret[0] * 1000), (int)(ret[1] * 1000), (int)(ret[2] * 1000), (int)(ret[3] * 1000));

	game.pipeline = compilePipeline(getResource(ID_PIPELINE));
}
void gexit(gstate* state) {


}
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
	glUseProgram(game.pipeline);
	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_TRIANGLES);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f((f32)sin(state->seconds * 10), (f32)cos(state->seconds * 10), 0);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-1, -1, 0);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(1, -1, 0);
	glEnd();
}

