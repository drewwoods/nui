#include "renderer.h"

#include "gl.h"
#include "mesh_file.h"
#include "../common/file.h"

static void renderer_init_gl(renderer_t *renderer);
static bool renderer_init_scene(renderer_t *renderer);
static bool renderer_init_shaders(renderer_t *renderer);

static bool renderer_init_material(renderer_t *renderer);
static bool renderer_init_texture(renderer_t *renderer);
static bool renderer_init_mesh(renderer_t *renderer);

static void renderer_render_scene(renderer_t *renderer, const game_t *game);

static void renderer_light_pass(void *data, mat4x4_t light_matrix);

bool renderer_init(renderer_t *renderer)
{
  buffer_init(&renderer->buffer, 4096);
  
  view_init(&renderer->view);
  view_perspective(&renderer->view, 720.0 / 1280.0, to_radians(90.0), 0.1, 100.0);
  
  renderer_init_gl(renderer);
  
  if (!renderer_init_shaders(renderer))
    return false;
  
  if (!renderer_init_scene(renderer))
    return false;
  
  return true;
}

static void renderer_init_gl(renderer_t *renderer)
{
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
}

static bool renderer_init_shaders(renderer_t *renderer)
{
  if (!lights_init(&renderer->lights))
    return false;
  
  if (!skybox_init(&renderer->skybox, &renderer->buffer))
    return false;
  
  return true;
}

static bool renderer_init_scene(renderer_t *renderer)
{
  if (!renderer_init_mesh(renderer))
    return false;
  
  if (!renderer_init_texture(renderer))
    return false;
  
  if (!renderer_init_material(renderer))
    return false;
  
  renderer->lights.shadow_pass.data = renderer;
  renderer->lights.shadow_pass.draw = renderer_light_pass;
  
  light_t light;
  lights_new_light(&renderer->lights, &light);
  light.pos = vec3_init(0.0, 5.0, 0.0);
  light.intensity = 40.0;
  lights_sub_light(&renderer->lights, &light);
  
  return true;
}

void renderer_render(renderer_t *renderer, const game_t *game)
{
  glViewport(0, 0, 1280, 720);
  glClear(GL_DEPTH_BUFFER_BIT);
  
  skybox_render(&renderer->skybox, &renderer->view, game->rotation);
  
  lights_bind(&renderer->lights);
  renderer_render_scene(renderer, game);
}

static void renderer_render_scene(renderer_t *renderer, const game_t *game)
{
  view_move(&renderer->view, game->position, game->rotation);
  lights_set_view_pos(&renderer->lights, game->position);
  lights_set_material(&renderer->tile_mtl);
  view_sub_data(&renderer->view, mat4x4_init_identity());
  glDrawArrays(GL_TRIANGLES, renderer->scene_mesh.offset, renderer->scene_mesh.count);
}

static void renderer_light_pass(void *data, mat4x4_t light_matrix)
{
  renderer_t *renderer = (renderer_t*) data;
  
  view_set(&renderer->view, light_matrix);
  view_sub_data(&renderer->view, mat4x4_init_identity());
  glDrawArrays(GL_TRIANGLES, renderer->scene_mesh.offset, renderer->scene_mesh.count);
}

static bool renderer_init_texture(renderer_t *renderer)
{
  if (!texture_load(&renderer->tile_diffuse_tex, "res/mtl/tile/color.jpg"))
    return false;
  
  if (!texture_load(&renderer->tile_normal_tex, "res/mtl/tile/normal.jpg"))
    return false;
  
  return true;
}

static bool renderer_init_material(renderer_t *renderer)
{
  renderer->tile_mtl.diffuse = renderer->tile_diffuse_tex;
  renderer->tile_mtl.normal = renderer->tile_normal_tex;
  
  return true;
}

static bool renderer_init_mesh(renderer_t *renderer)
{
  struct {
    const char  *path;
    mesh_t      *mesh;
  } meshes[] = {
    { .path = "res/mesh/scene.mesh", &renderer->scene_mesh },
    { .path = "res/mesh/cube.mesh", &renderer->cube_mesh },
  };
  
  for (int i = 0; i < 2; i++) {
    mesh_file_t mesh_file;
    
    if (!mesh_file_load(&mesh_file, meshes[i].path))
      return false;
    
    if (
      !buffer_new_mesh(
        &renderer->buffer,
        meshes[i].mesh,
        mesh_file.vertices,
        mesh_file.num_vertices
      )
    ) {
      return false;
    }
    
    mesh_file_free(&mesh_file);
  }
  
  return true;
}
