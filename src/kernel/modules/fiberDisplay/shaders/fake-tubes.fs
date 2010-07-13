varying vec3 tangentR3; // Tangent vector in world space
varying float s_param; // s parameter of texture [-1..1]
varying float tangent_dot_view;
varying vec4 myColor;
uniform bool globalColor;
uniform bool useTexture;

varying vec4 VaryingTexCoord0;
uniform int dimX, dimY, dimZ;

uniform sampler3D tex;
uniform int type;
uniform float threshold;
uniform int cMap;


#include "WGEColorMaps.glsl"

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
    {
        return col1.r;
    }
}
/*
 * simple fragment shader that does rendering of tubes with diffuse illumination
 */
void main()
{

	vec3 color;

	if ( useTexture )
	{
	    float value = lookupTex();
	    colorMap(color.rgb, value, cMap );
	}
	else
        {
            color = abs(normalize(myColor.rgb));
        }

	vec3 view = vec3(0., 0., -1.);
	float view_dot_normal = sqrt(1. - s_param * s_param) + .1;

	gl_FragColor.rgb = clamp(view_dot_normal * (color + 0.15 * pow( view_dot_normal, 10.) *
	        pow(tangent_dot_view, 10.) * vec3(1., 1., 1.)), 0., 1.); //< set the color of this fragment (i.e. pixel)

	gl_FragColor.a = myColor.a;
}

