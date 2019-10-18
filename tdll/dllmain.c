// dllmain.cpp : Defines the entry point for the DLL application.
#include "tdll.h"

// MALLOC MUST GO FIRST!
void* pmalloc ( size_t sz ){
  void* ret = (void*)GlobalAlloc ( GPTR, sz );
  if(ret == NULL)
    mexit ();
  return ret;
}
#define malloc pmalloc
void pfree ( void* p ){
  GlobalFree ( p );
}
#define free pfree

#define IconSize 32



gstate* state = NULL;


void mexit ( void );
void matexit ( void(*func)(void*), void* arg ){
  state->_exitCallbacks[state->_numExitCallbacks] = func;
  state->_exitArgs[state->_numExitCallbacks] = arg;
  state->_numExitCallbacks++;
  // Test power of 2
  u32 tst = state->_numExitCallbacks;
  while(!(tst & 1))
    tst >>= 1;
  // Remalloc if power of 2
  if(tst == 1){
    void(**nfuncs)(void*) = malloc ( sizeof ( void(**)(void*) ) * state->_numExitCallbacks * 2 );
    void** nargs = malloc ( sizeof ( void* ) * state->_numExitCallbacks * 2 );
    memcpy ( nfuncs, state->_exitCallbacks, sizeof ( void(**)(void*) ) * state->_numExitCallbacks );
    memcpy ( nargs, state->_exitArgs, sizeof ( void* ) * state->_numExitCallbacks );
    free ( state->_exitCallbacks ); state->_exitCallbacks = nfuncs;
    free ( state->_exitArgs ); state->_exitArgs = nargs;
  }
}

void delProgram ( void* arg ){
  GLuint todel = (GLuint)arg;
  glDeleteProgram ( todel );
}
void delTexture ( void* arg ){
  GLuint todel = (GLuint)arg;
  glDeleteTextures ( 1, &todel );
}
void delBuffer ( void* arg ){
  GLuint todel = (GLuint)arg;
  glDeleteBuffers ( 1, &todel );
}


u64 nmrand = 0;

void smrand ( u64 s ){
  nmrand = s;
}
u32 mrand ( void ){
  nmrand = nmrand * 6364136223846793005 + 1442695040888963407;
  return nmrand >> 32;
}

void enter ( void ){
  if(state == NULL){
    state = (gstate*)malloc ( sizeof ( gstate ) );
    state->started = 0;
    state->framesDone = 0;
    for(u32 i = 0; i < 256; ++i){
      state->_ks1[i] = state->_ks2[i] = 0;
      state->keys[i] = state->keysReleased[i] = state->keysPressed[i] = 0;
    }
    state->_keys = state->_ks1;
    state->_keysOld = state->_ks2;

    state->iconABits = (u8*)malloc ( IconSize * IconSize );
    state->iconXBits = (u8*)malloc ( IconSize * IconSize * 4 );
    LARGE_INTEGER pc, pf;
    QueryPerformanceCounter ( &pc );
    QueryPerformanceFrequency ( &pf );
    state->_pcBase = pc.QuadPart;
    state->useconds = 0;
    state->usecondDelta = 1;
    state->secondDelta = 0.000001;
    state->seconds = 0.0;
    for(u32 y = 0; y < IconSize; ++y)
      for(u32 x = 0; x < IconSize; ++x){

        u32 addr = (y * IconSize + x);
        state->iconABits[addr] = 0;
        state->iconXBits[addr * 4 + 2] = 0;
        if(y == IconSize - 1){
          state->iconXBits[addr * 4 + 1] = x & 1 ? 255 : 0;
          state->iconXBits[addr * 4 + 0] = state->iconXBits[addr * 4 + 1] / 2;
        }
        else {
          state->iconXBits[addr * 4 + 1] = 0;
          state->iconXBits[addr * 4 + 0] = 0;
        }
        state->iconXBits[addr * 4 + 3] = 255;

      }
  }
}
void mexit ( void ){
  longjmp ( *(state->_exit), 1 );
}
void tick ( void ){
  // Input
  for(u32 i = 0; i < 256; ++i)
    state->_keys[i] = GetAsyncKeyState ( i );
  for(u32 i = 0; i < 256; ++i){
    int pressed = 0;
    int released = 0;
    int me = 0;
    if(state->_keys[i] < 0){
      me = 1;
      if(state->_keysOld[i] >= 0)
        pressed = 1;
    }
    else if(state->_keysOld[i] < 0)
      released = 1;
    state->keys[i] = me;
    state->keysPressed[i] = pressed;
    state->keysReleased[i] = released;
  }
  PSHORT t = state->_keys;
  state->_keys = state->_keysOld;
  state->_keysOld = t;

  gtick ( state );

}
void display ( void ){
  if(state->started){
    {
      // Update time.
      LARGE_INTEGER pc, pf;

      QueryPerformanceCounter ( &pc );
      QueryPerformanceFrequency ( &pf );
      u64 newval = ((pc.QuadPart - state->_pcBase) * 1000000) / pf.QuadPart;
      if(newval != state->useconds){
        state->usecondDelta = newval - state->useconds;
        state->useconds = newval;
        state->seconds = state->useconds / 1000000.0;
        state->secondDelta = state->usecondDelta / 1000000.0;
      }
    }

    static u32 framesdone = 0;
    static u64 permillisecond = 0;
    while(permillisecond < (state->useconds / 1000)){
      if(permillisecond % 10 == 0){
        static u64 rval = 0;
        for(int i = 0; i < 100; ++i){
          int x2 = mrand () % IconSize;
          int y = mrand () % (IconSize - 1);
          int x = (x2 - 1) + (mrand () % 3);
          int y2 = y + 1;
          int addr = (x + y * IconSize) * 4;
          int addr2 = (x2 + y2 * IconSize) * 4;
          int c = state->iconXBits[addr2 + 1] - mrand () % 19;
          if(c < 0)
            c = 0;

          state->iconXBits[addr + 1] = c;
          state->iconXBits[addr + 0] = c / 2;
        }

        HICON todel = state->wc.hIcon;
        state->wc.hIcon = CreateIcon ( NULL, IconSize, IconSize, 4, 8, state->iconABits, state->iconXBits );
        SendMessage ( state->hWnd, WM_SETICON, ICON_SMALL, (LPARAM)state->wc.hIcon );
        DestroyIcon ( todel );
      }

      if(permillisecond % 250 == 0){

        WCHAR t = state->title[0];
        for(u32 i = 1; i < state->titleSize; ++i)
          state->title[i - 1] = state->title[i];
        state->title[state->titleSize - 1] = t;

        WCHAR* bigt = (WCHAR*)malloc ( 2050 );
        static u32 pframesdone = 0;
        wsprintfW ( bigt, L"%s - %d fps", state->title, pframesdone );
        SetWindowTextW ( state->hWnd, bigt );
        free ( bigt );
        if(permillisecond % 1000 == 0){
          pframesdone = framesdone;
          framesdone = 0;
        }


      }

      ++permillisecond;
    }
    ++framesdone;

    // Draw
    gdisplay ( state );
    if(state->framesDone++ > 2
      )
      tick ();
    glFlush ();
    SwapBuffers ( state->hDC );
  }
}

LONG WINAPI eventLoop ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ){
  static PAINTSTRUCT ps;

  switch(uMsg){
  case WM_PAINT:
    display ();
    BeginPaint ( hWnd, &ps );
    EndPaint ( hWnd, &ps );
    return 0;

  case WM_SIZE:
    if(!state->fullscreen){
      state->clientWidth = state->width = LOWORD ( lParam );
      state->clientHeight = state->height = HIWORD ( lParam );
      RECT r;
      r.left = state->x;
      r.top = state->y;
      r.right = r.left + state->width;
      r.bottom = r.top + state->height;
      AdjustWindowRect ( &r, WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, FALSE );
      state->width = r.right - r.left;
      state->height = r.bottom - r.top;

    }

    glViewport ( 0, 0, LOWORD ( lParam ), HIWORD ( lParam ) );
    PostMessage ( hWnd, WM_PAINT, 0, 0 );

    return 0;

  case WM_MOVE:
    if(!state->fullscreen){
      state->x = LOWORD ( lParam );
      state->y = HIWORD ( lParam );
      RECT r;
      r.left = state->x;
      r.top = state->y;
      r.right = r.left + state->width;
      r.bottom = r.top + state->height;
      AdjustWindowRect ( &r, WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, FALSE );
      state->x = r.left;
      state->y = r.top;
    }
    PostMessage ( hWnd, WM_PAINT, 0, 0 );
    return 0;


  case WM_CLOSE:
    state->_tobreak = 1;
    return 0;
  }

  return (LONG)DefWindowProc ( hWnd, uMsg, wParam, lParam );
}

__declspec(dllexport) HWND setup ( HMODULE ll, HICON hi, HINSTANCE hinst, char* title, int x, int y, int width, int height, BYTE type, DWORD flags ){
  enter ();
  state->hInstance = hinst;
  state->ll = ll;
  state->fullscreen = 0;
  state->screenWidth = GetSystemMetrics ( SM_CXSCREEN );
  state->screenHeight = GetSystemMetrics ( SM_CYSCREEN );
  state->_numExitCallbacks = 0;
  state->_exitCallbacks = malloc ( sizeof ( void (*)(void*) ) );
  state->_exitArgs = malloc ( sizeof ( void* ) );
  state->_tobreak = 0;

  state->wc.style = CS_OWNDC;
  state->wc.lpfnWndProc = (WNDPROC)eventLoop;
  state->wc.cbClsExtra = 0;
  state->wc.cbWndExtra = 0;
  state->wc.hInstance = hinst;
  state->wc.hIcon = hi;
  state->wc.hCursor = LoadCursor ( NULL, IDC_ARROW );
  state->wc.hbrBackground = NULL;
  state->wc.lpszMenuName = NULL;
  state->wc.lpszClassName = L"OpenGL";

  RegisterClassW ( &state->wc );

  state->titleSize = MultiByteToWideChar ( CP_UTF8, 0, title, -1, state->title, 1020 ) - 1;
  state->hWnd = CreateWindowExW ( WS_EX_WINDOWEDGE, L"OpenGL", state->title, WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, x, y, width, height, NULL, NULL, state->hInstance, NULL );
  state->width = width;
  state->height = height;

  state->hDC = GetDC ( state->hWnd );

  /* there is no guarantee that the contents of the stack that become
     the pfd are zeroed, therefore _make sure_ to clear these bits. */
  memset ( &state->pfd, 0, sizeof ( state->pfd ) );
  state->pfd.nSize = sizeof ( state->pfd );
  state->pfd.nVersion = 1;
  state->pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | flags;
  state->pfd.iPixelType = type;
  state->pfd.cColorBits = 32;

  state->pf = ChoosePixelFormat ( state->hDC, &state->pfd );

  SetPixelFormat ( state->hDC, state->pf, &state->pfd );

  DescribePixelFormat ( state->hDC, state->pf, sizeof ( PIXELFORMATDESCRIPTOR ), &state->pfd );

  ReleaseDC ( state->hWnd, state->hDC );



  state->hDC = GetDC ( state->hWnd );
  state->hRC = wglCreateContext ( state->hDC );
  wglMakeCurrent ( state->hDC, state->hRC );


  ShowWindow ( state->hWnd, SW_SHOW );

  // Load gl before calling genter
  loadGl ( PFNGLACTIVETEXTUREPROC, glActiveTexture );
  loadGl ( PFNGLBINDIMAGETEXTUREPROC, glBindImageTexture );
  loadGl ( PFNWGLSWAPINTERVALEXTPROC, wglSwapIntervalEXT );
  loadGl ( PFNGLGETINTEGERI_VPROC, glGetIntegeri_v );
  loadGl ( PFNGLCREATESHADERPROC, glCreateShader );
  loadGl ( PFNGLCREATEPROGRAMPROC, glCreateProgram );
  loadGl ( PFNGLSHADERSOURCEPROC, glShaderSource );
  loadGl ( PFNGLCOMPILESHADERPROC, glCompileShader );
  loadGl ( PFNGLGETSHADERIVPROC, glGetShaderiv );
  loadGl ( PFNGLGETSHADERINFOLOGPROC, glGetShaderInfoLog );
  loadGl ( PFNGLATTACHSHADERPROC, glAttachShader );
  loadGl ( PFNGLLINKPROGRAMPROC, glLinkProgram );
  loadGl ( PFNGLGETPROGRAMIVPROC, glGetProgramiv );
  loadGl ( PFNGLGETPROGRAMINFOLOGPROC, glGetProgramInfoLog );
  loadGl ( PFNGLUSEPROGRAMPROC, glUseProgram );
  loadGl ( PFNGLDISPATCHCOMPUTEPROC, glDispatchCompute );
  loadGl ( PFNGLGETTEXTURESUBIMAGEPROC, glGetTextureSubImage );
  loadGl ( PFNGLUNIFORM1FPROC, glUniform1f );
  loadGl ( PFNGLUNIFORM4FPROC, glUniform4f );
  loadGl ( PFNGLUNIFORM1FVPROC, glUniform1fv );
  loadGl ( PFNGLUNIFORM4FVPROC, glUniform4fv );
  loadGl ( PFNGLUNIFORM1IPROC, glUniform1i );
  loadGl ( PFNGLUNIFORM4IPROC, glUniform4i );
  loadGl ( PFNGLUNIFORM1IVPROC, glUniform1iv );
  loadGl ( PFNGLUNIFORM4IVPROC, glUniform4iv );
  loadGl ( PFNGLGETUNIFORMLOCATIONPROC, glGetUniformLocation );
  loadGl ( PFNGLDELETESHADERPROC, glDeleteShader );
  loadGl ( PFNGLDELETEPROGRAMPROC, glDeleteProgram );
  loadGl ( PFNGLVERTEXATTRIBPOINTERARBPROC, glVertexAttribPointer );
  loadGl ( PFNGLENABLEVERTEXATTRIBARRAYARBPROC, glEnableVertexAttribArray );
  loadGl ( PFNGLGENBUFFERSPROC, glGenBuffers );
  loadGl ( PFNGLBINDBUFFERPROC, glBindBuffer );
  loadGl ( PFNGLBUFFERDATAPROC, glBufferData );
  loadGl ( PFNGLDELETEBUFFERSPROC, glDeleteBuffers );



  // GL info
  glGetIntegeri_v ( GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &state->workGroupCount[0] );
  glGetIntegeri_v ( GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &state->workGroupCount[1] );
  glGetIntegeri_v ( GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &state->workGroupCount[2] );
  glGetIntegeri_v ( GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &state->workGroupSize[0] );
  glGetIntegeri_v ( GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &state->workGroupSize[1] );
  glGetIntegeri_v ( GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &state->workGroupSize[2] );
  glGetIntegerv ( GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &state->workGroupInvokations );

  // VSYNC!!!!!
  wglSwapIntervalEXT ( 1 );

  // Now actually enter.
  jmp_buf jenv;
  static u32 exits = 0;
  int val = setjmp ( jenv );
  exits++;
  if(!val){
    state->_exit = &jenv;
    state->rendererString = glGetString ( GL_RENDERER );
    genter ( state );
    state->started = 1;
  }

  while(!val){
    while(PeekMessage ( &state->msg, state->hWnd, 0, 0, PM_NOREMOVE )){
      if(GetMessage ( &state->msg, state->hWnd, 0, 0 )){
        TranslateMessage ( &state->msg );
        DispatchMessageW ( &state->msg );
      }
      if(state->_tobreak)
        mexit ();
    }
    display ();
    checkGlErrors ( "Main loop" );
  }
  if(exits == 2){
    gexit ( state );
    for(u32 i = 0; i < state->_numExitCallbacks; ++i){
      state->_exitCallbacks[i] ( state->_exitArgs[i] );
    }
  }
  wglMakeCurrent ( NULL, NULL );
  ReleaseDC ( state->hWnd, state->hDC );
  wglDeleteContext ( state->hRC );
  DestroyWindow ( state->hWnd );
  // Why no free?
  //free(state->iconXBits);
  //free(state->iconABits);
  DestroyIcon ( state->wc.hIcon );
  free ( state->_exitArgs );
  free ( state->_exitCallbacks );
  free ( state );
  return NULL;
}


const u8* getResource ( u32 id, u32* sz ){
  HRSRC h = FindResourceExA ( state->ll, "MISC", MAKEINTRESOURCEA ( id ), MAKELANGID ( LANG_NEUTRAL, SUBLANG_NEUTRAL ) );
  HGLOBAL hg = LoadResource ( state->ll, h );
  if(sz != NULL)
    *sz = SizeofResource ( state->ll, h );
  return (const u8*)LockResource ( hg );
}

BOOL APIENTRY DllMain ( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved ){
  switch(ul_reason_for_call){
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


GLuint compileComputeShader ( const u8* src, u32 sz ){
  GLuint ret = glCreateProgram ();
  matexit ( delProgram, (void*)ret );
  GLuint shad = glCreateShader ( GL_COMPUTE_SHADER );
  glShaderSource ( shad, 1, &src, &sz );
  glCompileShader ( shad );
#ifdef _DEBUG
  int rvalue;
  glGetShaderiv ( shad, GL_COMPILE_STATUS, &rvalue );
  if(!rvalue){
    GLchar log[10240];
    GLsizei length;
    glGetShaderInfoLog ( shad, 10220, &length, log );
    printf ( "Error! Compiler log:\n%s\n", log );
    mexit ();
  }
#endif
  glAttachShader ( ret, shad );

  glLinkProgram ( ret );
#ifdef _DEBUG
  glGetProgramiv ( ret, GL_LINK_STATUS, &rvalue );
  if(!rvalue){
    GLchar log[10240];
    GLsizei length;
    glGetProgramInfoLog ( ret, 10239, &length, log );
    printf ( "Error! Linker log:\n%s\n", log );
    mexit ();
  }
#endif
  glDeleteShader ( shad );
  checkGlErrors ( "compileComputeShader" );
  return ret;
}

GLuint compilePipeline ( const u8* src, u32 sz ){
  const u8* vert = strstr ( src, "#VERTEX" ) + 7;
  const u8* geom = strstr ( vert, "#GEOMETRY" );
  u32 vertlen = (u32)(geom - vert);
  printf ( "%s", geom );
  geom += 9;
  const u8* frag = strstr ( geom, "#FRAGMENT" );
  u32 geomlen = (u32)(frag - geom);
  frag += 9;
  u32 fraglen = sz - (25 + geomlen + vertlen);
  GLuint ret = glCreateProgram ();
  matexit ( delProgram, (void*)ret );

  GLuint shads[3];
  GLenum types[3] = { GL_GEOMETRY_SHADER, GL_VERTEX_SHADER, GL_FRAGMENT_SHADER };
#ifdef _DEBUG
  const u8* typess[3] = { "GL_GEOMETRY_SHADER", "GL_VERTEX_SHADER", "GL_FRAGMENT_SHADER" };
  int rvalue;
#endif
  const u8* srcs[3] = { geom,vert,frag };
  u32 szs[3] = { geomlen,vertlen,fraglen };


  for(u32 i = 0; i < 3; ++i){
    shads[i] = glCreateShader ( types[i] );
    glShaderSource ( shads[i], 1, &(srcs[i]), &(szs[i]) );
    glCompileShader ( shads[i] );
#ifdef _DEBUG
    glGetShaderiv ( shads[i], GL_COMPILE_STATUS, &rvalue );
    if(!rvalue){
      GLchar log[10240];
      GLsizei length;
      glGetShaderInfoLog ( shads[i], 10220, &length, log );
      printf ( "Error! Compiler %s log:\n%s\n", typess[i], log );
      mexit ();
    }
#endif
    glAttachShader ( ret, shads[i] );
  }

  glLinkProgram ( ret );
#ifdef _DEBUG
  glGetProgramiv ( ret, GL_LINK_STATUS, &rvalue );
  if(!rvalue){
    GLchar log[10240];
    GLsizei length;
    glGetProgramInfoLog ( ret, 10239, &length, log );
    printf ( "Error! Linker log:\n%s\n", log );
    mexit ();
  }
#endif
  for(u32 i = 0; i < 3; ++i)
    glDeleteShader ( shads[i] );
  checkGlErrors ( "compilePipeline" );
  return ret;
}
GLuint makeTax ( u32 w, u32 h, GLint mip, GLenum format, GLenum type ){
  GLuint ret;
  glGenTextures ( 1, &ret );
  matexit ( delTexture, (void*)ret );
  glActiveTexture ( GL_TEXTURE0 );

  glBindTexture ( GL_TEXTURE_2D, ret );
  glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
  glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
  if(mip == GL_LINEAR || mip == GL_NEAREST){
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mip );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mip );
  }
  glTexImage2D ( GL_TEXTURE_2D, 0, GL_RGBA32F, w, h, 0, format, type, NULL );
  return ret;
}