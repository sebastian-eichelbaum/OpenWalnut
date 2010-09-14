#include "WGELighting-vertex.glsl"

#include "WGETransformationTools.glsl"
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
    prepareLight();
    v_normal = gl_Normal;

    // transform the ray direction to texture space, which equals object space
    // Therefore use two points, as we transform a vector
    vec4 camLookAt = vec4( 0.0, 0.0, -1.0, 1.0 );
    vec4 camPos    = vec4( 0.0, 0.0, 0.0, 1.0 );
    v_ray = worldToLocal( camLookAt, camPos ).xyz;

    // also get the coordinates of the light
    vec4 lpos = gl_LightSource[0].position;
    lpos = vec4( 0.0, 0.0, 1000.0, 1.0 );
    v_lightSource = worldToLocal( lpos ).xyz;
    gl_FrontColor = gl_Color;

    gl_Position = ftransform();
}
