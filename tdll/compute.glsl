#version 460
layout(local_size_x=32,local_size_y=32) in;

layout( location = 0, rgba32f ) uniform image2D destTex;
layout( location = 1 ) uniform vec4 span;
layout( location = 2 ) uniform int scount;
layout( location = 3 ) uniform int string[300];
layout( location = 5000 ) uniform int font[95*2];


void main(){
  ivec2 index = ivec2(gl_GlobalInvocationID.xy);
  ivec2 pindex = index / 4;
  int letter = ( pindex.x / 6 + ( pindex.y / 8 ) * 40 ) % scount;
  int shift = ( letter % 4 ) * 8;
  letter = letter / 4;
  letter = ( ( string[letter] >> shift ) & 255 ) - 32;

  vec3 lp = vec3(0.0);
  int lx = pindex.x % 6;
  int ly = pindex.y & 7;
  int idx = letter * 2;
  int byte = 8*lx;
  if( byte >= 32 ){
    ++idx;
	byte -= 32;
  }
  int mask = 1 << ly;
  if( ((font[idx] >> byte)&mask) != 0){
    lp.b = index.x / span.x;
	lp.g = 0.9;
    lp.r = index.y / span.y;
  }
  imageStore(destTex, index, vec4( lp, 1.0));
};

