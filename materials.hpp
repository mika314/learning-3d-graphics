#pragma once
#include "base-example.hpp"
#include <bgfx/platform.h>
#include <glm/glm.hpp>
#include <sdlpp/sdlpp.hpp>

class Materials final : public BaseExample
{
public:
  Materials(sdl::Window &, int width, int height);
  auto update() -> void final;
  auto strafe(float forward, float right) -> void final;
  auto look(float dx, float dy) -> void final;
  ~Materials() final;

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
  bgfx::UniformHandle lightPositionUniform;
  bgfx::UniformHandle lightAmbient;
  bgfx::UniformHandle lightDiffuse;
  bgfx::UniformHandle lightSpecular;
  bgfx::UniformHandle materialAmbient;
  bgfx::UniformHandle materialDiffuse;
  bgfx::UniformHandle materialSpecular;
  bgfx::UniformHandle materialShininess;

  glm::vec3 camPos = glm::vec3{-10.f, 0.0f, 0.0f};
  float camYaw = 0.0f;
  float camPitch = 0.0f;
  bgfx::UniformHandle viewPosUniform;
};
