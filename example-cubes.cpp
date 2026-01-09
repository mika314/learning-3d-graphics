#include "example-cubes.hpp"
#include "get-natives.hpp"
#include <bx/math.h>

BX_PRAGMA_DIAGNOSTIC_PUSH()
BX_PRAGMA_DIAGNOSTIC_IGNORED_CLANG("-Wextern-c-compat")
#include <SDL2/SDL_syswm.h>
BX_PRAGMA_DIAGNOSTIC_POP()

static const char *s_ptNames[]{
  "Triangle List",
  "Triangle Strip",
  "Lines",
  "Line Strip",
  "Points",
};

static_assert(BX_COUNTOF(s_ptState) == BX_COUNTOF(s_ptNames));

namespace
{
  struct PosColorVertex
  {
    float x;
    float y;
    float z;
    uint32_t abgr;

    static bgfx::VertexLayout ms_layout;
  };

  bgfx::VertexLayout PosColorVertex::ms_layout = []() {
    auto r = bgfx::VertexLayout{};
    r.begin()
      .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
      .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
      .end();
    return r;
  }();
} // namespace

static PosColorVertex s_cubeVertices[] = {
  {-1.0f, 1.0f, 1.0f, 0xff000000},
  {1.0f, 1.0f, 1.0f, 0xff0000ff},
  {-1.0f, -1.0f, 1.0f, 0xff00ff00},
  {1.0f, -1.0f, 1.0f, 0xff00ffff},
  {-1.0f, 1.0f, -1.0f, 0xffff0000},
  {1.0f, 1.0f, -1.0f, 0xffff00ff},
  {-1.0f, -1.0f, -1.0f, 0xffffff00},
  {1.0f, -1.0f, -1.0f, 0xffffffff},
};

static const uint16_t s_cubeTriList[] = {
  0, 1, 2,          // 0
  1, 3, 2, 4, 6, 5, // 2
  5, 6, 7, 0, 2, 4, // 4
  4, 2, 6, 1, 5, 3, // 6
  5, 7, 3, 0, 4, 1, // 8
  4, 5, 1, 2, 3, 6, // 10
  6, 3, 7,
};

static const uint16_t s_cubeTriStrip[] = {
  0,
  1,
  2,
  3,
  7,
  1,
  5,
  0,
  4,
  2,
  6,
  7,
  4,
  5,
};

static const uint16_t s_cubeLineList[] = {
  0, 1, 0, 2, 0, 4, 1, 3, 1, 5, 2, 3, 2, 6, 3, 7, 4, 5, 4, 6, 5, 7, 6, 7,
};

static const uint16_t s_cubeLineStrip[] = {
  0,
  2,
  3,
  1,
  5,
  7,
  6,
  4,
  0,
  2,
  6,
  4,
  5,
  7,
  3,
  1,
  0,
};

static const uint16_t s_cubePoints[] = {0, 1, 2, 3, 4, 5, 6, 7};

static const bgfx::Memory *loadMem(const char *_filePath)
{
  FILE *fin = fopen(_filePath, "rb");

  if (fin == nullptr)
  {
    printf("%s not found\n", _filePath);
    fflush(stdout);
    return NULL;
  }

  long file_size;

  if (fseek(fin, 0, SEEK_END) != 0)
  {
    /* Handle error */
  }

  file_size = ftell(fin);
  if (file_size == -1)
  {
    /* Handle error */
  }

  uint32_t size = file_size;
  const bgfx::Memory *mem = bgfx::alloc(size + 1);

  rewind(fin);

  fread(mem->data, 1, size, fin);

  fclose(fin);

  mem->data[mem->size - 1] = '\0';

  return mem;
}

static bgfx::ShaderHandle loadShader(const char *_name)
{
  char filePath[512];

  const char *shaderPath = "???";

  switch (bgfx::getRendererType())
  {
  case bgfx::RendererType::Noop:
  case bgfx::RendererType::Direct3D11:
  case bgfx::RendererType::Direct3D12: shaderPath = "bgfx/bgfx/examples/runtime/shaders/dx11/"; break;
  case bgfx::RendererType::Agc:
  case bgfx::RendererType::Gnm: shaderPath = "bgfx/bgfx/examples/runtime/shaders/pssl/"; break;
  case bgfx::RendererType::Metal: shaderPath = "bgfx/bgfx/examples/runtime/shaders/metal/"; break;
  case bgfx::RendererType::Nvn: shaderPath = "bgfx/bgfx/examples/runtime/shaders/nvn/"; break;
  case bgfx::RendererType::OpenGL: shaderPath = "bgfx/bgfx/examples/runtime/shaders/glsl/"; break;
  case bgfx::RendererType::OpenGLES: shaderPath = "bgfx/bgfx/examples/runtime/shaders/essl/"; break;
  case bgfx::RendererType::Vulkan: shaderPath = "bgfx/bgfx/examples/runtime/shaders/spirv/"; break;

  case bgfx::RendererType::Count: BX_ASSERT(false, "You should not be here!"); break;
  }

  bx::strCopy(filePath, BX_COUNTOF(filePath), shaderPath);
  bx::strCat(filePath, BX_COUNTOF(filePath), _name);
  bx::strCat(filePath, BX_COUNTOF(filePath), ".bin");

  bgfx::ShaderHandle handle = bgfx::createShader(loadMem(filePath));
  bgfx::setName(handle, _name);

  return handle;
}

static bgfx::ProgramHandle loadProgram(const char *_vsName, const char *_fsName)
{
  bgfx::ShaderHandle vsh = loadShader(_vsName);
  bgfx::ShaderHandle fsh = BGFX_INVALID_HANDLE;
  if (NULL != _fsName)
  {
    fsh = loadShader(_fsName);
  }

  return bgfx::createProgram(vsh, fsh, true /* destroy shaders when program is destroyed */);
}

ExampleCubes::ExampleCubes(sdl::Window &w, int _width, int _height)
  : m_window(w), m_pt(0), m_r(true), m_g(true), m_b(true), m_a(true)
{
  m_width = _width;
  m_height = _height;
  m_debug = BGFX_DEBUG_NONE;
  m_reset = BGFX_RESET_VSYNC;

  // Set view 0 clear state.
  bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);

  // Create static vertex buffer.
  m_vbh = bgfx::createVertexBuffer(
    // Static data can be passed with bgfx::makeRef
    bgfx::makeRef(s_cubeVertices, sizeof(s_cubeVertices)),
    PosColorVertex::ms_layout);

  // Create static index buffer for triangle list rendering.
  m_ibh[0] = bgfx::createIndexBuffer(
    // Static data can be passed with bgfx::makeRef
    bgfx::makeRef(s_cubeTriList, sizeof(s_cubeTriList)));

  // Create static index buffer for triangle strip rendering.
  m_ibh[1] = bgfx::createIndexBuffer(
    // Static data can be passed with bgfx::makeRef
    bgfx::makeRef(s_cubeTriStrip, sizeof(s_cubeTriStrip)));

  // Create static index buffer for line list rendering.
  m_ibh[2] = bgfx::createIndexBuffer(
    // Static data can be passed with bgfx::makeRef
    bgfx::makeRef(s_cubeLineList, sizeof(s_cubeLineList)));

  // Create static index buffer for line strip rendering.
  m_ibh[3] = bgfx::createIndexBuffer(
    // Static data can be passed with bgfx::makeRef
    bgfx::makeRef(s_cubeLineStrip, sizeof(s_cubeLineStrip)));

  // Create static index buffer for point list rendering.
  m_ibh[4] = bgfx::createIndexBuffer(
    // Static data can be passed with bgfx::makeRef
    bgfx::makeRef(s_cubePoints, sizeof(s_cubePoints)));

  // Create program from shaders.
  m_program = loadProgram("vs_cubes", "fs_cubes");

  m_timeOffset = SDL_GetTicks(); // SDL_TICKS_PASSED
}

ExampleCubes::~ExampleCubes()
{
  // Cleanup.
  for (uint32_t ii = 0; ii < BX_COUNTOF(m_ibh); ++ii)
  {
    bgfx::destroy(m_ibh[ii]);
  }

  bgfx::destroy(m_vbh);
  bgfx::destroy(m_program);
}

void ExampleCubes::update()
{
  float time = (float)((SDL_GetTicks() - m_timeOffset) / double(1000));

  const bx::Vec3 at = {0.0f, 0.0f, 0.0f};
  const bx::Vec3 eye = {0.0f, 0.0f, -35.0f};

  // Set view and projection matrix for view 0.
  {
    float view[16];
    bx::mtxLookAt(view, eye, at);

    float proj[16];
    bx::mtxProj(
      proj, 60.0f, float(m_width) / float(m_height), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
    bgfx::setViewTransform(0, view, proj);

    // Set view 0 default viewport.
    bgfx::setViewRect(0, 0, 0, uint16_t(m_width), uint16_t(m_height));
  }

  // This dummy draw call is here to make sure that view 0 is cleared
  // if no other draw calls are submitted to view 0.
  bgfx::touch(0);

  bgfx::IndexBufferHandle ibh = m_ibh[m_pt];
  uint64_t state = 0 | (m_r ? BGFX_STATE_WRITE_R : 0) | (m_g ? BGFX_STATE_WRITE_G : 0) |
                   (m_b ? BGFX_STATE_WRITE_B : 0) | (m_a ? BGFX_STATE_WRITE_A : 0) | BGFX_STATE_WRITE_Z |
                   BGFX_STATE_DEPTH_TEST_LESS | BGFX_STATE_CULL_CW | BGFX_STATE_MSAA | s_ptState[m_pt];

  // Submit 11x11 cubes.
  for (uint32_t yy = 0; yy < 11; ++yy)
  {
    for (uint32_t xx = 0; xx < 11; ++xx)
    {
      float mtx[16];
      bx::mtxRotateXY(mtx, time + xx * 0.21f, time + yy * 0.37f);
      mtx[12] = -15.0f + float(xx) * 3.0f;
      mtx[13] = -15.0f + float(yy) * 3.0f;
      mtx[14] = 0.0f;

      // Set model matrix for rendering.
      bgfx::setTransform(mtx);

      // Set vertex and index buffer.
      bgfx::setVertexBuffer(0, m_vbh);
      bgfx::setIndexBuffer(ibh);

      // Set render states.
      bgfx::setState(state);

      // Submit primitive for rendering to view 0.
      bgfx::submit(0, m_program);
    }
  }

  // Advance to next frame. Rendering thread will be kicked to
  // process submitted rendering primitives.
  bgfx::frame();
}

REG(ExampleCubes, "cubes");
