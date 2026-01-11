$input v_normal, v_frag_position, v_tex_coord

#include <bgfx_shader.sh>

SAMPLER2D(materialDiffuse, 0);
SAMPLER2D(materialSpecular, 1);
uniform vec4 materialShininess;
uniform vec4 viewPos;

uniform vec4 lightPosition;
uniform vec4 lightAmbient;
uniform vec4 lightDiffuse;
uniform vec4 lightSpecular;

void main()
{
  vec4 ambient = lightAmbient * texture2D(materialDiffuse, v_tex_coord);

  vec3 norm = normalize(v_normal.xyz);
  vec3 lightDir = normalize((lightPosition - v_frag_position).xyz);
  float diff = max(dot(norm, lightDir), 0.0);
  vec4 diffuse = vec4(lightDiffuse.rgb * (diff * texture2D(materialDiffuse, v_tex_coord).rgb), 1.f);

  vec3 viewDir = normalize((viewPos - v_frag_position).xyz);
  vec3 reflectDir = reflect(-lightDir, norm);

  float spec = pow(max(dot(viewDir, reflectDir), 0.0), materialShininess.x);
  vec4 specular = vec4((spec * texture2D(materialSpecular, v_tex_coord).rgb) * lightSpecular.rgb, 1.f);

  vec4 result = ambient + diffuse + specular;
  gl_FragColor = result;
  // gl_FragColor = vec4(1.f, 1.f, 1.f, 1.f);
}
