#pragma once
#include <bgfx/platform.h>
#include <glm/glm.hpp>
#include <sdlpp/sdlpp.hpp>

class BasicLighting
{
public:
  BasicLighting(sdl::Window &, int width, int height);
  auto update() -> void;
  auto strafe(float forward, float right) -> void;
  auto look(float dx, float dy) -> void;
  ~BasicLighting();

private:
  sdl::Window &window;
  int width;
  int height;
  bgfx::ProgramHandle program;
  bgfx::ProgramHandle programLight;
  bgfx::VertexBufferHandle vbh;
  bgfx::IndexBufferHandle ibh;
  bgfx::TextureHandle bricksTexture;
  bgfx::UniformHandle bricksUniform;
  bgfx::TextureHandle smileTexture;
  bgfx::UniformHandle smileUniform;
  bgfx::UniformHandle transUniform;
  bgfx::UniformHandle objectColor;
  bgfx::UniformHandle lightColor;
  glm::vec3 camPos = glm::vec3{-10.f, 0.0f, 0.0f};
  float camYaw = 0.0f;
  float camPitch = 0.0f;
  glm::vec3 lightPos = glm::vec3{4.f, 0.0f, 2.f};
  bgfx::UniformHandle lightPosUniform;
  bgfx::UniformHandle viewPosUniform;
};
