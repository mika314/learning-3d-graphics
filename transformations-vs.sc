$input a_position, a_color0, a_texcoord0
$output v_tex_coord, v_color1

uniform vec4 offset;
uniform mat4 trans;

void main()
{
  gl_Position = trans * vec4(a_position, 1.0) + offset;
  v_color1 = a_color0;
  v_tex_coord = a_texcoord0;
}
