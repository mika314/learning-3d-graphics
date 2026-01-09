$input v_normal, v_frag_position

uniform vec4 materialAmbient;
uniform vec4 materialDiffuse;
uniform vec4 materialSpecular;
uniform vec4 materialShininess;
uniform vec4 viewPos;

uniform vec4 lightPosition;
uniform vec4 lightAmbient;
uniform vec4 lightDiffuse;
uniform vec4 lightSpecular;

void main()
{
  vec4 ambient = lightAmbient * materialAmbient;

  vec3 norm = normalize(v_normal.xyz);
  vec3 lightDir = normalize((lightPosition - v_frag_position).xyz);
  float diff = max(dot(norm, lightDir), 0.0);
  vec4 diffuse = vec4(lightDiffuse.rgb * (diff * materialDiffuse.rgb), 1.f);

  vec3 viewDir = normalize((viewPos - v_frag_position).xyz);
  vec3 reflectDir = reflect(-lightDir, norm);

  float spec = pow(max(dot(viewDir, reflectDir), 0.0), materialShininess.x);
  vec4 specular = vec4((spec * materialSpecular.rgb) * lightSpecular.rgb, 1.f);

  vec4 result = ambient + diffuse + specular;
  gl_FragColor = result;
}
