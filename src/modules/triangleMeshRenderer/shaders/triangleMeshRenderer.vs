varying vec4 VaryingTexCoord0;

#include "lighting.vs"

void main()
{
    VaryingTexCoord0 = gl_MultiTexCoord0;

    prepareLight();

    gl_Position = ftransform();
}