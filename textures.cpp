#include "textures.hpp"
#include "load-program.hpp"
#include "load-texture.hpp"
#include <array>

namespace
{
  struct PosVertex
  {
    float x;
    float y;
    float z;
    float u;
    float v;
    uint32_t c;
    static bgfx::VertexLayout msLayout;
  };

  bgfx::VertexLayout PosVertex::msLayout = []() {
    auto r = bgfx::VertexLayout{};
    r.begin()
      .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
      .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
      .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
      .end();
    return r;
  }();
} // namespace

static auto vertices = std::array{PosVertex{.5f, .5f, 0.0f, 1.f, 0.f, 0xff'00'00'ff},
                                  PosVertex{.5f, -.5f, 0.0f, 1.f, 1.0f, 0xff'00'ff'00},
                                  PosVertex{-.5f, -.5f, 0.0f, 0.0f, 1.0f, 0xff'ff'00'00},
                                  PosVertex{-.5f, .5f, 0.0f, 0.0f, 0.f, 0xff'ff'ff'00}};

static auto triList = std::array<uint16_t, 6>{2, 1, 0, 3, 2, 0};

Textures::Textures(sdl::Window &aWindow, int aWidth, int aHeight)
  : window(aWindow), width(aWidth), height(aHeight)
{
  bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);
  vbh = bgfx::createVertexBuffer(bgfx::makeRef(vertices.data(), vertices.size() * sizeof(vertices[0])),
                                 PosVertex::msLayout);
  ibh = bgfx::createIndexBuffer(bgfx::makeRef(triList.data(), triList.size() * sizeof(triList[0])));
  bgfx::setViewRect(0, 0, 0, width, height);
  offset = bgfx::createUniform("offset", bgfx::UniformType::Vec4);
  bricksTexture = loadTexture("bgfx/bgfx/examples/runtime/textures/fieldstone-rgba.dds");
  bricksUniform = bgfx::createUniform("bricks", bgfx::UniformType::Sampler);
  smileTexture = loadTexture("smile.png");
  smileUniform = bgfx::createUniform("smile", bgfx::UniformType::Sampler);
  program = loadProgram("vs-textures", "fs-textures");
}

Textures::~Textures()
{
  bgfx::destroy(program);
  bgfx::destroy(smileUniform);
  bgfx::destroy(smileTexture);
  bgfx::destroy(bricksUniform);
  bgfx::destroy(bricksTexture);
  bgfx::destroy(offset);
  bgfx::destroy(ibh);
  bgfx::destroy(vbh);
}

auto Textures::update() -> void
{
  {
    bgfx::setIndexBuffer(ibh);
    bgfx::setVertexBuffer(0, vbh);
    bgfx::setState(BGFX_STATE_WRITE_R | BGFX_STATE_WRITE_G | BGFX_STATE_WRITE_B | BGFX_STATE_WRITE_A |
                   BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_LESS | BGFX_STATE_CULL_CW |
                   BGFX_STATE_MSAA);
    bgfx::setUniform(offset, std::array{.0001f * SDL_GetTicks(), 0.0f, 0.0f, 0.0f}.data());
    bgfx::setTexture(0, smileUniform, smileTexture);
    bgfx::setTexture(1, bricksUniform, bricksTexture);
    bgfx::submit(0, program);
  }

  bgfx::frame();
}
