#include "camera.hpp"
#include "coordinate-system.hpp"
#include "example-cubes.hpp"
#include "get-natives.hpp"
#include "hello-triangle.hpp"
#include "shaders-tutorial.hpp"
#include "textures.hpp"
#include "transformations.hpp"
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
    auto example = Camera{window, width, height};
    auto done = false;
    auto e = sdl::EventHandler{};
    auto forward = false;
    auto backward = false;
    auto left = false;
    auto right = false;
    auto mouseDx = 0.0f;
    auto mouseDy = 0.0f;
    auto mouseDown = false;
    e.quit = [&done](const SDL_QuitEvent &) { done = true; };
    e.keyDown = [&](const SDL_KeyboardEvent &e) {
      switch (e.keysym.sym)
      {
      case SDLK_q: done = true; break;
      case SDLK_w: forward = true; break;
      case SDLK_s: backward = true; break;
      case SDLK_a: left = true; break;
      case SDLK_d: right = true; break;
      }
    };
    e.keyUp = [&](const SDL_KeyboardEvent &e) {
      switch (e.keysym.sym)
      {
      case SDLK_w: forward = false; break;
      case SDLK_s: backward = false; break;
      case SDLK_a: left = false; break;
      case SDLK_d: right = false; break;
      }
    };
    e.mouseMotion = [&](const SDL_MouseMotionEvent &e) {
      mouseDx = e.xrel * .01f;
      mouseDy = e.yrel * .01f;
    };
    e.mouseButtonDown = [&](const SDL_MouseButtonEvent &e) { mouseDown = true; };
    e.mouseButtonUp = [&](const SDL_MouseButtonEvent &e) { mouseDown = false; };
    while (!done)
    {
      while (e.poll()) {}
      example.update();
      example.strafe((forward ? 1 : 0) + (backward ? -1 : 0), (right ? 1 : 0) + (left ? -1 : 0));
      if (mouseDown)
        example.look(mouseDx, mouseDy);
      mouseDx = 0.0f;
      mouseDy = 0.0f;
    }
  }
  bgfx::shutdown();
  return 0;
}
