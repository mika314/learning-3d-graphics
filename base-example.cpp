#include "base-example.hpp"

std::unordered_map<
  std::string,
  std::function<auto(sdl::Window &, int width, int height)->std::unique_ptr<BaseExample>>>
  BaseExample::examples;
