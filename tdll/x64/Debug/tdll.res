        ��  ��                  �  (   M I S C   ��e       0 	        #version 460
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

  (   M I S C   ��f       0 	        #VERTEX
#version 460

layout(location = 0) in vec4 pos;
layout(location = 1) in vec4 hmm;

out vec4 vcolor;

void main(){
    gl_Position = hmm;
    vcolor = hmm * vec4(0.5,0.5,0.5,1.0) + vec4(0.5,0.5,0.5,0.0);
}
 

#GEOMETRY
#version 460

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec4 vcolor[3];
out vec4 color;

void main(){
  for(int i = 0; i < 3; i++){
    gl_Position = gl_in[i].gl_Position;
	color = vcolor[i];
    EmitVertex();
  }
  EndPrimitive();
}

#FRAGMENT
#version 460

in vec4 color;
out vec4 res;
uniform sampler2D scrn;
layout( location = 4 ) uniform vec4 mult;

void main(){
  res = vec4( color.x, color.y, color.z, 1.0);
  res = texture2D( scrn, color.xy * mult.zw ); res.w = 1.0;
}

 