$input v_texcoord

#include "bgfx_shader.sh"

SAMPLER2D(u_texture0, 0);

void main()
{
    gl_FragColor = texture2D(u_texture0, v_texcoord);
}
