#include "pos-color-vertex.hpp"

void PosColorVertex::init()
{
  ms_layout.begin()
    .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
    .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
    .end();
};

bgfx::VertexLayout PosColorVertex::ms_layout;
