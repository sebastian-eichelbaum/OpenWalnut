varying vec4 VaryingTexCoord0;

uniform int opacity;

#include "WGELighting-fragment.glsl"

void main()
{
    vec4 col = gl_Color;

    vec4 ambient = vec4(0.0);
    vec4 diffuse = vec4(0.0);
    vec4 specular = vec4(0.0);
    calculateLighting(-normal, gl_FrontMaterial.shininess, ambient, diffuse, specular);

    col = (ambient * col / 2.0) + (diffuse * col) + (specular * col / 3.0);

    col = clamp(col, 0.0, 1.0);

    col.a = float( opacity ) * 0.01; // MacOS X has old shader spec, no implicit conversion to float takes place

    gl_FragColor = col;
}
