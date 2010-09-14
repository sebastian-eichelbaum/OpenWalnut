#include "WGEColorMaps.glsl"
#include "WGELighting-fragment.glsl"
#include "WGEShadingTools.glsl"

// The ray's starting point in texture space
varying vec3 v_rayStart;

// The ray direction in texture space
varying vec3 v_ray;

// the Surface normal at this point
varying vec3 v_normal;

// The light source in local coordinates
varying vec3 v_lightSource;


void main()
{
    vec4 col;

    float light = blinnPhongIlluminationIntensity(
        0.1,                                // material ambient
        0.75,                               // material diffuse
        1.3,                                // material specular
        10.0,                               // shinines
        1.0,                                // light diffuse
        0.75,                               // light ambient
        normalize( v_normal ),              // normal
        normalize( v_ray ),                 // view direction
        normalize( v_lightSource )          // light source position
        );

    col = light * gl_Color;
    col.a = 1.0;
    gl_FragColor = col;
}
