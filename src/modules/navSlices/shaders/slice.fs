varying vec4 VaryingTexCoord0;
varying vec4 VaryingTexCoord1;
varying vec4 VaryingTexCoord2;
varying vec4 VaryingTexCoord3;
varying vec4 VaryingTexCoord4;
varying vec4 VaryingTexCoord5;
varying vec4 VaryingTexCoord6;
varying vec4 VaryingTexCoord7;

uniform sampler3D tex0;
uniform sampler3D tex1;
uniform sampler3D tex2;
uniform sampler3D tex3;
uniform sampler3D tex4;
uniform sampler3D tex5;
uniform sampler3D tex6;
uniform sampler3D tex7;
uniform sampler3D tex8;
uniform sampler3D tex9;

uniform float threshold0;
uniform float threshold1;
uniform float threshold2;
uniform float threshold3;
uniform float threshold4;
uniform float threshold5;
uniform float threshold6;
uniform float threshold7;
uniform float threshold8;
uniform float threshold9;

uniform int type0;
uniform int type1;
uniform int type2;
uniform int type3;
uniform int type4;
uniform int type5;
uniform int type6;
uniform int type7;
uniform int type8;
uniform int type9;

uniform float alpha0;
uniform float alpha1;
uniform float alpha2;
uniform float alpha3;
uniform float alpha4;
uniform float alpha5;
uniform float alpha6;
uniform float alpha7;
uniform float alpha8;
uniform float alpha9;


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
    vec4 col = vec4(0.0, 0.0, 0.0, 0.0);

    if ( type9 > 0 ) lookupTex(col, type9, tex9, threshold9, VaryingTexCoord7.xyz, alpha9);
    if ( type8 > 0 ) lookupTex(col, type8, tex8, threshold8, VaryingTexCoord7.xyz, alpha8);
    if ( type7 > 0 ) lookupTex(col, type7, tex7, threshold7, VaryingTexCoord7.xyz, alpha7);
    if ( type6 > 0 ) lookupTex(col, type6, tex6, threshold6, VaryingTexCoord6.xyz, alpha6);
    if ( type5 > 0 ) lookupTex(col, type5, tex5, threshold5, VaryingTexCoord5.xyz, alpha5);
    if ( type4 > 0 ) lookupTex(col, type4, tex4, threshold4, VaryingTexCoord4.xyz, alpha4);
    if ( type3 > 0 ) lookupTex(col, type3, tex3, threshold3, VaryingTexCoord3.xyz, alpha3);
    if ( type2 > 0 ) lookupTex(col, type2, tex2, threshold2, VaryingTexCoord2.xyz, alpha2);
    if ( type1 > 0 ) lookupTex(col, type1, tex1, threshold1, VaryingTexCoord1.xyz, alpha1);
    if ( type0 > 0 ) lookupTex(col, type0, tex0, threshold0, VaryingTexCoord0.xyz, alpha0);

    col = clamp(col, 0.0, 1.0);

    col.a = sqrt( col.r * col.r + col.g * col.g + col.b * col.b);

    if ( ( col.r + col.g + col.b ) < 0.01 )
        discard;

    gl_FragColor = col;
}
