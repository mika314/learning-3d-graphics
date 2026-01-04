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

static auto vertices = std::array{PosVertex{-0.5f, -0.5f, 0.0f}, //
                                  PosVertex{0.5f, -0.5f, 0.0f},  //
                                  PosVertex{0.0f, 0.5f, 0.0f}};

static auto triList = std::array<uint16_t, 4>{0, 1, 2, 0};

HelloTriangle::HelloTriangle(sdl::Window &aWindow, int aWidth, int aHeight)
  : window(aWindow), width(aWidth), height(aHeight)
{
  bgfx::init([this]() {
    auto r = bgfx::Init{};
    r.type = bgfx::RendererType::OpenGL; // bgfx::RendererType::Vulkan
    r.vendorId = BGFX_PCI_ID_NONE;
    r.platformData.nwh = getNativeWindowHandle(window);
    r.platformData.ndt = getNativeDisplayHandle(window);
    r.resolution.width = width;
    r.resolution.height = height;
    r.resolution.reset = BGFX_RESET_VSYNC;
    return r;
  }());
  bgfx::setDebug(BGFX_DEBUG_NONE);
  bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);
  program = loadProgram("vs-hello-triangle", "fs-hello-triangle");
  vbh = bgfx::createVertexBuffer(bgfx::makeRef(vertices.data(), vertices.size() * sizeof(vertices[0])),
                                 PosVertex::msLayout);
  ibh = bgfx::createIndexBuffer(bgfx::makeRef(triList.data(), triList.size() * sizeof(triList[0])));
  bgfx::setViewRect(0, 0, 0, width, height);
}

HelloTriangle::~HelloTriangle()
{
  bgfx::destroy(ibh);
  bgfx::destroy(vbh);
  bgfx::destroy(program);
  bgfx::shutdown();
}

auto HelloTriangle::update() -> void
{
  bgfx::setVertexBuffer(0, vbh);
  bgfx::setIndexBuffer(ibh);
  bgfx::setState(BGFX_STATE_WRITE_R | BGFX_STATE_WRITE_G | BGFX_STATE_WRITE_B | BGFX_STATE_WRITE_A |
                 BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_LESS | BGFX_STATE_CULL_CW | BGFX_STATE_MSAA |
                 BGFX_STATE_PT_LINESTRIP);
  bgfx::submit(0, program);
  bgfx::frame();
}
