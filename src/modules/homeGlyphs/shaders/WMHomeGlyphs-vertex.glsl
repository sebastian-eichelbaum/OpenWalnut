#include "WGELighting-vertex.glsl"

void main()
{
    prepareLight();

    gl_FrontColor = gl_Color;

    gl_Position = ftransform();
}
