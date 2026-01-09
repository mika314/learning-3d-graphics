#include "hello-triangle.hpp"
#include "get-natives.hpp"
#include "load-program.hpp"
#include <array>

namespace
{
  struct PosVertex
  {
    float x;
    float y;
    float z;
    static bgfx::VertexLayout msLayout;
  };

  bgfx::VertexLayout PosVertex::msLayout = []() {
    auto r = bgfx::VertexLayout{};
    r.begin().add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float).end();
    return r;
  }();
} // namespace

static auto vertices1 = std::array{PosVertex{-.5f - .25f, -.5f, 0.0f}, //
                                   PosVertex{.5f - .25f, -.5f, 0.0f},  //
                                   PosVertex{.0f - .25f, .5f, 0.0f}};

static auto vertices2 = std::array{PosVertex{-.5f + .25f, -.5f, 0.0f}, //
                                   PosVertex{.5f + .25f, -.5f, 0.0f},  //
                                   PosVertex{.0f + .25f, .5f, 0.0f}};

static auto triList = std::array<uint16_t, 4>{0, 1, 2, 0};

HelloTriangle::HelloTriangle(sdl::Window &aWindow, int aWidth, int aHeight)
  : window(aWindow), width(aWidth), height(aHeight)
{
  bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);
  program = loadProgram("hello-triangle-vs", "hello-triangle-fs");
  vbh1 = bgfx::createVertexBuffer(
    bgfx::makeRef(vertices1.data(), vertices1.size() * sizeof(vertices1[0])), PosVertex::msLayout);
  vbh2 = bgfx::createVertexBuffer(
    bgfx::makeRef(vertices2.data(), vertices2.size() * sizeof(vertices2[0])), PosVertex::msLayout);
  ibh = bgfx::createIndexBuffer(bgfx::makeRef(triList.data(), triList.size() * sizeof(triList[0])));
  bgfx::setViewRect(0, 0, 0, width, height);
}

HelloTriangle::~HelloTriangle()
{
  bgfx::destroy(ibh);
  bgfx::destroy(vbh1);
  bgfx::destroy(vbh2);
  bgfx::destroy(program);
}

auto HelloTriangle::update() -> void
{
  {
    bgfx::setIndexBuffer(ibh);
    bgfx::setVertexBuffer(0, vbh1);
    bgfx::setState(BGFX_STATE_WRITE_R | BGFX_STATE_WRITE_G | BGFX_STATE_WRITE_B | BGFX_STATE_WRITE_A |
                   BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_LESS | BGFX_STATE_CULL_CW |
                   BGFX_STATE_MSAA | BGFX_STATE_PT_TRISTRIP);
    bgfx::submit(0, program);
  }
  {
    bgfx::setIndexBuffer(ibh);
    bgfx::setVertexBuffer(0, vbh2);
    bgfx::setState(BGFX_STATE_WRITE_R | BGFX_STATE_WRITE_G | BGFX_STATE_WRITE_B | BGFX_STATE_WRITE_A |
                   BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_LESS | BGFX_STATE_CULL_CW |
                   BGFX_STATE_MSAA | BGFX_STATE_PT_LINESTRIP);
    bgfx::submit(0, program);
  }

  bgfx::frame();
}

REG(HelloTriangle, "tri");
