#pragma once
#include "base-example.hpp"
#include <bgfx/platform.h>
#include <sdlpp/sdlpp.hpp>

class HelloTriangle final : public BaseExample
{
public:
  HelloTriangle(sdl::Window &, int width, int height);
  auto update() -> void final;
  ~HelloTriangle() final;

private:
  sdl::Window &window;
  int width;
  int height;
  bgfx::ProgramHandle program;
  bgfx::VertexBufferHandle vbh1;
  bgfx::VertexBufferHandle vbh2;
  bgfx::IndexBufferHandle ibh;
};
