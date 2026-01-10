#include "base-example.hpp"

auto BaseExample::getExamplesMap() -> std::unordered_map<
  std::string,
  std::function<auto(sdl::Window &, int width, int height)->std::unique_ptr<BaseExample>>> &
{
  static std::unordered_map<
    std::string,
    std::function<auto(sdl::Window &, int width, int height)->std::unique_ptr<BaseExample>>>
    tmp;
  return tmp;
}
