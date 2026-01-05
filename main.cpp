#include "example-cubes.hpp"
#include "get-natives.hpp"
#include "hello-triangle.hpp"
#include "shaders-tutorial.hpp"
#include <bgfx/platform.h>
#include <bx/readerwriter.h>
#include <sdlpp/sdlpp.hpp>

#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
#if ENTRY_CONFIG_USE_WAYLAND
#include <wayland-egl.h>
#endif
#elif BX_PLATFORM_WINDOWS
#define SDL_MAIN_HANDLED
#endif

#if defined(None) // X11 defines this...
#undef None
#endif // defined(None)

auto main(int /*argc*/, char ** /*argv*/) -> int
{
  auto init = sdl::Init(SDL_INIT_VIDEO);
  int width = 1280;
  int height = 720;

  auto window = sdl::Window{
    "bgfx", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN};

  bgfx::renderFrame();

  bgfx::init([&]() {
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
  {
    auto example = ShadersTutorial{window, width, height};
    auto done = false;
    auto e = sdl::EventHandler{};
    e.quit = [&done](const SDL_QuitEvent &) { done = true; };
    e.keyDown = [&done](const SDL_KeyboardEvent &e) { done = done | (e.keysym.sym == SDLK_q); };
    while (!done)
    {
      while (e.poll()) {}
      example.update();
    }
  }
  bgfx::shutdown();
  return 0;
}
