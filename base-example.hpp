#pragma once
#include <functional>
#include <memory>
#include <sdlpp/sdlpp.hpp>
#include <string>
#include <unordered_map>
#include <vector>

class BaseExample
{
public:
  virtual ~BaseExample() = default;
  virtual auto update() -> void = 0;
  virtual auto strafe(float /*forward*/, float /*right*/) -> void {}
  virtual auto look(float /*dx*/, float /*dy*/) -> void {}

  template <typename T>
  static auto reg(const std::string &name) -> void
  {
    auto &examples = getExamplesMap();
    examples[name] = [](sdl::Window &w, int width, int height) {
      return std::make_unique<T>(w, width, height);
    };
  }

  static auto ctor(const std::string &name, sdl::Window &w, int width, int height)
    -> std::unique_ptr<BaseExample>
  {
    return getExamplesMap()[name](w, width, height);
  }

  static auto getExamples() -> std::vector<std::string>
  {
    auto ret = std::vector<std::string>{};
    auto &examples = getExamplesMap();
    for (const auto &e : examples)
      ret.push_back(e.first);
    return ret;
  }

private:
  static auto getExamplesMap() -> std::unordered_map<
    std::string,
    std::function<auto(sdl::Window &, int width, int height)->std::unique_ptr<BaseExample>>> &;
};

template <typename T>
class Registrator
{
public:
  Registrator(std::string name) { BaseExample::reg<T>(std::move(name)); }
};

#define REG(T, name)                         \
  static Registrator<T> reg = Registrator<T> \
  {                                          \
    name                                     \
  }
