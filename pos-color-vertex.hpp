#pragma once
#include <bgfx/bgfx.h>

struct PosColorVertex
{
  float x;
  float y;
  float z;
  uint32_t abgr;

  static void init();
  static bgfx::VertexLayout ms_layout;
};
