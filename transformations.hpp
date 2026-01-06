#pragma once
#include <bgfx/platform.h>
#include <sdlpp/sdlpp.hpp>

class Transformations
{
public:
  Transformations(sdl::Window &, int width, int height);
  auto update() -> void;
  ~Transformations();

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
};
