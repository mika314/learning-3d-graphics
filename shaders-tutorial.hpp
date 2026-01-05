#pragma once
#include <bgfx/platform.h>
#include <sdlpp/sdlpp.hpp>

class ShadersTutorial
{
public:
  ShadersTutorial(sdl::Window &, int width, int height);
  auto update() -> void;
  ~ShadersTutorial();

private:
  sdl::Window &window;
  int width;
  int height;
  bgfx::ProgramHandle program;
  bgfx::VertexBufferHandle vbh;
  bgfx::IndexBufferHandle ibh;
  bgfx::UniformHandle vertexColor;
};
