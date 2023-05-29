#ifndef RENDERER_DEF
#define RENDERER_DEF

#include "gl.h"
#include "../common/nui_math.h"

typedef GLuint texture_t;
typedef GLuint shader_t;

typedef struct {
  vec3_t  pos;
  vec3_t  tangent;
  vec3_t  bitangent;
  vec3_t  normal;
  vec2_t  uv;
} vertex_t;

typedef struct {
  GLuint    offset;
  GLuint    count;
} mesh_t;

typedef struct {
  texture_t   diffuse;
  texture_t   normal;
} material_t;

#endif
