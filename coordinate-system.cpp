#include "coordinate-system.hpp"
#include "load-program.hpp"
#include "load-texture.hpp"
#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <ranges>

namespace
{
  struct PosVertex
  {
    float x;
    float y;
    float z;
    float u;
    float v;
    static bgfx::VertexLayout msLayout;
  };

  bgfx::VertexLayout PosVertex::msLayout = []() {
    auto r = bgfx::VertexLayout{};
    r.begin()
      .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
      .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
      .end();
    return r;
  }();
} // namespace

static auto vertices = std::array{
  PosVertex{-0.5f, +0.5f, +0.5f, 0.0f, 0.0f}, PosVertex{+0.5f, +0.5f, +0.5f, 1.0f, 0.0f},
  PosVertex{+0.5f, -0.5f, +0.5f, 1.0f, 1.0f}, PosVertex{-0.5f, -0.5f, +0.5f, 0.0f, 1.0f},

  PosVertex{-0.5f, -0.5f, +0.5f, 0.0f, 0.0f}, PosVertex{+0.5f, -0.5f, +0.5f, 1.0f, 0.0f},
  PosVertex{+0.5f, -0.5f, -0.5f, 1.0f, 1.0f}, PosVertex{-0.5f, -0.5f, -0.5f, 0.0f, 1.0f},

  PosVertex{-0.5f, -0.5f, -0.5f, 0.0f, 0.0f}, PosVertex{+0.5f, -0.5f, -0.5f, 1.0f, 0.0f},
  PosVertex{+0.5f, +0.5f, -0.5f, 1.0f, 1.0f}, PosVertex{-0.5f, +0.5f, -0.5f, 0.0f, 1.0f},

  PosVertex{+0.5f, +0.5f, +0.5f, 0.0f, 0.0f}, PosVertex{-0.5f, +0.5f, +0.5f, 1.0f, 0.0f},
  PosVertex{-0.5f, +0.5f, -0.5f, 1.0f, 1.0f}, PosVertex{+0.5f, +0.5f, -0.5f, 0.0f, 1.0f},

  PosVertex{-0.5f, +0.5f, +0.5f, 0.0f, 0.0f}, PosVertex{-0.5f, -0.5f, +0.5f, 1.0f, 0.0f},
  PosVertex{-0.5f, -0.5f, -0.5f, 1.0f, 1.0f}, PosVertex{-0.5f, +0.5f, -0.5f, 0.0f, 1.0f},

  PosVertex{+0.5f, -0.5f, +0.5f, 0.0f, 0.0f}, PosVertex{+0.5f, +0.5f, +0.5f, 1.0f, 0.0f},
  PosVertex{+0.5f, +0.5f, -0.5f, 1.0f, 1.0f}, PosVertex{+0.5f, -0.5f, -0.5f, 0.0f, 1.0f},

};

static auto triList = std::array<uint16_t, 6 * 6>{
  0 + 4 * 0, 1 + 4 * 0, 2 + 4 * 0, 0 + 4 * 0, 2 + 4 * 0,
  3 + 4 * 0, //
  0 + 4 * 1, 1 + 4 * 1, 2 + 4 * 1, 0 + 4 * 1, 2 + 4 * 1,
  3 + 4 * 1, //
  0 + 4 * 2, 1 + 4 * 2, 2 + 4 * 2, 0 + 4 * 2, 2 + 4 * 2,
  3 + 4 * 2, //
  0 + 4 * 3, 1 + 4 * 3, 2 + 4 * 3, 0 + 4 * 3, 2 + 4 * 3,
  3 + 4 * 3, //
  0 + 4 * 4, 1 + 4 * 4, 2 + 4 * 4, 0 + 4 * 4, 2 + 4 * 4,
  3 + 4 * 4, //
  0 + 4 * 5, 1 + 4 * 5, 2 + 4 * 5, 0 + 4 * 5, 2 + 4 * 5,
  3 + 4 * 5, //
};

CoordinateSystem::CoordinateSystem(sdl::Window &aWindow, int aWidth, int aHeight)
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
  program = loadProgram("coordinate-system-vs", "coordinate-system-fs");
}

CoordinateSystem::~CoordinateSystem()
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

auto CoordinateSystem::update() -> void
{
  auto i = 0;
  for (auto cubePosition : std::array{glm::vec3(0.0f, 0.0f, 0.0f),
                                      glm::vec3(2.0f, 5.0f, -15.0f),
                                      glm::vec3(-1.5f, -2.2f, -2.5f),
                                      glm::vec3(-3.8f, -2.0f, -12.3f),
                                      glm::vec3(2.4f, -0.4f, -3.5f),
                                      glm::vec3(-1.7f, 3.0f, -7.5f),
                                      glm::vec3(1.3f, -2.0f, -2.5f),
                                      glm::vec3(1.5f, 2.0f, -2.5f),
                                      glm::vec3(1.5f, 0.2f, -1.5f),
                                      glm::vec3(-1.3f, 1.0f, -1.5f)})

  {

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, cubePosition);
    float angle = 20.0f * i++;
    model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

    auto secs = SDL_GetTicks() / 1000.f;

    model = glm::rotate(model, glm::radians(-20 * secs), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(-5 * secs), glm::vec3(1.0f, 0.0f, 0.0f));

    glm::mat4 view = glm::mat4(1.0f);
    // note that we’re translating the scene in the reverse direction
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

    glm::mat4 projection;
    projection = glm::perspective(glm::radians(60.0f), 1.f * width / height, 0.1f, 100.0f);

    bgfx::setViewTransform(0, &view, &projection);
    bgfx::setTransform(&model);

    bgfx::setIndexBuffer(ibh);
    bgfx::setVertexBuffer(0, vbh);
    bgfx::setState(BGFX_STATE_WRITE_R | BGFX_STATE_WRITE_G | BGFX_STATE_WRITE_B | BGFX_STATE_WRITE_A |
                   BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_LESS | /*BGFX_STATE_CULL_CCW |*/
                   BGFX_STATE_MSAA);
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
