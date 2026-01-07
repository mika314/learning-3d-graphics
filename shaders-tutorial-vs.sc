$input a_position, a_color0
$output v_color1

uniform vec4 offset;

void main()
{
  gl_Position = vec4(a_position, 1.0) + offset;
  v_color1 = a_color0;
  // v_color1 = gl_Position;
}
