varying vec4 VaryingTexCoord0;
varying vec4 VaryingTexCoord1;
varying vec4 VaryingTexCoord2;
varying vec4 VaryingTexCoord3;
varying vec4 VaryingTexCoord4;
varying vec4 VaryingTexCoord5;
varying vec4 VaryingTexCoord6;
varying vec4 VaryingTexCoord7;

#include "WGELighting.glsl"

void main()
{
    VaryingTexCoord0 = gl_MultiTexCoord0;
    VaryingTexCoord1 = gl_MultiTexCoord1;
    VaryingTexCoord2 = gl_MultiTexCoord2;
    VaryingTexCoord3 = gl_MultiTexCoord3;
    VaryingTexCoord4 = gl_MultiTexCoord4;
    VaryingTexCoord5 = gl_MultiTexCoord5;
    VaryingTexCoord6 = gl_MultiTexCoord6;
    VaryingTexCoord7 = gl_MultiTexCoord7;

    prepareLight();

    gl_FrontColor = gl_Color;

    gl_Position = ftransform();
}
