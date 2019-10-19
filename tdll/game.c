#include "tdll.h"


typedef struct {
  GLuint screenTex;
  GLuint fontTex;
  GLuint stringTex;
  GLuint stringTexWidth, stringTexHeight;
  u32* theMatrix;
  GLuint matrixTex;
  GLuint pipeline;
  GLuint quadBuf;
  GLuint compute;
  u32 stWidth, stHeight;
} sgame;
sgame game;

void genter( gstate* state ){
  // dimensions of the image

  checkGlErrors( "sab" );
  game.stringTexWidth = ((state->screenWidth / 6) / 8) * 8 + 16; game.stringTexHeight = ((state->screenHeight / 8) / 8) * 8 + 16;
  game.theMatrix = malloc( sizeof( u32 ) * game.stringTexWidth );
  memset( game.theMatrix, 0, sizeof( u32 ) * game.stringTexWidth );

  game.stWidth = state->screenWidth + 32;
  game.stHeight = state->screenHeight + 32;
  game.screenTex = makeTex( game.stWidth, game.stHeight, GL_NEAREST, GL_RGBA8, NULL );
  GLuint font[95 * 2] = { 0,0,7995392,0,805318656,0,1041514004,20,712976912,4,856188513,67,826689847,6,3145728,0,4268572,0,574685184,28,473832490,42,138283016,8,262656,0,134744064,0,512,0,805832193,64,1095319870,62,25116961,1,1229275937,49,1229537570,54,2131232888,8,1364281722,78,1229539646,6,1347174977,96,1229539638,54,1229539632,62,1310720,0,1311232,0,571741192,34,336860180,20,336863778,8,1347043376,32,1717191228,60,1212696639,63,1229539711,54,1094795582,65,1094795647,62,1229539711,65,1212696703,64,1229539634,126,134744191,127,1098858817,65,2118205762,64,571738239,65,16843135,1,1077887039,63,20856959,127,1094795582,62,1212696703,48,1178681662,57,1246513279,49,1229539634,38,1082081344,64,16843134,126,234950256,112,25035134,126,336073827,99,168364144,124,1363756355,97,1094811392,0,101199936,1,2134982912,0,541073424,16,33686018,2,8256,0,707406340,30,303174270,12,572662300,34,303174156,126,707406364,24,673720350,32,690563600,30,235407422,0,3014656,0,1008861700,0,571738174,0,33686078,0,538845214,30,538972222,30,572662300,28,405021759,0,1059333144,1,538972222,16,707406352,4,540942368,32,33686076,60,67241016,56,37487164,60,336073762,34,168364336,60,841623074,34,1094811400,0,8323072,0,2134982912,8,67637256,8 };
  {
    u8* dat = malloc( 95 * 7 * 8);
    for(u32 let = 0; let < 95; ++let){
      for(u32 x = 0; x < 5; ++x){
        for(u32 y = 0; y < 7; ++y){
          u32 idx = let * 2 + (x == 4 ? 1 : 0);
          u32 shift = (x % 4) * 8;

          u32 didx = x + y * 8 + let * 56;
          if((font[idx] >> shift) & (1 << y))
            dat[didx] = 255;
          else
            dat[didx] = 0;
        }
      }
    }
    game.fontTex = makeTex( 5, 95 * 7, GL_NEAREST, GL_R8UI, dat );
    free( dat );
  }
  checkGlErrors( "ww22222b" );


  u32 sz;
  const u8* src = getResource( ID_COMPUTE_SHADER, &sz );
  game.compute = compileComputeShader( src, sz );
  glUseProgram( game.compute );


  const u8* msg = "The quick brown fox Jumped over the lazy black dog. The Quick Brown Fox Jumped Over The Lazy Black Dog;	u32 msz = strlen(msg);	u32 * msgi = malloc(sizeof(u32) * msz);	for (u32 i = 0; i < msz; ++i)	msgi[i] = msg[i];	glUniform1i(2, msz);glUniform1iv(3, msz / 4 + 1, msg);ENDENDENDEND";
  u32 msz = (u32)strlen( msg );
  glUniform1i( 2, msz );

  game.stringTex = makeTex( game.stringTexWidth, msz / game.stringTexWidth + 1, GL_NEAREST, GL_R8UI, msg );
  
  game.matrixTex = makeTex( game.stringTexWidth, 1, GL_NEAREST, GL_R32UI, game.theMatrix );


  glActiveTexture( GL_TEXTURE0 );
  glBindTexture( GL_TEXTURE_2D, game.screenTex );
  glBindImageTexture( 2, game.screenTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8 );
  glUniform1i( 0, 2 );
  glActiveTexture( GL_TEXTURE1 );
  glBindTexture( GL_TEXTURE_2D, game.fontTex );
  glBindImageTexture( 4, game.fontTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R8UI );
  glUniform1i( 3, 4 );
  glActiveTexture( GL_TEXTURE2 );
  glBindTexture( GL_TEXTURE_2D, game.stringTex );
  glBindImageTexture( 3, game.stringTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R8UI );
  glUniform1i( 4, 3 );
  glActiveTexture( GL_TEXTURE3 );
  glBindTexture( GL_TEXTURE_2D, game.matrixTex );
  glBindImageTexture( 1, game.matrixTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32UI );
  glUniform1i( 6, 1 );

  //glUniform1iv( 5000, 95 * 2, font );

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
  free( game.theMatrix );
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
  static u64 matd = 0;
  while( matd < state->seconds * 1000 ){
    for( u32 i = 0; i < game.stringTexWidth / 32; ++i ){
      u32 d = mrand() % game.stringTexWidth;
      game.theMatrix[ d ]++;
      if( mrand() % 100 == 0 )
        game.theMatrix[d] = 0;
    }
    ++matd;
  }
}
void gdisplay( gstate* state ){
  glClear( GL_COLOR_BUFFER_BIT );
  glUseProgram( game.compute );
  u32 tilesX = state->clientWidth / 32 + 1;
  u32 tilesY = state->clientHeight / 32 + 1;

  glActiveTexture( GL_TEXTURE3 );
  glBindTexture( GL_TEXTURE_2D, game.matrixTex );

  checkGlErrors( "123" );
  glTexImage2D( GL_TEXTURE_2D, 0, GL_R32UI, game.stringTexWidth, 1, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, game.theMatrix );
  checkGlErrors( "122" );
  glUniform4f( 1, (float)state->clientWidth, (float)state->clientHeight, (float)state->clientWidth / game.stWidth, (float)state->clientHeight / game.stHeight );
  glUniform4i( 5, game.stringTexWidth, game.stringTexHeight, 0, 0 );

  glDispatchCompute( tilesX, tilesY, 1 );
  glUseProgram( game.pipeline );
  glUniform4f( 4, (float)state->clientWidth, (float)state->clientHeight, (float)state->clientWidth / game.stWidth, (float)state->clientHeight / game.stHeight );
  glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
}


