$input a_position
$output v_texcoord

uniform vec2 u_viewSize;

void main()
{
    v_texcoord   = a_texcoord;
    gl_Position  = vec4(mul(mul(2.0, a_position.x), rcp(u_viewSize.x)) - 1.0,
                        1.0 - mul(mul(2.0, a_position.y), rcp(u_viewSize.y)),
                        0, 1);
}