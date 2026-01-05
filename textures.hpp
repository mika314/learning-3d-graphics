#pragma once
#include <bgfx/platform.h>
#include <sdlpp/sdlpp.hpp>

class Textures
{
public:
  Textures(sdl::Window &, int width, int height);
  auto update() -> void;
  ~Textures();

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
};
