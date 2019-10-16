#VERTEX
#version 460

layout(location = 0) in vec4 pos;

out vec4 vcolor;

void main(){
    gl_Position = pos;
    vcolor = gl_Color;
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

void main(){
  res = vec4( 1.0, color.y, color.z, 1.0);
}

