$input a_position

void main()
{
  gl_Position = vec4(a_position, 1.0); // we give a vec3 to vec4’s constructor
}
