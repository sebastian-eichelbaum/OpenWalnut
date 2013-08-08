//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2009 OpenWalnut Community, BSV-Leipzig and CNCF-CBS
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

#ifndef WGECOLORMAPS_GLSL
#define WGECOLORMAPS_GLSL

#version 130

uniform int useColorMap;

vec4 blueGreenPurpleColorMap( in float value )
{
    value *= 5.0;
    vec4 color;
    if( value < 0.0 )
    {
        color = vec4( 0.0, 0.0, 0.0, 1.0 );
    }
    else if( value < 1.0 )
    {
        color = vec4( 0.0, value, 1.0, 1.0 );
    }
    else if( value < 2.0 )
    {
        color = vec4( 0.0, 1.0, 2.0 - value, 1.0 );
    }
    else if( value < 3.0 )
    {
        color =  vec4( value - 2.0, 1.0, 0.0, 1.0 );
    }
    else if( value < 4.0 )
    {
        color = vec4( 1.0, 4.0 - value, 0.0, 1.0 );
    }
    else if( value <= 5.0 )
    {
        color = vec4( 1.0, 0.0, value - 4.0, 1.0 );
    }
    else
    {
        color =  vec4( 1.0, 0.0, 1.0, 1.0 );
    }

    return color;
}

vec4 rainbowColorMap( in float value )
{
    float i = floor( 6.0 * value );
    float f = 6.0 * value - i;
    float q = 1.0 - f;

    int iq = int( mod( i, 6.0 ) );

    if( ( iq == 0 ) || ( iq == 6 ) )
    {
        return vec4( 1.0, f, 0.0, 1.0 );
    }
    else if( iq == 1 )
    {
        return vec4( q, 1.0, 0.0, 1.0 );
    }
    else if( iq == 2 )
    {
        return vec4( 0.0, 1.0, f, 1.0 );
    }
    else if( iq == 3 )
    {
        return vec4( 0.0, q, 1.0, 1.0 );
    }
    else if( iq == 4 )
    {
        return vec4( f, 0.0, 1.0, 1.0 );
    }
    else // iq == 5
    {
        return vec4( 1.0, 0.0, q, 1.0 );
    }
}

vec4 hotIronColorMap( in float value )
{
    vec4 color8  = vec4( 255.0 / 255.0, 255.0 / 255.0, 204.0 / 255.0, 1.0 );
    vec4 color7  = vec4( 255.0 / 255.0, 237.0 / 255.0, 160.0 / 255.0, 1.0 );
    vec4 color6  = vec4( 254.0 / 255.0, 217.0 / 255.0, 118.0 / 255.0, 1.0 );
    vec4 color5  = vec4( 254.0 / 255.0, 178.0 / 255.0,  76.0 / 255.0, 1.0 );
    vec4 color4  = vec4( 253.0 / 255.0, 141.0 / 255.0,  60.0 / 255.0, 1.0 );
    vec4 color3  = vec4( 252.0 / 255.0,  78.0 / 255.0,  42.0 / 255.0, 1.0 );
    vec4 color2  = vec4( 227.0 / 255.0,  26.0 / 255.0,  28.0 / 255.0, 1.0 );
    vec4 color1  = vec4( 189.0 / 255.0,   0.0 / 255.0,  38.0 / 255.0, 1.0 );
    vec4 color0  = vec4( 128.0 / 255.0,   0.0 / 255.0,  38.0 / 255.0, 1.0 );

    float colorValue = value * 8.0;
    int sel = int( floor( colorValue ) );

    if( sel >= 8 )
    {
        return color0;
    }
    else if( sel < 0 )
    {
        return color0;
    }
    else
    {
        colorValue -= float( sel );

        if( sel < 1 )
        {
            return ( color1 * colorValue + color0 * ( 1.0 - colorValue ) );
        }
        else if( sel < 2 )
        {
            return ( color2 * colorValue + color1 * ( 1.0 - colorValue ) );
        }
        else if( sel < 3 )
        {
            return ( color3 * colorValue + color2 * ( 1.0 - colorValue ) );
        }
        else if( sel < 4 )
        {
            return ( color4 * colorValue + color3 * ( 1.0 - colorValue ) );
        }
        else if( sel < 5 )
        {
            return ( color5 * colorValue + color4 * ( 1.0 - colorValue ) );
        }
        else if( sel < 6 )
        {
            return ( color6 * colorValue + color5 * ( 1.0 - colorValue ) );
        }
        else if( sel < 7 )
        {
            return ( color7 * colorValue + color6 * ( 1.0 - colorValue ) );
        }
        else if( sel < 8 )
        {
            return ( color8 * colorValue + color7 * ( 1.0 - colorValue ) );
        }
        else
        {
            return color0;
        }
    }
}

vec4 redYellowColorMap( in float value )
{
    vec4 color0 = vec4( 1.0, 0.0, 0.0, 1.0 );
    vec4 color1 = vec4( 1.0, 1.0, 0.0, 1.0 );
    return ( color1 * value + color0 * ( 1.0 - value ) );
}

vec4 vectorColorMap( in vec3 col )
{
    // These weird computations give me the vector directions back as I want them. See WDataTexture3D for floats with dim==3
    float fac = 2.0;
    float dist = 0.5;
    vec3 result = clamp( vec3( abs( col.r - dist ) * fac,
                               abs( col.g - dist ) * fac,
                               abs( col.b - dist ) * fac ), 0.0, 1.0 );
    // This eliminate the effect of the fac concerning zero closeness
    if( result.r + result.g + result.b < ( 0.01 * fac ) )
    {
        result = vec3( 0.0 );
    }
    return vec4( result, 1.0 );
}

vec4 blueLightBlueColorMap( in float value )
{
    vec4 color0 = vec4( 0.0, 0.0, 1.0, 1.0 );
    vec4 color1 = vec4( 0.78, 1.0, 1.0, 1.0 );
    return ( color1 * value + color0 * ( 1.0 - value ) );
}

vec4 negative2positive( in float value )
{
    float valueDescaled = value;
    float minV = 0.0;
    float scaleV = 1.0;

    const vec3 zeroColor = vec3( 1.0, 1.0, 1.0 );
    const vec3 negColor = vec3( 1.0, 1.0, 0.0 );
    const vec3 posColor= vec3( 0.0, 1.0, 1.0 );

    // the descaled value can be in interval [minV,minV+Scale]. But as we want linear scaling where the pos and neg colors are scaled linearly
    // agains each other, and we want to handle real negative values correctly. For only positive values, use their interval mid-point.
    float isNegative = 1.0 - ( -1.0 * clamp( sign( minV ), -1.0, 0.0 ) ); // this is 1.0 if minV is smaller than zero
    float mid = ( 1.0 - isNegative ) * 0.5 * scaleV;    // if negative, the mid point always is 0.0
    // the width of the interval is its original width/2 if there are no negative values in the dataset
    float width = ( isNegative * max( abs( minV ), abs( minV + scaleV ) ) ) + ( ( 1.0 - isNegative ) * mid );

    // pos-neg mix factor
    float share = ( valueDescaled - mid ) / width;

    // use neg color for shares < 0.0 and pos color for the others
    return vec4( zeroColor - ( abs( clamp( share, -1.0 , 0.0 ) * negColor ) + ( clamp( share, 0.0 , 1.0 ) * posColor ) ),
                 1.0 ); // clip zeros is done in colormapping function
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

    if( val == 0.0 )
    {
        return vec4( vec3( 0.0 ), 1.0 );
    }

    if( isBitSet( val, 0.0 ) )
    {
        b = 1.0;
    }
    if( isBitSet( val, 1.0 ) )
    {
        g = 1.0;
    }
    if( isBitSet( val, 2.0 ) )
    {
        r = 1.0;
    }
    if( isBitSet( val, 3.0 ) )
    {
        mult -= 0.15;
        if( r < 1.0 && g < 1.0 && b < 1.0 )
        {
            r = 1.0;
            g = 1.0;
        }
    }
    if( isBitSet( val, 4.0 ) )
    {
        mult -= 0.15;
        if( r < 1.0 && g < 1.0 && b < 1.0 )
        {
            b = 1.0;
            g = 1.0;
        }
    }
    if( isBitSet( val, 5.0 ) )
    {
        mult -= 0.15;
        if( r < 1.0 && g < 1.0 && b < 1.0 )
        {
            r = 1.0;
            b = 1.0;
        }
    }
    if( isBitSet( val, 6.0 ) )
    {
        mult -= 0.15;
        if( r < 1.0 && g < 1.0 && b < 1.0 )
        {
            g = 1.0;
        }
    }
    if( isBitSet( val, 7.0 ) )
    {
        mult -= 0.15;
        if( r < 1.0 && g < 1.0 && b < 1.0 )
        {
            r = 1.0;
        }
    }

    r *= mult;
    g *= mult;
    b *= mult;

    clamp( r, 0.0, 1.0 );
    clamp( g, 0.0, 1.0 );
    clamp( b, 0.0, 1.0 );

    return vec4( r, g, b, 1.0 );
}

vec4 colorMap5( in float value )
{
    vec4 color0 = vec4( 255.0 / 255.0, 255.0 / 255.0, 217.0 / 255.0, 1.0 );
    vec4 color1 = vec4( 237.0 / 255.0, 248.0 / 255.0, 177.0 / 255.0, 1.0 );
    vec4 color2 = vec4( 199.0 / 255.0, 233.0 / 255.0, 180.0 / 255.0, 1.0 );
    vec4 color3 = vec4( 127.0 / 255.0, 205.0 / 255.0, 187.0 / 255.0, 1.0 );
    vec4 color4 = vec4(  65.0 / 255.0, 182.0 / 255.0, 196.0 / 255.0, 1.0 );
    vec4 color5 = vec4(  29.0 / 255.0, 145.0 / 255.0, 192.0 / 255.0, 1.0 );
    vec4 color6 = vec4(  34.0 / 255.0,  94.0 / 255.0, 168.0 / 255.0, 1.0 );
    vec4 color7 = vec4(  37.0 / 255.0,  52.0 / 255.0, 148.0 / 255.0, 1.0 );
    vec4 color8 = vec4(   8.0 / 255.0,  29.0 / 255.0,  88.0 / 255.0, 1.0 );

    float colorValue = value * 8.0;
    int sel = int( floor( colorValue ) );

    if( sel >= 8 )
    {
        return color0;
    }
    else if( sel < 0 )
    {
        return color0;
    }
    else
    {
        colorValue -= float( sel );

        if( sel < 1 )
        {
            return ( color1 * colorValue + color0 * ( 1. - colorValue ) );
        }
        else if( sel < 2 )
        {
            return ( color2 * colorValue + color1 * ( 1. - colorValue ) );
        }
        else if( sel < 3 )
        {
            return ( color3 * colorValue + color2 * ( 1. - colorValue ) );
        }
        else if( sel < 4 )
        {
            return ( color4 * colorValue + color3 * ( 1. - colorValue ) );
        }
        else if( sel < 5 )
        {
            return ( color5 * colorValue + color4 * ( 1. - colorValue ) );
        }
        else if( sel < 6 )
        {
            return ( color6 * colorValue + color5 * ( 1. - colorValue ) );
        }
        else if( sel < 7 )
        {
            return ( color7 * colorValue + color6 * ( 1. - colorValue ) );
        }
        else if( sel < 8 )
        {
            return ( color8 * colorValue + color7 * ( 1. - colorValue ) );
        }
        else
        {
            return color0;
        }
    }
}

vec4 colorMap6( in float value )
{
    float basecolor = 0.0;
    float frequency = 5.0;
    float sqrt3 = sqrt( 3.0 );
    float onedtwodsqrt3 = 1.0 / 2.0 / sqrt3;
    float onepsqrt3 = 1.0 + sqrt3;
    float onemsqrt3 = 1.0 - sqrt3;
    float wvalue = sqrt( 3.0 / 2.0 ) * value * ( 1.0 - value ); // = omega(value)
    float twotz = 2.0 * sqrt3 * value; // = 2.0 * z(value)
    float sinTerm = sin( frequency * value + basecolor );
    float cosTerm = cos( frequency * value + basecolor );
    float wtsinTerm = wvalue * sinTerm;
    float wtcosTerm = wvalue * cosTerm;

    float colorRed   = ( onedtwodsqrt3 * ( onepsqrt3 * wtsinTerm + onemsqrt3 * wtcosTerm + twotz ) );
    float colorGreen = ( onedtwodsqrt3 * ( onemsqrt3 * wtsinTerm + onepsqrt3 * wtcosTerm + twotz ) );
    float colorBlue  = ( onedtwodsqrt3 * ( -2.0 * ( wtsinTerm + wtcosTerm ) + twotz ) );

    return vec4( colorRed, colorGreen, colorBlue, 1.0 );
}

void colorMap( inout vec3 col, in float value, int cmap )
{
    if( cmap == 1 )
    {
        col = rainbowColorMap( value ).rgb;
    }
    else if( cmap == 2 )
    {
        col = hotIronColorMap( value ).rgb;
    }
    else if( cmap == 3 )
    {
        col = negative2positive( value ).rgb;
    }
    else if( cmap == 4 )
    {
        col = atlasColorMap( value ).rgb;
    }
    else if( cmap == 5 )
    {
        col = blueGreenPurpleColorMap( value ).rgb;
    }
    else if( cmap == 6 )
    {
        col = vectorColorMap( col ).rgb;
    }
}

/**
 * This method applies a colormap to the specified value an mixes it with the specified color. It uses the proper colormap and is able to unscale
 * values if needed.
 *
 * \param color this color gets mixed using alpha value with the new colormap color
 * \param value the value to map
 * \param minV the minimum of the original value
 * \param scaleV the scaler used to downscale the original value to [0-1]
 * \param thresholdV a threshold in original space (you need to downscale it to [0-1] if you want to use it to scaled values.
 * \param alpha the alpha blending value
 * \param colormap the colormap index to use
 */
void colormap( inout vec4 color, in vec4 value, float minV, float scaleV, float thresholdV, float alpha, int colormap,
               bool cmactive )
{
    // below threshold?
    bool clip = ( value.r + value.g + value.b ) / 3.0 < ( ( minV + thresholdV ) / scaleV );
    if( clip )
    {
        return;
    }

    vec4 col;
    if( colormap == 0 )
    {
        col = value;
    }
    else if( colormap == 1 )
    {
        col = rainbowColorMap( value.r );
    }
    else if( colormap == 2 )
    {
        col = hotIronColorMap( value.r );
    }
    else if( colormap == 3 )
    {
        col = negative2positive( value.r );
    }
    else if( colormap == 4 )
    {
        col = atlasColorMap( value.r );
    }
    else if( colormap == 5 )
    {
        col = blueGreenPurpleColorMap( value.r );
    }
    else if( colormap == 6 )
    {
        col = vectorColorMap( value.rgb );
    }

    // finally mix colors according to alpha
    color = mix( color, col, float( cmactive ) * alpha );
}

#endif // WGECOLORMAPS_GLSL

