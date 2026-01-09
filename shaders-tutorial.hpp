#pragma once
#include "base-example.hpp"
#include <bgfx/platform.h>
#include <sdlpp/sdlpp.hpp>

class ShadersTutorial final : public BaseExample
{
public:
  ShadersTutorial(sdl::Window &, int width, int height);
  auto update() -> void final;
  ~ShadersTutorial() final;

private:
  sdl::Window &window;
  int width;
  int height;
  bgfx::ProgramHandle program;
  bgfx::VertexBufferHandle vbh;
  bgfx::IndexBufferHandle ibh;
  bgfx::UniformHandle vertexColor;
  bgfx::UniformHandle offset;
};
