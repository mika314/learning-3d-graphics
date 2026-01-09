#pragma once
#include "base-example.hpp"
#include <bgfx/platform.h>
#include <glm/glm.hpp>
#include <sdlpp/sdlpp.hpp>

class Camera final : public BaseExample
{
public:
  Camera(sdl::Window &, int width, int height);
  auto update() -> void final;
  auto strafe(float forward, float right) -> void;
  auto look(float dx, float dy) -> void;
  ~Camera() final;

private:
  sdl::Window &window;
  int width;
  int height;
  bgfx::ProgramHandle program;
  bgfx::VertexBufferHandle vbh;
  bgfx::IndexBufferHandle ibh;
  bgfx::UniformHandle offset;
  bgfx::TextureHandle bricksTexture;
  bgfx::UniformHandle bricksUniform;
  bgfx::TextureHandle smileTexture;
  bgfx::UniformHandle smileUniform;
  bgfx::UniformHandle transUniform;
  glm::vec3 camPos = glm::vec3{-10.f, 0.0f, 0.0f};
  float camYaw = 0.0f;
  float camPitch = 0.0f;
};
