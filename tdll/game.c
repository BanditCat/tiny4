#include "tdll.h"


typedef struct {
  GLuint screenTex;
  GLuint pipeline;
  GLuint quadBuf;
  GLuint compute;
  u32 stWidth, stHeight;
} sgame;
sgame game;

void genter( gstate* state ){
  // dimensions of the image

  checkGlErrors( "sab" );
  game.stWidth = state->screenWidth + 32;
  game.stHeight = state->screenHeight + 32;
  game.screenTex = makeTax( game.stWidth, game.stHeight, GL_NEAREST, GL_RGBA, GL_FLOAT, NULL );
  checkGlErrors( "ww22222b" );

  checkGlErrors( "ascdb" );

  u32 sz;
  const u8* src = getResource( ID_COMPUTE_SHADER, &sz );
  game.compute = compileComputeShader( src, sz );
  glUseProgram( game.compute );
  glBindImageTexture( 2, game.screenTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F );
  glUniform1i( 0, 2 );
  GLuint font[95 * 2] = { 0,0,7995392,0,805318656,0,1041514004,20,712976912,4,856188513,67,826689847,6,3145728,0,4268572,0,574685184,28,473832490,42,138283016,8,262656,0,134744064,0,512,0,805832193,64,1095319870,62,25116961,1,1229275937,49,1229537570,54,2131232888,8,1364281722,78,1229539646,6,1347174977,96,1229539638,54,1229539632,62,1310720,0,1311232,0,571741192,34,336860180,20,336863778,8,1347043376,32,1717191228,60,1212696639,63,1229539711,54,1094795582,65,1094795647,62,1229539711,65,1212696703,64,1229539634,126,134744191,127,1098858817,65,2118205762,64,571738239,65,16843135,1,1077887039,63,20856959,127,1094795582,62,1212696703,48,1178681662,57,1246513279,49,1229539634,38,1082081344,64,16843134,126,234950256,112,25035134,126,336073827,99,168364144,124,1363756355,97,1094811392,0,101199936,1,2134982912,0,541073424,16,33686018,2,8256,0,707406340,30,303174270,12,572662300,34,303174156,126,707406364,24,673720350,32,690563600,30,235407422,0,3014656,0,1008861700,0,571738174,0,33686078,0,538845214,30,538972222,30,572662300,28,405021759,0,1059333144,1,538972222,16,707406352,4,540942368,32,33686076,60,67241016,56,37487164,60,336073762,34,168364336,60,841623074,34,1094811400,0,8323072,0,2134982912,8,67637256,8 };

  const u8* msg = "The quick brown fox Jumped over the lazy black dog. The Quick Brown Fox Jumped Over The Lazy Black Dog;	u32 msz = strlen(msg);	u32 * msgi = malloc(sizeof(u32) * msz);	for (u32 i = 0; i < msz; ++i)	msgi[i] = msg[i];	glUniform1i(2, msz);glUniform1iv(3, msz / 4 + 1, msg);";
  u32 msz = strlen( msg );
  u32* msgi = malloc( sizeof( u32 ) * msz );
  for(u32 i = 0; i < msz; ++i)
    msgi[i] = msg[i];
  glUniform1i( 2, msz );
  glUniform1iv( 3, msz / 4 + 1, msg );

  glUniform1iv( 5000, 95 * 2, font );

  src = getResource( ID_PIPELINE, &sz );
  game.pipeline = compilePipeline( src, sz );


  float arr[] = { -1.0,-1.0,0.0,1.0,
                   1.0,-1.0,0.0,1.0,
                  -1.0,1.0,0.0,1.0,
                   1.0, 1.0,0.0,1.0 };
  glGenBuffers( 1, &game.quadBuf );
  matexit( delBuffer, (void*)game.quadBuf );
  glBindBuffer( GL_ARRAY_BUFFER, game.quadBuf );
  glBufferData( GL_ARRAY_BUFFER, 64, arr, GL_STATIC_DRAW );
  glEnableVertexAttribArray( 1 );
  glVertexAttribPointer( 1, 4, GL_FLOAT, GL_FALSE, 0, NULL );
}
void gexit( gstate* state ){


}
void gtick( gstate* state ){
  if(state->keys[VK_ESCAPE]){
    mexit();
  }
  if(state->keysPressed[VK_RETURN]){
    if(state->fullscreen){
      state->fullscreen = 0;
      SetWindowLongPtrW( state->hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN );
      SetWindowPos( state->hWnd, HWND_TOP, state->x, state->y, state->width, state->height, 0 );
      ShowWindow( state->hWnd, SW_SHOW );
    }
    else {
      state->fullscreen = 1;
      SetWindowLongPtrW( state->hWnd, GWL_STYLE, WS_CLIPSIBLINGS | WS_CLIPCHILDREN );
      state->clientWidth = GetSystemMetrics( SM_CXSCREEN );
      state->clientHeight = GetSystemMetrics( SM_CYSCREEN );
      SetWindowPos( state->hWnd, HWND_TOPMOST, 0, 0, state->clientWidth, state->clientHeight, 0 );
      ShowWindow( state->hWnd, SW_SHOW );
    }
  }
}
void gdisplay( gstate* state ){
  glClear( GL_COLOR_BUFFER_BIT );
  glUseProgram( game.compute );
  u32 tilesX = state->clientWidth / 32 + 1;
  u32 tilesY = state->clientHeight / 32 + 1;
  glUniform4f( 1, state->clientWidth, state->clientHeight, (float)state->clientWidth / game.stWidth, (float)state->clientHeight / game.stHeight );
  glDispatchCompute( tilesX, tilesY, 1 );
  glUseProgram( game.pipeline );
  glUniform4f( 4, state->clientWidth, state->clientHeight, (float)state->clientWidth / game.stWidth, (float)state->clientHeight / game.stHeight );
  glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
}


