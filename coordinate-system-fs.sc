$input v_tex_coord

uniform sampler2D bricks;
uniform sampler2D smile;

void main()
{
  gl_FragColor = mix(texture(bricks, v_tex_coord), texture(smile, v_tex_coord), .5);
}
