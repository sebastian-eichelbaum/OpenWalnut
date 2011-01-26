//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2009 OpenWalnut Community, BSV@Uni-Leipzig and CNCF@MPI-CBS
// For more information see http://www.openwalnut.org/copying
//
// This file is part of OpenWalnut.
//
// OpenWalnut is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// OpenWalnut is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with OpenWalnut. If not, see <http://www.gnu.org/licenses/>.
//
//---------------------------------------------------------------------------

#ifndef WGECOLORMAPPING_FRAGMENT_GLSL
#define WGECOLORMAPPING_FRAGMENT_GLSL

#version 120

#include "WGEColormapping-uniforms.glsl"
#include "WGEColormapping-varyings.glsl"

vec4 blueGreenPurpleColorMap( in float value )
{
    value *= 5.0;
    vec4 color;
    if( value < 0.0 )
        color = vec4( 0.0, 0.0, 0.0, 1.0 );
    else if( value < 1.0 )
        color = vec4( 0.0, value, 1.0, 1.0 );
    else if( value < 2.0 )
        color = vec4( 0.0, 1.0, 2.0-value, 1.0 );
    else if( value < 3.0 )
        color =  vec4( value-2.0, 1.0, 0.0, 1.0 );
    else if( value < 4.0 )
        color = vec4( 1.0, 4.0-value, 0.0, 1.0 );
    else if( value <= 5.0 )
        color = vec4( 1.0, 0.0, value-4.0, 1.0 );
    else
        color =  vec4( 1.0, 0.0, 1.0, 1.0 );
    return color;
}


vec4 rainbowColorMap( in float value )
{
    float i = floor( 6. * value );
    float f = 6. * value - i;
    float q = 1.0 - f;

    int iq = int( mod( i, 6. ) );

    if ( ( iq == 0 ) || ( iq == 6 ) )
        return vec4( 1., f, 0., 1.0 );
    else if (iq == 1)
        return vec4( q, 1., 0., 1.0 );
    else if (iq == 2)
        return vec4( 0., 1., f, 1.0 );
    else if (iq == 3)
        return vec4( 0., q, 1., 1.0 );
    else if (iq == 4)
        return vec4( f, 0., 1., 1.0 );
    else // iq == 5
        return vec4( 1., 0., q, 1.0 );
}

vec4 hotIronColorMap( in float value )
{
    vec4 color8  = vec4( 255. / 255., 255. / 255., 204. / 255., 1. );
    vec4 color7  = vec4( 255. / 255., 237. / 255., 160. / 255., 1. );
    vec4 color6  = vec4( 254. / 255., 217. / 255., 118. / 255., 1. );
    vec4 color5  = vec4( 254. / 255., 178. / 255.,  76. / 255., 1. );
    vec4 color4  = vec4( 253. / 255., 141. / 255.,  60. / 255., 1. );
    vec4 color3  = vec4( 252. / 255.,  78. / 255.,  42. / 255., 1. );
    vec4 color2  = vec4( 227. / 255.,  26. / 255.,  28. / 255., 1. );
    vec4 color1  = vec4( 189. / 255.,   0. / 255.,  38. / 255., 1. );
    vec4 color0  = vec4( 128. / 255.,   0. / 255.,  38. / 255., 1. );

    float colorValue = value * 8.;
    int sel = int( floor( colorValue ) );

    if ( sel >= 8 )
        return color0;
    else if ( sel < 0 )
        return color0;
    else
    {
        colorValue -= float( sel );

        if ( sel < 1 )
            return ( color1 * colorValue + color0 * ( 1. - colorValue ) );
        else if ( sel < 2 )
            return ( color2 * colorValue + color1 * ( 1. - colorValue ) );
        else if ( sel < 3 )
            return ( color3 * colorValue + color2 * ( 1. - colorValue ) );
        else if ( sel < 4 )
            return ( color4 * colorValue + color3 * ( 1. - colorValue ) );
        else if ( sel < 5 )
            return ( color5 * colorValue + color4 * ( 1. - colorValue ) );
        else if ( sel < 6 )
            return ( color6 * colorValue + color5 * ( 1. - colorValue ) );
        else if ( sel < 7 )
            return ( color7 * colorValue + color6 * ( 1. - colorValue ) );
        else if ( sel < 8 )
            return ( color8 * colorValue + color7 * ( 1. - colorValue ) );
        else
            return color0;
    }
}

vec4 redYellowColorMap( in float value )
{
    vec4 color0 = vec4( 1., 0., 0., 1. );
    vec4 color1 = vec4( 1., 1., 0., 1. );
    return ( color1 * value + color0 * ( 1. - value ) );
}

vec4 blueLightBlueColorMap( in float value )
{
    vec4 color0 = vec4( 0., 0., 1., 1. );
    vec4 color1 = vec4( 0.78, 1., 1., 1. );
    return ( color1 * value + color0 * ( 1. - value ) );
}

vec4 negative2positive( in float value )
{
    float val = value * 2.0 - 1.0;

    vec4 zeroColor = vec4( 1., 1., 1., 1.0 );
    vec4 negColor = vec4( 1., 1., 0., 1.0 );
    vec4 posColor= vec4( 0., 1., 1., 1.0 );
    if ( val < -0.5 )
    {
        return ( zeroColor + negColor * val );
    }
    else if ( val > 0.5 )
    {
        return ( zeroColor - posColor * val );
    }
    else
        return vec4( 0.0, 0.0, 0.0, 1.0 );
}

// TODO(math): Remove this function and replace its calls with bitwise operations as soon as there
// on all platforms available. Currently Mac OS 10.6.4. doesn't support GLSL 1.30 which introduces
// those operations.
// e.g. replace: isBitSet( val, 5 ) with ( val & 32 ) == 1
bool isBitSet( in float value, in float bitpos )
{
    return ( abs( mod( floor( value / pow( 2.0, bitpos ) ), 2.0 ) - 1.0 ) ) < 0.001;
}

vec4 atlasColorMap( in float value )
{
    float val = floor( value * 255.0 );
    float r = 0.0;
    float g = 0.0;
    float b = 0.0;
    float mult = 1.0;

    if ( val == 0.0 )
    {
        return vec4( vec3( 0.0 ), 1.0 );
    }

    if ( isBitSet( val, 0.0 ) )
        b = 1.0;
    if ( isBitSet( val, 1.0 ) )
        g = 1.0;
    if ( isBitSet( val, 2.0 ) )
        r = 1.0;
    if ( isBitSet( val, 3.0 ) )
    {
        mult -= 0.15;
        if ( r < 1.0 && g < 1.0 && b < 1.0 )
        {
            r = 1.0;
            g = 1.0;
        }
    }
    if ( isBitSet( val, 4.0 ) )
    {
        mult -= 0.15;
        if ( r < 1.0 && g < 1.0 && b < 1.0 )
        {
            b = 1.0;
            g = 1.0;
        }
    }
    if ( isBitSet( val, 5.0 ) )
    {
        mult -= 0.15;
        if ( r < 1.0 && g < 1.0 && b < 1.0 )
        {
            r = 1.0;
            b = 1.0;
        }
    }
    if ( isBitSet( val, 6.0 ) )
    {
        mult -= 0.15;
        if ( r < 1.0 && g < 1.0 && b < 1.0 )
        {
            g = 1.0;
        }
    }
    if ( isBitSet( val, 7.0 ) )
    {
        mult -= 0.15;
        if ( r < 1.0 && g < 1.0 && b < 1.0 )
        {
            r = 1.0;
        }
    }

    r *= mult;
    g *= mult;
    b *= mult;

    clamp( r, 0., 1. );
    clamp( g, 0., 1. );
    clamp( b, 0., 1. );

    return vec4( r, g, b, 1.0 );
}

vec4 colorMap5( in float value )
{
    vec4 color0 = vec4( 255. / 255., 255. / 255., 217. / 255., 1. );
    vec4 color1 = vec4( 237. / 255., 248. / 255., 177. / 255., 1. );
    vec4 color2 = vec4( 199. / 255., 233. / 255., 180. / 255., 1. );
    vec4 color3 = vec4( 127. / 255., 205. / 255., 187. / 255., 1. );
    vec4 color4 = vec4(  65. / 255., 182. / 255., 196. / 255., 1. );
    vec4 color5 = vec4(  29. / 255., 145. / 255., 192. / 255., 1. );
    vec4 color6 = vec4(  34. / 255.,  94. / 255., 168. / 255., 1. );
    vec4 color7 = vec4(  37. / 255.,  52. / 255., 148. / 255., 1. );
    vec4 color8 = vec4(   8. / 255.,  29. / 255.,  88. / 255., 1. );

    float colorValue = value * 8.;
    int sel = int( floor( colorValue ) );

    if ( sel >= 8 )
        return color0;
    else if ( sel < 0 )
        return color0;
    else
    {
        colorValue -= float( sel );

        if( sel < 1 )
            return ( color1 * colorValue + color0 * ( 1. - colorValue ) );
        else if (sel < 2)
            return ( color2 * colorValue + color1 * ( 1. - colorValue ) );
        else if (sel < 3)
            return ( color3 * colorValue + color2 * ( 1. - colorValue ) );
        else if (sel < 4)
            return ( color4 * colorValue + color3 * ( 1. - colorValue ) );
        else if (sel < 5)
            return ( color5 * colorValue + color4 * ( 1. - colorValue ) );
        else if (sel < 6)
            return ( color6 * colorValue + color5 * ( 1. - colorValue ) );
        else if (sel < 7)
            return ( color7 * colorValue + color6 * ( 1. - colorValue ) );
        else if (sel < 8)
            return ( color8 * colorValue + color7 * ( 1. - colorValue ) );
        else
            return color0;
    }
}

vec4 colorMap6( in float value )
{
    float basecolor = 0.0;
    float frequency = 5.0;
    float sqrt3 = sqrt( 3. );
    float onedtwodsqrt3 = 1. / 2. / sqrt3;
    float onepsqrt3 = 1. + sqrt3;
    float onemsqrt3 = 1. - sqrt3;
    float wvalue = sqrt( 3. / 2. ) * value * ( 1. - value ); // = omega(value)
    float twotz = 2. * sqrt3 * value; // = 2. * z(value)
    float sinTerm = sin( frequency * value + basecolor );
    float cosTerm = cos( frequency * value + basecolor );
    float wtsinTerm = wvalue * sinTerm;
    float wtcosTerm = wvalue * cosTerm;

    float colorRed   = ( onedtwodsqrt3 * ( onepsqrt3 * wtsinTerm + onemsqrt3 * wtcosTerm + twotz ) );
    float colorGreen = ( onedtwodsqrt3 * ( onemsqrt3 * wtsinTerm + onepsqrt3 * wtcosTerm + twotz ) );
    float colorBlue  = ( onedtwodsqrt3 * ( -2. * ( wtsinTerm + wtcosTerm ) + twotz ) );

    return vec4( colorRed, colorGreen, colorBlue, 1.0 );
}

/**
 * This method applies a colormap to the specified value an mixes it with the specified color. It uses the proper colormap and is able to unscale
 * values if needed.
 * 
 * \param color this color gets mixed using alpha value with the new colormap color
 * \param sampler the texture sampler to use
 * \param coord the coordinate where to get the value
 * \param minV the minimum of the original value
 * \param scaleV the scaler used to downscale the original value to [0-1]
 * \param thresholdV a threshold in original space (you need to downscale it to [0-1] if you want to use it to scaled values.
 * \param alpha the alpha blending value
 * \param colormap the colormap index to use
 */
void colormap( inout vec4 color, in sampler3D sampler, in vec3 coord, float minV, float scaleV, float thresholdV, float alpha, int colormap,
               bool active )
{
    // get the value
    vec4 value = texture3D( sampler, coord );

    // below threshold?
    bool clip = ( value.r + value.g + value.b ) / 3.0 < ( ( minV + thresholdV ) / scaleV );
    if ( clip )
    {
        return;
    }

    vec4 col;
    if ( colormap == 0 )
    {
        col = value;
    }
    else if ( colormap == 1 )
    {
        col = rainbowColorMap( value.r );
    }
    else if ( colormap == 2 )
    {
        col = hotIronColorMap( value.r );
    }
    else if ( colormap == 3 )
    {
        //col = redYellowColorMap( value );
        col = negative2positive( value.r );
    }
    else if ( colormap == 4 )
    {
        col = atlasColorMap( value.r );
    }
    else
    {
        col = blueGreenPurpleColorMap( value.r );
    }

    // finally mix colors according to alpha
    color = mix( color, col, float( active ) * alpha );
}



/**
 * Calculates the final colormapping. Call this from your fragment shader.
 * Be aware that this only works with the WGEColormapping class. If you are using real geometry use the other colormapping call. This version
 * takes a unit-cube texture coordinate which gets translated to the right coordinate space of the texture.
 *
 * \note if your are using this method, the call to colormapping() inside your vertex shader is NOT needed.
 *
 * \param texcoord the texture coordinate in the bounding box space of the data
 * \return the final color determined by the user defined colormapping
 */
vec4 colormapping( vec4 texcoord )
{
    vec4 finalColor = vec4( 0.0, 0.0, 0.0, 1.0 );

    // ColormapPreTransform is a mat4 defined by OpenWalnut before compilation
    vec4 t = ColormapPreTransform * texcoord;

    // back to front compositing
#ifdef Colormap7Enabled
    colormap( finalColor, u_colormap7Sampler, ( gl_TextureMatrix[ Colormap7Unit ] * t ).xyz,
              u_colormap7Min, u_colormap7Scale, u_colormap7Threshold,
              u_colormap7Alpha, u_colormap7Colormap, u_colormap7Active );
#endif
#ifdef Colormap6Enabled
    colormap( finalColor, u_colormap6Sampler, ( gl_TextureMatrix[ Colormap6Unit ] * t ).xyz,
              u_colormap6Min, u_colormap6Scale, u_colormap6Threshold,
              u_colormap6Alpha, u_colormap6Colormap, u_colormap6Active );
#endif
#ifdef Colormap5Enabled
    colormap( finalColor, u_colormap5Sampler, ( gl_TextureMatrix[ Colormap5Unit ] * t ).xyz,
              u_colormap5Min, u_colormap5Scale, u_colormap5Threshold,
              u_colormap5Alpha, u_colormap5Colormap, u_colormap5Active );
#endif
#ifdef Colormap4Enabled
    colormap( finalColor, u_colormap4Sampler, ( gl_TextureMatrix[ Colormap4Unit ] * t ).xyz,
              u_colormap4Min, u_colormap4Scale, u_colormap4Threshold,
              u_colormap4Alpha, u_colormap4Colormap, u_colormap4Active );
#endif
#ifdef Colormap3Enabled
    colormap( finalColor, u_colormap3Sampler, ( gl_TextureMatrix[ Colormap3Unit ] * t ).xyz,
              u_colormap3Min, u_colormap3Scale, u_colormap3Threshold,
              u_colormap3Alpha, u_colormap3Colormap, u_colormap3Active );
#endif
#ifdef Colormap2Enabled
    colormap( finalColor, u_colormap2Sampler, ( gl_TextureMatrix[ Colormap2Unit ] * t ).xyz,
              u_colormap2Min, u_colormap2Scale, u_colormap2Threshold,
              u_colormap3Alpha, u_colormap2Colormap, u_colormap2Active );
#endif
#ifdef Colormap1Enabled
    colormap( finalColor, u_colormap1Sampler, ( gl_TextureMatrix[ Colormap1Unit ] * t ).xyz,
              u_colormap1Min, u_colormap1Scale, u_colormap1Threshold,
              u_colormap1Alpha, u_colormap1Colormap, u_colormap1Active );
#endif
#ifdef Colormap0Enabled
    colormap( finalColor, u_colormap0Sampler, ( gl_TextureMatrix[ Colormap0Unit ] * t ).xyz,
              u_colormap0Min, u_colormap0Scale, u_colormap0Threshold,
              u_colormap0Alpha, u_colormap0Colormap, u_colormap0Active );
#endif

    return finalColor;
}

/**
 * Calculates the final colormapping. Call this from your fragment shader. A call to colormapping() from within the vertex shader is also needed.
 * Be aware that this only works with the WGEColormapping class. This version uses the interpolated texture coordinate from the vertex shader. Be
 * yourself aware that this should be used only for geometry based data. If you are using raytracing-like techniques where only texture
 * coordinates of the proxy geometry is available, use the colormapping( vec3 ) call instead.
 * 
 * \return the final color determined by the user defined colormapping
 */
vec4 colormapping()
{
    vec4 finalColor = vec4( 0.0, 0.0, 0.0, 1.0 );

    // back to front compositing
#ifdef Colormap7Enabled
    colormap( finalColor, u_colormap7Sampler, v_colormap7TexCoord.xyz, u_colormap7Min, u_colormap7Scale, u_colormap7Threshold,
              u_colormap7Alpha, u_colormap7Colormap, u_colormap7Active );
#endif
#ifdef Colormap6Enabled
    colormap( finalColor, u_colormap6Sampler, v_colormap6TexCoord.xyz, u_colormap6Min, u_colormap6Scale, u_colormap6Threshold,
              u_colormap6Alpha, u_colormap6Colormap, u_colormap6Active );
#endif
#ifdef Colormap5Enabled
    colormap( finalColor, u_colormap5Sampler, v_colormap5TexCoord.xyz, u_colormap5Min, u_colormap5Scale, u_colormap5Threshold,
              u_colormap5Alpha, u_colormap5Colormap, u_colormap5Active );
#endif
#ifdef Colormap4Enabled
    colormap( finalColor, u_colormap4Sampler, v_colormap4TexCoord.xyz, u_colormap4Min, u_colormap4Scale, u_colormap4Threshold,
              u_colormap4Alpha, u_colormap4Colormap, u_colormap4Active );
#endif
#ifdef Colormap3Enabled
    colormap( finalColor, u_colormap3Sampler, v_colormap3TexCoord.xyz, u_colormap3Min, u_colormap3Scale, u_colormap3Threshold,
              u_colormap3Alpha, u_colormap3Colormap, u_colormap3Active );
#endif
#ifdef Colormap2Enabled
    colormap( finalColor, u_colormap2Sampler, v_colormap2TexCoord.xyz, u_colormap2Min, u_colormap2Scale, u_colormap2Threshold,
              u_colormap3Alpha, u_colormap2Colormap, u_colormap2Active );
#endif
#ifdef Colormap1Enabled
    colormap( finalColor, u_colormap1Sampler, v_colormap1TexCoord.xyz, u_colormap1Min, u_colormap1Scale, u_colormap1Threshold,
              u_colormap1Alpha, u_colormap1Colormap, u_colormap1Active );
#endif
#ifdef Colormap0Enabled
    colormap( finalColor, u_colormap0Sampler, v_colormap0TexCoord.xyz, u_colormap0Min, u_colormap0Scale, u_colormap0Threshold,
              u_colormap0Alpha, u_colormap0Colormap, u_colormap0Active );
#endif

    return finalColor;
}

#endif // WGECOLORMAPPING_FRAGMENT_GLSL

