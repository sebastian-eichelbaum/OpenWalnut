varying vec4 VaryingTexCoord0;

#include "WGELighting-vertex.glsl"

void main()
{
    VaryingTexCoord0 = gl_MultiTexCoord0;

    prepareLight();

    gl_FrontColor = gl_Color;

    gl_Position = ftransform();
}
