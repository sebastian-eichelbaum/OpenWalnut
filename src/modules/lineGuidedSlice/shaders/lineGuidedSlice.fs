varying vec4 VaryingTexCoord0;
varying vec4 VaryingTexCoord1;

uniform sampler3D tex0;
uniform sampler3D tex1;

uniform float threshold0;
uniform float threshold1;

uniform int type0;
uniform int type1;

uniform float alpha0;
uniform float alpha1;

#include "colorMaps.fs"

void lookupTex(inout vec4 col, in int type, in sampler3D tex, in float threshold, in vec3 v, in float alpha)
{
    vec3 col1 = vec3(0.0);

    col1 = clamp( texture3D(tex, v).rgb, 0.0, 1.0);

    if ( ( col1.r + col1.g + col1.b ) / 3.0  - threshold <= 0.0) return;

    if ( type == 16 && useColorMap != -1)
    {
        if (threshold < 1.0)
            col1.r = (col1.r - threshold) / (1.0 - threshold);

        colorMap(col1, col1.r);
    }

    col.rgb = mix( col.rgb, col1.rgb, alpha);
}

void main()
{
    vec4 col = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 colFib = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 colFA = vec4(0.0, 0.0, 0.0, 1.0);

    if ( type1 > 0 ) lookupTex(colFA, type1, tex1, threshold1, VaryingTexCoord1.xyz, alpha1);
    if ( type0 > 0 ) lookupTex(colFib, type0, tex0, threshold0, VaryingTexCoord0.xyz, alpha0);

    col = clamp(colFA, 0.0, 1.0);

    if ( ( colFib.r + colFib.g + colFib.b ) < 0.2 )
    {
        discard;
    }
    gl_FragColor = col;
}
