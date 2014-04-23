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

#ifndef WGECOLORMAPSIMPROVED_GLSL
#define WGECOLORMAPSIMPROVED_GLSL

#version 120

#include "WGEUtils.glsl"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Functions to ease clipping.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Clip pixel if it is the specified value. Use return-value as alpha.
 *
 * \param valueDescaled descaled value
 * \param clipValue value to use for clipping.
 *
 * \return 0.0 if clip, 1.0 if not.
 */
float clipIfValue( in float valueDescaled, in float clipValue )
{
    return clamp( sign( abs( valueDescaled - clipValue ) - 0.0001 ), 0.0, 1.0 );
}

/**
 * This function returns 0.0 if the pixel should be clipped. This could be used directly for alpha values. It therefore checks whether a descaled
 * value is 0.0 and clips it if it is.
 *
 * \param valueDescaled the descaled value
 * \param minV minimum
 *
 * \return 0.0 if clip, 1.0 if not.
 */
float clipZero( in float valueDescaled, in float minV )
{
    if( minV > 0.0 )
    {
        return clipIfValue( valueDescaled, minV );
    }
    return clipIfValue( valueDescaled, 0.0 );
}

/**
 * Works similiar to the other clipZero, but uses the vector lenght unstead.
 *
 * \param valueDescaled the descaled vector value
 *
 * \return 0.0 if clip, 1.0 if not.
 */
float clipZero( in vec3 valueDescaled )
{
    return clamp( sign( length( valueDescaled ) - 0.01 ), 0.0, 1.0 );
}

/**
 * The method decides whether the value should be clipped because of the threshold value. This is different for vector and scalar data, which is
 * why this method handles it transparently.
 *
 * \param valueDescaled the descaled data. Scalar or vector.
 * \param colormap if this is a vector colormap, thresholding is done using vector length.
 * \param thresholdVLower the descaled threshold value
 * \param thresholdVUpper the descaled threshold value
 * \param thresholdEnabled flag denoting whether to use thresholding or not
 *
 * \return 0.0 if clipped
 */
float clipThreshold( in vec3 valueDescaled, in int colormap, in float thresholdVLower, in float thresholdVUpper, in bool thresholdEnabled )
{
    float isVec = float( colormap == 6 );

    return max( 1.0 - float( thresholdEnabled ),
            isVec * clamp( sign( length( valueDescaled ) - thresholdVLower ) +
                           sign( - ( length( valueDescaled ) - thresholdVUpper ) ),
                           0.0, 1.0 )
                +
          ( 1.0 - isVec ) * clamp( sign( valueDescaled.r - ( thresholdVLower  ) ) + // or
                                   sign( - ( valueDescaled.r - ( thresholdVUpper ) ) ),
                                   0.0, 1.0 ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Colormaps.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Grayscale colormap. Maps value directly to gray values.
 *
 * \param value the <b>scaled</b> value
 *
 * \return color.
 */
vec4 grayscale( in vec3 value )
{
    return vec4( value, 1.0 );
}

/**
 * Maps red and blue to the positive and negative part of the interval linearly. It handles the zero-point according to specified min and scale
 * values.
 *
 * \note it clips on the mid-point of the scala or 0 if minV < 0
 *
 * \param valueDescaled <b>descaled</b> value
 * \param minV min value
 * \param scaleV scaling factor
 *
 * \return pos2neg colormap
 */
vec4 negative2positive( in float valueDescaled, in float minV, in float scaleV )
{
    /*
    // NOTE: this version should also respect positive-only values.
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
    */

    const vec3 zeroColor = vec3( 1.0, 1.0, 1.0 );
    const vec3 negColor = vec3( 1.0, 1.0, 0.0 );
    const vec3 posColor = vec3( 0.0, 1.0, 1.0 );

    float isNegative = 1.0 - ( -1.0 * clamp( sign( minV ), -1.0, 0.0 ) ); // this is 1.0 if minV is smaller than zero
    float negShare = isNegative * abs( valueDescaled / minV ) * ( 1.0 - step( 0.0, valueDescaled ) );
    float posShare = ( 1.0 - isNegative ) * abs( valueDescaled / ( minV + scaleV ) ) * step( 0.0, valueDescaled );

    vec3 r1 = zeroColor - ( negColor * negShare ) -  ( posColor * posShare );
    return vec4( r1, 1.0 );
}

/**
 * Vector colormap. This basically is a grayscale colormap for each channel. It additionally clips according to vector length.
 *
 * \param valueDescaled the <b>descaled</b> vector data
 * \param minV minimum value
 * \param scaleV scaling value
 *
 * \return colormap.
 */
vec4 vector( in vec3 valueDescaled, in float minV, in float scaleV )
{
    // similar to negative2positive, we need the width of the interval.
    float m = max( abs( minV ), abs( minV + scaleV ) );
    return vec4( abs( valueDescaled / m ), 1.0 );
}

/**
 * Hot Iron colormap. It fades between red and yellowish colors.
 *
 * \param value the scaled value
 *
 * \return color.
 */
vec4 hotIron( in float value )
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

/**
 * Rainbow colormap. Fading through the rainbow colors.
 *
 * \param value the value in [0,1]
 *
 * \return color
 */
vec4 rainbow( in float value )
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

/**
 * Colormap fading between green, blue and purple.
 *
 * \param value the scaled value in [0,1]
 *
 * \return the color
 */
vec4 blueGreenPurple( in float value )
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


/**
 * Checks wether a bit is set. This is done in this function since the &-operator is not available in GLSL 1.20 and GLSL 1.30 is problematic on
 * Mac OS 10.6.*
 *
 * \note Remove this function and replace its calls with bitwise operations as soon as there on all platforms available.
 *
 * \param value the value in [0,1]
 * \param bitpos which bit
 *
 * \return
 */
bool isBitSet( in float value, in float bitpos )
{
    return ( abs( mod( floor( value / pow( 2.0, bitpos ) ), 2.0 ) - 1.0 ) ) < 0.001;
}

/**
 * Colormap especially suitable for mask data. It tries to find distinct colors for neighbouring values.
 *
 * \param value the value in [0,1]
 *
 * \return the volor
 */
vec4 atlas( in float value )
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

/**
 * This method applies a colormap to the specified value an mixes it with the specified color. It uses the proper colormap and works on scaled
 * values.
 *
 * \return this color gets mixed using alpha value with the new colormap color
 * \param value the value to map, <b>scaled</b>
 * \param minV the minimum of the original value *
 * \param scaleV the scaler used to downscale the original value to [0-1]
 * \param clipZeroEnabled if true, zero values get clipped; based on descaled value.
 * \param thresholdVLower a threshold in original space (you need to downscale it to [0-1] if you want to use it to scaled values.
 * \param thresholdVUpper a threshold in original space (you need to downscale it to [0-1] if you want to use it to scaled values.
 * \param thresholdEnabled a flag denoting whether threshold-based clipping should be done or not
 * \param window a window level scaling in the descaled value
 * \param windowEnabled if true, the window level scaling is applied
 * \param alpha the alpha blending value
 * \param colormap the colormap index to use
 * \param cmactive allows disabling this colormap
 */
vec4 colormap( in vec4 value, float minV, float scaleV,
               bool clipZeroEnabled,
               float thresholdVLower, float thresholdVUpper, bool thresholdEnabled,
               vec2  window, bool windowEnabled,
               float alpha, int colormap, bool cmactive )
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Scale the input data to original space, and apply windowing
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // descale value
    vec3 valueDescaled = vec3( minV ) + ( value.rgb * scaleV );

    // apply window scaling. If windowing is disabled, the window is the same as the original data interval
    float winLo = ( window.x  * float( windowEnabled ) ) + // OR
                  ( minV * ( 1.0 - float( windowEnabled ) ) );
    float winUp = ( window.y  * float( windowEnabled ) ) + // OR
                  ( ( minV + scaleV ) * ( 1.0 - float( windowEnabled ) ) );
    float winLen = winUp - winLo;

    // this is the descaled value, scaled using the window interval to be mapped to [0,1]
    vec3 valueWindowedNormalized = ( valueDescaled - vec3( winLo ) ) / winLen;

    // clip values outside the window level
    valueWindowedNormalized = min( vec3( 1.0 ), valueWindowedNormalized );
    valueWindowedNormalized = max( vec3( 0.0 ), valueWindowedNormalized );

    // scale it back to original data space
    vec3 valueWindowedOriginal = vec3( minV ) + ( valueWindowedNormalized * scaleV );

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Do clippings
    //
    // It is important to note that the clip values actually define the pixels alpha value. This means a clip value of 1 means
    // the pixel is not clipped. A clip value of 0 means it gets fully transparent (=clipped in this context).
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // is this a border pixel marked by a 0 alpha value?
    float isNotBorder = float( value.a >= 0.75 );

    // make "zero" values transarent
    float clip = max( float( !clipZeroEnabled ), clipZero( valueWindowedOriginal.r, minV ) );

    // use threshold to clip away fragments.
    // NOTE: thresholding is applied to the original interval in valueDescaled, NOT the window interval
    float clipTh = clipThreshold( valueDescaled, colormap, thresholdVLower, thresholdVUpper, thresholdEnabled );

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Do colormapping
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // this is the final color returned by the colormapping algorithm. This is the correct value for the gray colormap
    vec4 cmapped = grayscale( valueWindowedNormalized.rgb );

    // negative to positive shading in red-blue
    if( colormap == 1 )
    {
        cmapped = rainbow( valueWindowedNormalized.r );
    }
    else if( colormap == 2 )
    {
        cmapped = hotIron( valueWindowedNormalized.r );
    }
    else if( colormap == 3 )
    {
        cmapped = negative2positive( valueWindowedOriginal.r, minV, scaleV );
    }
    else if( colormap == 4 )
    {
        cmapped = atlas( valueWindowedNormalized.r );
    }
    else if( colormap == 5 )
    {
        cmapped = blueGreenPurple( valueWindowedNormalized.r );
    }
    else if( colormap == 6 )
    {
        cmapped = vector( valueWindowedOriginal, minV, scaleV );
        clip = clipZero( valueDescaled );   // vectors get clipped by their length
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Compose
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // build final color
    return vec4( cmapped.rgb, cmapped.a *           // did the colormap use a alpha value?
                              isNotBorder *         // is this a border pixel?
                              alpha *               // did the user specified an alpha?
                              clip *                // value clip?
                              clipTh *              // clip due to threshold?
                              float( cmactive ) );    // is it active?
}

#endif // WGECOLORMAPSIMPROVED_GLSL

