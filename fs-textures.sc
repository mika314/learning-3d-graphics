$input v_tex_coord, v_color1

uniform sampler2D bricks;
uniform sampler2D smile;

void main()
{
  gl_FragColor = v_color1 * mix(texture(bricks, v_tex_coord), texture(smile, v_tex_coord), .5);
}
