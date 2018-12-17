$input a_position a_texcoord
$output v_texcoord

#include "bgfx_shader.sh"

void main()
{
    v_texcoord  = a_texcoord;
    gl_Position = vec4(2.0 * a_position.x / u_viewRect.z - 1.0,
                       1.0 - 2.0 * a_position.y / u_viewRect.w,
                       0.0, 1.0);
}
