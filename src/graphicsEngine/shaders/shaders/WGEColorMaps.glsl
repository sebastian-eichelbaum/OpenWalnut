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

uniform int useColorMap;

vec3 blueGreenPurpleColorMap( in float value )
{
    value *= 5.0;
    vec3 color;
    if( value < 0.0 )
        color = vec3( 0.0, 0.0, 0.0 );
    else if( value < 1.0 )
        color = vec3( 0.0, value, 1.0 );
    else if( value < 2.0 )
        color = vec3( 0.0, 1.0, 2.0-value );
    else if( value < 3.0 )
        color =  vec3( value-2.0, 1.0, 0.0 );
    else if( value < 4.0 )
        color = vec3( 1.0, 4.0-value, 0.0 );
    else if( value <= 5.0 )
        color = vec3( 1.0, 0.0, value-4.0 );
    else
        color =  vec3( 1.0, 0.0, 1.0 );
    return color;
}


vec3 rainbowColorMap( in float value )
{
    float i = floor( 6. * value );
    float f = 6. * value - i;
    float q = 1.0 - f;

    int iq = int( mod( i, 6. ) );

    if ( ( iq == 0 ) || ( iq == 6 ) )
        return vec3( 1., f, 0. );
    else if (iq == 1)
        return vec3( q, 1., 0. );
    else if (iq == 2)
        return vec3( 0., 1., f );
    else if (iq == 3)
        return vec3( 0., q, 1. );
    else if (iq == 4)
        return vec3( f, 0., 1. );
    else // iq == 5
        return vec3( 1., 0., q );
}

vec3 hotIronColorMap( in float value )
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
        return color0.rgb;
    else if ( sel < 0 )
        return color0.rgb;
    else
    {
        colorValue -= float( sel );

        if ( sel < 1 )
            return ( color1 * colorValue + color0 * ( 1. - colorValue ) ).rgb;
        else if ( sel < 2 )
            return ( color2 * colorValue + color1 * ( 1. - colorValue ) ).rgb;
        else if ( sel < 3 )
            return ( color3 * colorValue + color2 * ( 1. - colorValue ) ).rgb;
        else if ( sel < 4 )
            return ( color4 * colorValue + color3 * ( 1. - colorValue ) ).rgb;
        else if ( sel < 5 )
            return ( color5 * colorValue + color4 * ( 1. - colorValue ) ).rgb;
        else if ( sel < 6 )
            return ( color6 * colorValue + color5 * ( 1. - colorValue ) ).rgb;
        else if ( sel < 7 )
            return ( color7 * colorValue + color6 * ( 1. - colorValue ) ).rgb;
        else if ( sel < 8 )
            return ( color8 * colorValue + color7 * ( 1. - colorValue ) ).rgb;
        else
            return color0.rgb;
    }
}

vec3 redYellowColorMap( in float value )
{
    vec4 color0 = vec4( 1., 0., 0., 1. );
    vec4 color1 = vec4( 1., 1., 0., 1. );
    return ( color1 * value + color0 * ( 1. - value ) ).rgb;
}

vec3 blueLightBlueColorMap( in float value )
{
    vec4 color0 = vec4( 0., 0., 1., 1. );
    vec4 color1 = vec4( 0.78, 1., 1., 1. );
    return ( color1 * value + color0 * ( 1. - value ) ).rgb;
}

vec3 negative2positive( in float value )
{
    float val = value * 2.0 - 1.0;

    vec3 zeroColor = vec3( 1., 1., 1. );
    vec3 negColor = vec3( 1., 1., 0. );
    vec3 posColor= vec3( 0., 1., 1. );
    if ( val < -0.5 )
    {
        return ( zeroColor + negColor * val );
    }
    else if ( val > 0.5 )
    {
        return ( zeroColor - posColor * val );
    }
    else
        return vec3( 0.0, 0.0, 0.0 );
}

// TODO(math): Remove this function and replace its calls with bitwise operations as soon as there
// on all platforms available. Currently Mac OS 10.6.4. doesn't support GLSL 1.30 which introduces
// those operations.
// e.g. replace: isBitSet( val, 5 ) with ( val & 32 ) == 1
bool isBitSet( in float value, in float bitpos )
{
    return ( abs( mod( floor( value / pow( 2.0, bitpos ) ), 2.0 ) - 1.0 ) ) < 0.001;
}

vec3 atlasColorMap( in float value )
{
    float val = floor( value * 255.0 );
    float r = 0.0;
    float g = 0.0;
    float b = 0.0;
    float mult = 1.0;

    if ( val == 0.0 )
    {
        return vec3( 0.0 );
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

    return vec3( r, g, b );
}



vec3 colorMap5( in float value )
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
        return color0.rgb;
    else if ( sel < 0 )
        return color0.rgb;
    else
    {
        colorValue -= float( sel );

        if( sel < 1 )
            return ( color1 * colorValue + color0 * ( 1. - colorValue ) ).rgb;
        else if (sel < 2)
            return ( color2 * colorValue + color1 * ( 1. - colorValue ) ).rgb;
        else if (sel < 3)
            return ( color3 * colorValue + color2 * ( 1. - colorValue ) ).rgb;
        else if (sel < 4)
            return ( color4 * colorValue + color3 * ( 1. - colorValue ) ).rgb;
        else if (sel < 5)
            return ( color5 * colorValue + color4 * ( 1. - colorValue ) ).rgb;
        else if (sel < 6)
            return ( color6 * colorValue + color5 * ( 1. - colorValue ) ).rgb;
        else if (sel < 7)
            return ( color7 * colorValue + color6 * ( 1. - colorValue ) ).rgb;
        else if (sel < 8)
            return ( color8 * colorValue + color7 * ( 1. - colorValue ) ).rgb;
        else
            return color0.rgb;
    }
}

vec3 colorMap6( in float value )
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

    return vec3( colorRed, colorGreen, colorBlue );
}

void colorMap( inout vec3 col, in float value, int cmap )
{
    if ( cmap == 1 )
        col = rainbowColorMap( value );
    else if ( cmap == 2 )
        col = hotIronColorMap( value );
    else if ( cmap == 3 )
        //col = redYellowColorMap( value );
        col = negative2positive( value );
    else if ( cmap == 4 )
        col = atlasColorMap( value );
    else
        col = blueGreenPurpleColorMap( value );
}
