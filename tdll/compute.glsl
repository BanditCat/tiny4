#version 460
layout(local_size_x=32,local_size_y=32) in;

layout( location = 0, rgba8 ) uniform image2D destTex;
layout( location = 3, r8 ) uniform image2D fontTex;
layout( location = 4, r8 ) uniform image2D msgTex;
layout( location = 1 ) uniform vec4 span;
layout( location = 2 ) uniform int scount;
layout( location = 5 ) uniform ivec4 txtSize; 

void main(){
  ivec2 index = ivec2(gl_GlobalInvocationID.xy);
  ivec2 pindex = index / 4;
  pindex.y += pindex.x / 6;
  int let2 = ( pindex.x / 6 + ( pindex.y / 8 ) * -txtSize.x ) % scount;
  let2 = int( imageLoad( msgTex, ivec2( let2 % txtSize.x, let2 / txtSize.x ) ).x * 256.0 ) - 32;
  int yoff = pindex.x / 6;

  vec3 lp = vec3(0.0);
  int lx = pindex.x % 6;
  int ly = pindex.y & 7;
  int mask = 1 << ly;
  float val = imageLoad(fontTex,ivec2(lx,ly+let2*7) ).x;
  if( val != 0 && ly != 7 ){
    lp.b = lp.r = lp.g = 255.0;
  }
  imageStore( destTex, index, vec4( lp, 1.0 ) );
};

