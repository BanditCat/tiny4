#version 460
layout(local_size_x=32,local_size_y=32) in;

layout( location = 0, rgba32f ) uniform image2D destTex;
layout( location = 1 ) uniform vec4 span;
layout( location = 2 ) uniform int font[2];


void main() {
  ivec2 index = ivec2(gl_GlobalInvocationID.xy);
  ivec2 pindex = index / 20;
  vec3 lp = vec3(0.0);
  int lx = pindex.x & 7;
  int ly = pindex.y & 7;
  if( lx > 5 || ly > 6 )
    lp.g = 1.0;
  int idx = 0;
  int byte = 8*lx;
  if( byte >= 32 ){
    ++idx;
	byte -= 32;
  }
  int mask = 1 << ly;
  if( ((font[idx] >> byte)&mask) != 0)
    lp.b = 1.0;
  imageStore(destTex, index, vec4( lp, 1.0));
};

