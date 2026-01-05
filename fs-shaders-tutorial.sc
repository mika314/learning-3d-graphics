$input v_color0

uniform vec4 our_color;

void main()
{
  gl_FragColor = v_color0 * our_color;
}
