#include "shaders-tutorial.hpp"
#include "load-program.hpp"
#include <array>

namespace
{
  struct PosVertex
  {
    float x;
    float y;
    float z;
    uint32_t abgr;
    static bgfx::VertexLayout msLayout;
  };

  bgfx::VertexLayout PosVertex::msLayout = []() {
    auto r = bgfx::VertexLayout{};
    r.begin()
      .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
      .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
      .end();
    return r;
  }();
} // namespace

static auto vertices = std::array{PosVertex{-.5f, .5f, 0.0f, 0xff0000ff},
                                  PosVertex{.0f, -.5f, 0.0f, 0xffff0000},
                                  PosVertex{.5f, .5f, 0.0f, 0xff00ff00}};

static auto triList = std::array<uint16_t, 3>{0, 1, 2};

ShadersTutorial::ShadersTutorial(sdl::Window &aWindow, int aWidth, int aHeight)
  : window(aWindow), width(aWidth), height(aHeight)
{
  bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);
  vbh = bgfx::createVertexBuffer(bgfx::makeRef(vertices.data(), vertices.size() * sizeof(vertices[0])),
                                 PosVertex::msLayout);
  ibh = bgfx::createIndexBuffer(bgfx::makeRef(triList.data(), triList.size() * sizeof(triList[0])));
  bgfx::setViewRect(0, 0, 0, width, height);
  vertexColor = bgfx::createUniform("our_color", bgfx::UniformType::Vec4);
  offset = bgfx::createUniform("offset", bgfx::UniformType::Vec4);
  program = loadProgram("vs-shaders-tutorial", "fs-shaders-tutorial");
}

ShadersTutorial::~ShadersTutorial()
{
  bgfx::destroy(offset);
  bgfx::destroy(vertexColor);
  bgfx::destroy(ibh);
  bgfx::destroy(vbh);
  bgfx::destroy(program);
}

auto ShadersTutorial::update() -> void
{
  {
    bgfx::setIndexBuffer(ibh);
    bgfx::setVertexBuffer(0, vbh);
    bgfx::setState(BGFX_STATE_WRITE_R | BGFX_STATE_WRITE_G | BGFX_STATE_WRITE_B | BGFX_STATE_WRITE_A |
                   BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_LESS | BGFX_STATE_CULL_CW |
                   BGFX_STATE_MSAA);
    const auto timeValue = SDL_GetTicks() / 1000.f;
    const auto greenValue = (sin(timeValue) / 2.0f) + 0.5f;
    bgfx::setUniform(vertexColor, std::array{1.0f, greenValue, 1.0f, 1.0f}.data());
    bgfx::setUniform(offset, std::array{.0001f * SDL_GetTicks(), 0.0f, 0.0f, 0.0f}.data());
    bgfx::submit(0, program);
  }

  bgfx::frame();
}
