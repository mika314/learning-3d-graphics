uniform vec4 lightColor;
uniform vec4 objectColor;

void main()
{
  gl_FragColor = lightColor * objectColor;
}
