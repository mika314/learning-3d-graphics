$input v_color1

uniform vec4 our_color;

void main()
{
  gl_FragColor = v_color1 * our_color;
}
