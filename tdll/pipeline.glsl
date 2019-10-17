#VERTEX
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

void main() {
  for(int i = 0; i < 3; i++) {
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

