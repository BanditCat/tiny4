#version 460

const int atomsize = 1024;

layout(local_size_x=32,local_size_y=32) in;

layout( location = 0, rgba8 ) uniform image2D destTex;
layout( location = 3, r8 ) uniform image2D fontTex;
layout( location = 4, r8 ) uniform image2D msgTex;
layout( location = 6, r32ui ) uniform uimage2D matrixTex;
layout( location = 1 ) uniform vec4 span;
layout( location = 2 ) uniform int scount;
layout( location = 5 ) uniform ivec4 txtSize; 
layout( location = 7 ) uniform int msec; 

layout( binding = 0, offset = 0 ) uniform atomic_uint ac[ atomsize ];

uint hash( uint x ) {
    x += ( x << 10u );
    x ^= ( x >>  6u );
    x += ( x <<  3u );
    x ^= ( x >> 11u );
    x += ( x << 15u );
    return x;
}
uint hash( uvec2 v ) { return hash( v.x ^ hash(v.y) ); }

void main(){
  ivec2 index = ivec2( gl_GlobalInvocationID.xy );
  ivec2 pindex = index / 1;
  pindex.y += pindex.x / 6;
  uint param = imageLoad( matrixTex, ivec2( pindex.x / 6, 0 ) ).x;
  float hparm = float( 400 - param ) / 100.0 - float(pindex.y / 8) / ( 0.5 * txtSize.y );
  if( hparm > 1.0 )
    hparm = 0.0;
  int let2 = ( pindex.x / 6 + ( pindex.y / 8 ) * -txtSize.x ) % scount;
  let2 = int( imageLoad( msgTex, ivec2( let2 % txtSize.x, let2 / txtSize.x ) ).x * 256.0 ) - 32;
  int yoff = pindex.x / 6;
  if( hparm > 0.9 ){
   let2 = ( let2 + int(param) ) % 95;
  }
  
  vec3 lp = vec3(0.0);
  int lx = pindex.x % 6;
  int ly = pindex.y & 7;
  int mask = 1 << ly;
  float val = imageLoad( fontTex, ivec2( lx, ly + let2 * 7 ) ).x;
  if( val != 0 && ly != 7 ){
    lp.g = hparm;
    lp.b = lp.g * 0.8;
    if( hparm > 0.9 )
  	  lp.r = ( hparm - 0.9 ) * 10.0;
	else
	  lp.r = 0.0;
  }
  uint counter = atomicCounterIncrement( ac[ 0 ] );
  uint aindex = ( gl_GlobalInvocationID.x + gl_GlobalInvocationID.y * int( span.y ) ) % atomsize;
  float w = imageLoad( destTex, index ).w;
  lp.r = w;
  imageStore( destTex, index, vec4( lp, w ) );

  uint h = hash( gl_GlobalInvocationID.xy + ivec2( msec * 1024, msec * 1024 ) );
  int x = int( ( ( h >> 14 ) & 16383 ) % int( span.x ) );
  int y = int( ( h & 16383 ) % int( span.y ) );
  int x2 = x + ( int( h >> 28 & 3 ) - 1 ) ;
  int y2 = y + ( int( h >> 30 & 3 ) + 2 ) ;
  ivec2 xy = ivec2( x, y );
  ivec2 xy2 = ivec2( x2, y2 );
  vec4 lp4 = imageLoad( destTex, xy );
  vec4 lp42 = imageLoad( destTex, xy2 );
  if( y < 5 && !bool( x & 3 ) )
    lp42.w = 1.0;
  else
	lp42.w = lp4.w - 0.004;
  imageStore( destTex, xy2, lp42 );

};

