varying vec4 myColor;
varying vec4 VaryingTexCoord0;


uniform int dimX, dimY, dimZ;

uniform sampler3D tex;
uniform int type;
uniform float threshold;
uniform float minVal;
uniform float maxVal;
uniform int cMap;


#include "colorMaps.fs"
#include "utils.fs"

float lookupTex()
{
    vec3 v = VaryingTexCoord0.xyz;
    v.x = v.x / ( float( dimX ) );
    v.y = v.y / ( float( dimY ) );
    v.z = v.z / ( float( dimZ ) );

    vec3 col1;
    col1.r = clamp( texture3D( tex, v ).r, 0.0, 1.0 );

    if ( col1.r < threshold )
    {
        discard;
    }
    else
        return col1.r;
}


/*
 * simple fragment shader that uses a texture on fibers
 */
void main()
{
    vec4 color = vec4( 1.0 );

    threshold = scaleZeroOne( threshold, minVal, maxVal );// make the threshold lie between 0 and 1

    float value = lookupTex();

    colorMap(color.rgb, value, cMap );

    color.a = 1.0;
    gl_FragColor = color;
}

