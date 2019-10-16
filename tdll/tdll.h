#pragma once
#include <windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <math.h>

#include "..\\tiny.h"
#include "resource.h"
#include "glext.h"
#include "wglext.h"

__declspec(dllexport) int _fltused;
__declspec(dllexport) HWND setup(HMODULE, HICON, HINSTANCE hinst, char* title, int x, int y, int width, int height, BYTE type, DWORD flags);

#ifdef _DEBUG
#define say(X) MessageBoxA(NULL,X,"test",0)
#else
#define say(X) 
#endif

#ifdef _DEBUG
#define printf(...) { char buf[ 1050 ]; wsprintfA( buf, __VA_ARGS__ ); MessageBoxA(NULL,buf,"test",0); }
#else
#define printf(...) 
#endif



void mexit(void);
const u8* getResource( u32 id );


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

	// GL info
	const u8* rendererString;
	int workGroupCount[3];
	int workGroupSize[3];
	int workGroupInvokations;

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
	jmp_buf* _exit;
	u32  _numExitCallbacks;
	void (**_exitCallbacks)(void*);
	void** _exitArgs;
	int _tobreak;
} gstate;

void matexit(void(*)(void*), void*);

void gtick(gstate*);
void gdisplay(gstate*);
void genter(gstate*);
void gexit(gstate*);



// OpenGL
PFNGLACTIVETEXTUREPROC glActiveTexture;
PFNGLBINDIMAGETEXTUREPROC glBindImageTexture;
PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;
PFNGLGETINTEGERI_VPROC glGetIntegeri_v;
PFNGLCREATESHADERPROC glCreateShader;
PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLSHADERSOURCEPROC glShaderSource;
PFNGLCOMPILESHADERPROC glCompileShader;
PFNGLGETSHADERIVPROC glGetShaderiv;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLATTACHSHADERPROC glAttachShader;
PFNGLLINKPROGRAMPROC glLinkProgram;
PFNGLGETPROGRAMIVPROC glGetProgramiv;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
PFNGLUSEPROGRAMPROC glUseProgram;
PFNGLDISPATCHCOMPUTEPROC glDispatchCompute;
PFNGLGETTEXTURESUBIMAGEPROC glGetTextureSubImage;
PFNGLGETTEXTUREIMAGEPROC glGetTextureImage;
PFNGLUNIFORM1FPROC glUniform1f;
PFNGLUNIFORM1IPROC glUniform1i;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
PFNGLDELETESHADERPROC glDeleteShader;
PFNGLDELETEPROGRAMPROC glDeleteProgram;


GLuint compileComputeShader(const u8*);
GLuint compilePipeline(const u8*);
GLuint makeTax(u32 w, u32 h, GLint mip, GLenum format, GLenum type);

#ifdef _DEBUG
#define loadGl( Y, X ) { void* ret; ret = X = (Y)wglGetProcAddress(#X); if (ret == NULL){ say("FAILED " #X); mexit(); } }
#else
#define loadGl( Y, X ) X = (Y)wglGetProcAddress(#X);
#endif

#ifdef _DEBUG
#define checkGlErrors( msg ) { GLenum e = glGetError(); if( e!=GL_NO_ERROR){ while (e != GL_NO_ERROR) { printf( "OpenGL error in \"%s\": %s (%d)\n", msg, gluErrorString(e), e); e = glGetError(); } mexit(); } }
#else
#define checkGlErrors( X ) 
#endif

#ifdef _DEBUG

#endif