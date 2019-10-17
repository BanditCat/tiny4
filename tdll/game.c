#include "tdll.h"


typedef struct {
	GLuint screenTex;
	GLuint pipeline;
	GLuint quadBuf;
	GLuint compute;
	u32 stWidth, stHeight;
} sgame;
sgame game;

void genter(gstate* state) {
	// dimensions of the image

	checkGlErrors("sab");
	game.stWidth = state->screenWidth + 32;
	game.stHeight = state->screenHeight + 32;
	game.screenTex = makeTax(game.stWidth, game.stHeight, GL_NEAREST, GL_RGBA, GL_FLOAT);
	checkGlErrors("ww22222b");
	glBindImageTexture(2, game.screenTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

	checkGlErrors("ascdb");

	u32 sz;
	const u8* src = getResource(ID_COMPUTE_SHADER, &sz);
	game.compute = compileComputeShader(src,sz);
	glUseProgram(game.compute);
	glUniform1i(0, 2);
	GLuint font[2] = { 1414812680, 60 };
	glUniform1iv(2,2,font);

	src = getResource(ID_PIPELINE, &sz);
	game.pipeline = compilePipeline(src,sz);


	float arr[] = { -1.0,-1.0,0.0,1.0,
					1.0,-1.0,0.0,1.0,
                    -1.0,1.0,0.0,1.0,
					1.0, 1.0,0.0,1.0};
	glGenBuffers(1, &game.quadBuf);
	matexit(delBuffer, (void*)game.quadBuf);
	glBindBuffer(GL_ARRAY_BUFFER, game.quadBuf);
	glBufferData(GL_ARRAY_BUFFER, 64, arr, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, NULL);
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
			state->clientWidth = GetSystemMetrics(SM_CXSCREEN);
			state->clientHeight = GetSystemMetrics(SM_CYSCREEN);
			SetWindowPos(state->hWnd, HWND_TOPMOST, 0, 0, state->clientWidth, state->clientHeight, 0);
			ShowWindow(state->hWnd, SW_SHOW);
		}
	}
}
void gdisplay(gstate* state) {
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(game.compute);
	u32 tilesX = state->clientWidth / 32 + 1;
	u32 tilesY = state->clientHeight / 32 + 1;
	glUniform4f(1, state->clientWidth, state->clientHeight, (float)state->clientWidth / game.stWidth, (float)state->clientHeight / game.stHeight);
	glDispatchCompute(tilesX , tilesY, 1);
	glUseProgram(game.pipeline);
	glUniform4f(4, state->clientWidth, state->clientHeight, (float)state->clientWidth / game.stWidth, (float)state->clientHeight / game.stHeight);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
 }


