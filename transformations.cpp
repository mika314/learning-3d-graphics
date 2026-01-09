#include "transformations.hpp"
#include "load-program.hpp"
#include "load-texture.hpp"
#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

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

Transformations::Transformations(sdl::Window &aWindow, int aWidth, int aHeight)
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
  transUniform = bgfx::createUniform("trans", bgfx::UniformType::Mat4);
  program = loadProgram("transformations-vs", "transformations-fs");
}

Transformations::~Transformations()
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

auto Transformations::update() -> void
{
  {
    bgfx::setIndexBuffer(ibh);
    bgfx::setVertexBuffer(0, vbh);
    bgfx::setState(BGFX_STATE_WRITE_R | BGFX_STATE_WRITE_G | BGFX_STATE_WRITE_B | BGFX_STATE_WRITE_A |
                   BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_LESS | BGFX_STATE_CULL_CW |
                   BGFX_STATE_MSAA);
    auto secs = SDL_GetTicks() / 1000.f;
    bgfx::setUniform(offset, std::array{.1f * secs, 0.0f, 0.0f, 0.0f}.data());
    bgfx::setTexture(0, smileUniform, smileTexture);
    bgfx::setTexture(1, bricksUniform, bricksTexture);

    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::rotate(trans, glm::radians(50.f * secs), glm::vec3(0.0, 0.0, 1.0));
    trans = glm::scale(trans, glm::vec3(1., 1080. / 1920., 1.));
    bgfx::setUniform(transUniform, &trans);
    bgfx::submit(0, program);
  }

  bgfx::frame();
}

REG(Transformations, "trans");
