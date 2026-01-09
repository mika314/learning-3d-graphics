#pragma once
#include "base-example.hpp"
#include <bgfx/platform.h>
#include <bx/os.h>
#include <sdlpp/sdlpp.hpp>

static const uint64_t s_ptState[]{
  UINT64_C(0),
  BGFX_STATE_PT_TRISTRIP,
  BGFX_STATE_PT_LINES,
  BGFX_STATE_PT_LINESTRIP,
  BGFX_STATE_PT_POINTS,
};

class ExampleCubes final : public BaseExample
{
public:
  ExampleCubes(sdl::Window &, int _width, int _height);
  ~ExampleCubes() final;
  void update() final;

private:
  sdl::Window &m_window;
  uint32_t m_width;
  uint32_t m_height;
  uint32_t m_debug;
  uint32_t m_reset;
  bgfx::VertexBufferHandle m_vbh;
  bgfx::IndexBufferHandle m_ibh[BX_COUNTOF(s_ptState)];
  bgfx::ProgramHandle m_program;
  Uint32 m_timeOffset;
  int32_t m_pt;

  bool m_r;
  bool m_g;
  bool m_b;
  bool m_a;
};
