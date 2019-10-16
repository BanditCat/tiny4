#version 460
layout(local_size_x=256) in;

layout( location = 0, rgba32f ) uniform image2D destTex;
layout( binding = 0, std140 ) uniform ublock {
  vec2 span;
  float count[256];
};
layout( binding = 1 ) buffer pblock {
  vec4 rect; 
};

void main() {
  ivec2 index = ivec2(gl_GlobalInvocationID.xy);


  imageStore(destTex, index, vec4(index.x, count[index.x], 0.5, 1.0));
};

