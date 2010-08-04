varying vec4 myColor;
varying vec4 VaryingTexCoord0;

uniform int dimX, dimY, dimZ;

uniform sampler3D tex;
uniform int type;
uniform float threshold;
uniform int cMap;

uniform bool useTexture;
uniform bool useCullBox;
uniform bool insideCullBox;
uniform float cullBoxLBX;
uniform float cullBoxLBY;
uniform float cullBoxLBZ;
uniform float cullBoxUBX;
uniform float cullBoxUBY;
uniform float cullBoxUBZ;


#include "WGEColorMaps.glsl"
#include "WGEUtils.glsl"

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

void checkCullBox()
{
    vec3 pos = VaryingTexCoord0.xyz;
    
    if ( insideCullBox )
    {
        if ( pos.x < cullBoxLBX || pos.x > cullBoxUBX )
            discard;
        if ( pos.y < cullBoxLBY || pos.y > cullBoxUBY )
            discard; 
        if ( pos.z < cullBoxLBZ || pos.z > cullBoxUBZ )
            discard;
    }
    else
    {
        if ( ( pos.x > cullBoxLBX && pos.x < cullBoxUBX ) && ( pos.y > cullBoxLBY && pos.y < cullBoxUBY ) && ( pos.z > cullBoxLBZ && pos.z < cullBoxUBZ ) )
            discard;
    } 
}

/*
 * simple fragment shader that uses a texture on fibers
 */
void main()
{
    if ( useCullBox )
        checkCullBox();

    vec4 color = myColor;

    if ( useTexture )
    {
        float value = lookupTex();
        colorMap(color.rgb, value, cMap );
        color.a = 1.0;
    }
    
    gl_FragColor = color;
}

