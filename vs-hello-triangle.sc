#include <bgfx_shader.sh>

$input a_position

void main()
{
  gl_Position = vec4(a_position, 1.0);
}