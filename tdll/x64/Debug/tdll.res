        ��  ��                  �  (   M I S C   ��e       0 	        #version 460
layout(local_size_x=32,local_size_y=32) in;

layout( location = 0, rgba8 ) uniform image2D destTex;
layout( location = 3, r8 ) uniform image2D fontTex;
layout( location = 4, r8 ) uniform image2D msgTex;
layout( location = 6, r32ui ) uniform uimage2D matrixTex;
layout( location = 1 ) uniform vec4 span;
layout( location = 2 ) uniform int scount;
layout( location = 5 ) uniform ivec4 txtSize; 

void main(){
  ivec2 index = ivec2(gl_GlobalInvocationID.xy);
  ivec2 pindex = index / 1;
  pindex.y += pindex.x / 6;
  uint param = imageLoad( matrixTex, ivec2( pindex.x / 6, 0 ) ).x;
  float hparm = float( param ) / 100.0 + float(pindex.y / 8) / ( 2 * txtSize.y );
  if( hparm > 1.0 )
    hparm = 0.0;
  int let2 = ( pindex.x / 6 + ( pindex.y / 8 ) * -txtSize.x ) % scount;
  let2 = int( imageLoad( msgTex, ivec2( let2 % txtSize.x, let2 / txtSize.x ) ).x * 256.0 ) - 32;
  int yoff = pindex.x / 6;

  vec3 lp = vec3(0.0);
  int lx = pindex.x % 6;
  int ly = pindex.y & 7;
  int mask = 1 << ly;
  float val = imageLoad( fontTex, ivec2( lx, ly + let2 * 7 ) ).x;
  if( val != 0 && ly != 7 ){
    lp.g = hparm;
    lp.b = lp.g * 0.8;
	lp.r = 0.1;
  }
  imageStore( destTex, index, vec4( lp, 1.0 ) );
};

     (   M I S C   ��f       0 	        #VERTEX
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