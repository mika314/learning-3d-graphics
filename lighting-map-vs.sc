$input a_position, a_texcoord0, a_normal
$output v_normal, v_frag_position, v_tex_coord

uniform mat4  u_modelViewProj;
uniform mat4  u_model;
uniform mat4  u_view;
uniform mat4  u_proj;

void main()
{
  gl_Position = u_proj * u_view * u_model * vec4(a_position, 1.0);
  v_normal = vec4(mat3(transpose(inverse(u_model))) * a_normal.xyz, 0.0f);
  v_frag_position = u_model * vec4(a_position, 1.0);
  v_tex_coord = a_texcoord0;
}
