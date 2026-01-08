$input v_normal, v_frag_position

uniform vec4 lightColor;
uniform vec4 objectColor;
uniform vec4 lightPos;
uniform vec4 viewPos;

void main()
{
  float ambientStrength = 0.1;
  vec4 ambient = ambientStrength * lightColor;

  vec3 norm = normalize(v_normal.xyz);
  vec3 lightDir = normalize((lightPos - v_frag_position).xyz);
  float diff = max(dot(norm, lightDir), 0.0);
  vec4 diffuse = vec4(diff * lightColor.xyz, 1.f);

  float specularStrength = 1.f;

  vec3 viewDir = normalize((viewPos - v_frag_position).xyz);
  vec3 reflectDir = reflect(-lightDir, norm);

  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 256);
  vec4 specular = vec4(specularStrength * spec * lightColor.rgb, 1.f);

  vec4 result = (ambient + diffuse + specular) * objectColor;
  gl_FragColor = result;
}
