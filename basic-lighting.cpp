#include "basic-lighting.hpp"
#include "load-program.hpp"
#include "load-texture.hpp"
#include <array>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <log/log.hpp>
#include <ranges>

namespace
{
  struct PosVertex
  {
    float x;
    float y;
    float z;
    float nx;
    float ny;
    float nz;
    float u;
    float v;
    static bgfx::VertexLayout msLayout;
  };

  bgfx::VertexLayout PosVertex::msLayout = []() {
    auto r = bgfx::VertexLayout{};
    r.begin()
      .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
      .add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
      .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
      .end();
    return r;
  }();
} // namespace

static auto vertices = std::array{
  PosVertex{-0.5f, +0.5f, +0.5f, 0.0f, 0.0f, 1.f, 0.0f, 0.0f},
  PosVertex{+0.5f, +0.5f, +0.5f, 0.0f, 0.0f, 1.f, 1.0f, 0.0f},
  PosVertex{+0.5f, -0.5f, +0.5f, 0.0f, 0.0f, 1.f, 1.0f, 1.0f},
  PosVertex{-0.5f, -0.5f, +0.5f, 0.0f, 0.0f, 1.f, 0.0f, 1.0f},

  PosVertex{-0.5f, -0.5f, +0.5f, 0.0f, -1.f, 0.0f, 0.0f, 0.0f},
  PosVertex{+0.5f, -0.5f, +0.5f, 0.0f, -1.f, 0.0f, 1.0f, 0.0f},
  PosVertex{+0.5f, -0.5f, -0.5f, 0.0f, -1.f, 0.0f, 1.0f, 1.0f},
  PosVertex{-0.5f, -0.5f, -0.5f, 0.0f, -1.f, 0.0f, 0.0f, 1.0f},

  PosVertex{-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.f, 0.0f, 0.0f},
  PosVertex{+0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.f, 1.0f, 0.0f},
  PosVertex{+0.5f, +0.5f, -0.5f, 0.0f, 0.0f, -1.f, 1.0f, 1.0f},
  PosVertex{-0.5f, +0.5f, -0.5f, 0.0f, 0.0f, -1.f, 0.0f, 1.0f},

  PosVertex{+0.5f, +0.5f, +0.5f, 0.0f, 1.f, 0.0f, 0.0f, 0.0f},
  PosVertex{-0.5f, +0.5f, +0.5f, 0.0f, 1.f, 0.0f, 1.0f, 0.0f},
  PosVertex{-0.5f, +0.5f, -0.5f, 0.0f, 1.f, 0.0f, 1.0f, 1.0f},
  PosVertex{+0.5f, +0.5f, -0.5f, 0.0f, 1.f, 0.0f, 0.0f, 1.0f},

  PosVertex{-0.5f, +0.5f, +0.5f, -1.f, 0.0f, 0.0f, 0.0f, 0.0f},
  PosVertex{-0.5f, -0.5f, +0.5f, -1.f, 0.0f, 0.0f, 1.0f, 0.0f},
  PosVertex{-0.5f, -0.5f, -0.5f, -1.f, 0.0f, 0.0f, 1.0f, 1.0f},
  PosVertex{-0.5f, +0.5f, -0.5f, -1.f, 0.0f, 0.0f, 0.0f, 1.0f},

  PosVertex{+0.5f, -0.5f, +0.5f, 1.f, 0.0f, 0.0f, 0.0f, 0.0f},
  PosVertex{+0.5f, +0.5f, +0.5f, 1.f, 0.0f, 0.0f, 1.0f, 0.0f},
  PosVertex{+0.5f, +0.5f, -0.5f, 1.f, 0.0f, 0.0f, 1.0f, 1.0f},
  PosVertex{+0.5f, -0.5f, -0.5f, 1.f, 0.0f, 0.0f, 0.0f, 1.0f},

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

BasicLighting::BasicLighting(sdl::Window &aWindow, int aWidth, int aHeight)
  : window(aWindow), width(aWidth), height(aHeight)
{
  bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x000000ff, 1.0f, 0);
  vbh = bgfx::createVertexBuffer(bgfx::makeRef(vertices.data(), vertices.size() * sizeof(vertices[0])),
                                 PosVertex::msLayout);
  ibh = bgfx::createIndexBuffer(bgfx::makeRef(triList.data(), triList.size() * sizeof(triList[0])));
  bgfx::setViewRect(0, 0, 0, width, height);
  bricksTexture = loadTexture("bgfx/bgfx/examples/runtime/textures/fieldstone-rgba.dds");
  bricksUniform = bgfx::createUniform("bricks", bgfx::UniformType::Sampler);
  smileTexture = loadTexture("smile.png");
  smileUniform = bgfx::createUniform("smile", bgfx::UniformType::Sampler);
  transUniform = bgfx::createUniform("trans", bgfx::UniformType::Mat4);
  objectColor = bgfx::createUniform("objectColor", bgfx::UniformType::Vec4);
  lightColor = bgfx::createUniform("lightColor", bgfx::UniformType::Vec4);
  lightPosUniform = bgfx::createUniform("lightPos", bgfx::UniformType::Vec4);
  viewPosUniform = bgfx::createUniform("viewPos", bgfx::UniformType::Vec4);
  program = loadProgram("basic-lighting-vs", "basic-lighting-fs");
  programLight = loadProgram("basic-lighting-vs", "basic-lighting-light-fs");
}

BasicLighting::~BasicLighting()
{
  bgfx::destroy(programLight);
  bgfx::destroy(program);
  bgfx::destroy(viewPosUniform);
  bgfx::destroy(lightPosUniform);
  bgfx::destroy(lightColor);
  bgfx::destroy(objectColor);
  bgfx::destroy(smileUniform);
  bgfx::destroy(smileTexture);
  bgfx::destroy(bricksUniform);
  bgfx::destroy(bricksTexture);
  bgfx::destroy(ibh);
  bgfx::destroy(vbh);
}

auto BasicLighting::update() -> void
{
  {
    const auto view =
      glm::lookAt(camPos,
                  camPos + glm::vec3{glm::rotate(glm::mat4{1.0f}, camYaw, glm::vec3{0.0f, 0.0f, 1.f}) *
                                     glm::rotate(glm::mat4{1.0f}, camPitch, glm::vec3{0.0f, 1.f, 0.0f}) *
                                     glm::vec4{1.f, 0.f, 0.f, 1.f}},
                  glm::vec3(0.0, 0.0, 1.0));

    glm::mat4 projection;
    projection = glm::perspective(glm::radians(60.0f), 1.f * width / height, 0.1f, 100.0f);

    bgfx::setViewTransform(0, &view, &projection);
  }
  const auto secs = SDL_GetTicks() / 1000.f;
  const auto rotMat = glm::rotate(glm::mat4{1.0f}, secs, glm::vec3{0.0f, 0.0f, 1.f});
  lightPos = rotMat * glm::vec4{3.f, 0.0f, -2.0f, 1.f};

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
    auto model = glm::mat4(1.0f);
    model = glm::translate(model, cubePosition);
    float angle = 20.0f * i++;
    model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

    model = glm::rotate(model, glm::radians(-20 * secs), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(-5 * secs), glm::vec3(1.0f, 0.0f, 0.0f));

    bgfx::setTransform(&model);

    bgfx::setIndexBuffer(ibh);
    bgfx::setVertexBuffer(0, vbh);
    bgfx::setState(BGFX_STATE_WRITE_R | BGFX_STATE_WRITE_G | BGFX_STATE_WRITE_B | BGFX_STATE_WRITE_A |
                   BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_LESS | /*BGFX_STATE_CULL_CCW |*/
                   BGFX_STATE_MSAA);
    bgfx::setTexture(0, smileUniform, smileTexture);
    bgfx::setTexture(1, bricksUniform, bricksTexture);

    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::rotate(trans, glm::radians(50.f * secs), glm::vec3(0.0, 0.0, 1.0));
    trans = glm::scale(trans, glm::vec3(1., 1080. / 1920., 1.));
    bgfx::setUniform(transUniform, &trans);

    bgfx::setUniform(objectColor, std::array{1.f, .5f, .31f, 1.f}.data());
    bgfx::setUniform(lightColor, std::array{1.f, 1.f, 1.f, 1.f}.data());
    bgfx::setUniform(lightPosUniform, &lightPos);
    bgfx::setUniform(viewPosUniform, &camPos);

    bgfx::submit(0, program);
  }

  {
    auto model = glm::mat4(1.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.2f));
    bgfx::setTransform(&model);

    bgfx::setIndexBuffer(ibh);
    bgfx::setVertexBuffer(0, vbh);
    bgfx::setState(BGFX_STATE_WRITE_R | BGFX_STATE_WRITE_G | BGFX_STATE_WRITE_B | BGFX_STATE_WRITE_A |
                   BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_LESS | /*BGFX_STATE_CULL_CCW |*/
                   BGFX_STATE_MSAA);

    bgfx::submit(0, programLight);
  }

  bgfx::frame();
}

auto BasicLighting::strafe(float forward, float right) -> void
{
  camPos += 0.2f *
              glm::vec3{glm::rotate(glm::mat4{1.0f}, camYaw, glm::vec3{0.0f, 0.0f, 1.f}) *
                        glm::rotate(glm::mat4{1.0f}, camPitch, glm::vec3{0.0f, 1.f, 0.0f}) *
                        glm::vec4{1.f, 0.f, 0.f, 1.f}} *
              forward +
            0.2f *
              glm::vec3{glm::rotate(glm::mat4{1.0f}, camYaw, glm::vec3{0.0f, 0.0f, 1.f}) *
                        glm::rotate(glm::mat4{1.0f}, camPitch, glm::vec3{0.0f, 1.f, 0.0f}) *
                        glm::vec4{0.f, -1.f, 0.f, 1.f}} *
              right;
}

auto BasicLighting::look(float dx, float dy) -> void
{
  camYaw -= dx;
  camPitch += dy;
  camPitch =
    std::min(static_cast<float>(M_PI / 2.f), std::max(static_cast<float>(-M_PI / 2.f), camPitch));
}

REG(BasicLighting, "light");
