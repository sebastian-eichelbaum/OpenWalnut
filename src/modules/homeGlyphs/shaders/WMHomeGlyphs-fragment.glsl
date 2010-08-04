#include "WGEColorMaps.glsl"
#include "WGELighting.glsl"


void main()
{
    vec4 col = gl_Color;

    vec4 ambient = vec4(0.0);
    vec4 diffuse = vec4(0.0);
    vec4 specular = vec4(0.0);

    calculateLighting( -normal, gl_FrontMaterial.shininess, ambient, diffuse, specular );
    calculateLighting(  normal, gl_FrontMaterial.shininess, ambient, diffuse, specular );
    col = (ambient * col / 2.0) + (diffuse * col) + (specular * col / 2.0);
    col = clamp(col, 0.0, 1.0);
    gl_FragColor = col;
}
