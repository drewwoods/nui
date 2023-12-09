out vec4 frag_color;
  
in vec2 vs_uv;

uniform sampler2D u_scene;
  
uniform bool u_horizontal;

float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{       
  vec2 tex_offset = 1.0 / vec2(textureSize(u_scene, 0));
  vec3 result = texture(u_scene, vs_uv).rgb * weight[0];
  
  if (u_horizontal) {
    for(int i = 1; i < 5; i++) {
      result += texture(u_scene, vs_uv + vec2(tex_offset.x * float(i), 0.0)).rgb * weight[i];
      result += texture(u_scene, vs_uv + vec2(tex_offset.x * float(i), 0.0)).rgb * weight[i];
    }
  } else {
    for(int i = 1; i < 5; i++) {
      result += texture(u_scene, vs_uv + vec2(0.0, tex_offset.y * float(i))).rgb * weight[i];
      result += texture(u_scene, vs_uv - vec2(0.0, tex_offset.y * float(i))).rgb * weight[i];
    }
  }
  
  frag_color = vec4(result, 1.0);
}
