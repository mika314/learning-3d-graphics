$input a_position, a_color0
$output v_color0

void main()
{
  gl_Position = vec4(a_position, 1.0); // we give a vec3 to vec4’s constructor
  v_color0 = a_color0;
}
