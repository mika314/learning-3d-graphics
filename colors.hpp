#pragma once
#include "base-example.hpp"
#include <bgfx/platform.h>
#include <glm/glm.hpp>
#include <sdlpp/sdlpp.hpp>

class Colors final : public BaseExample
{
public:
  Colors(sdl::Window &, int width, int height);
  auto update() -> void final;
  auto strafe(float forward, float right) -> void final;
  auto look(float dx, float dy) -> void final;
  ~Colors() final;

private:
  sdl::Window &window;
  int width;
  int height;
  bgfx::ProgramHandle program;
  bgfx::ProgramHandle programLight;
  bgfx::VertexBufferHandle vbh;
  bgfx::IndexBufferHandle ibh;
  bgfx::UniformHandle offset;
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
  glm::vec3 lightPos = glm::vec3{1.2f, 1.f, 2.f};
};
