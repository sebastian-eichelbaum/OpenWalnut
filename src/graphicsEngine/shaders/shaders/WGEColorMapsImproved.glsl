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
    if ( minV > 0.0 )
    {
        return clipIfValue( valueDescaled, minV );
    }
    return clipIfValue( valueDescaled, 0.0 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Colormaps. They need to handle clipping!
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Grayscale colormap. Maps value directly to gray values.
 *
 * \param value the <b>scaled</b> value
 *
 * \return color. Alpha == 0 if value.r == 0.
 */
vec4 grayscale( in vec3 value, in vec3 valueDescaled, in float minV, in float scaleV )
{
    return vec4( value, clipZero( valueDescaled.r, minV ) );
}

/**
 * Maps red and blue to the positive and negative part of the interval linearly. It handles the zero-point according to specified min and scale
 * values.
 *
 * \note it does not show blue values for data that is purely positive (like T1 images and similar).
 *
 * \param valueDescaled <b>descaled</b> value
 * \param minV min value
 * \param scaleV scaling factor
 *
 * \return pos2neg colormap
 */
vec4 negative2positive( in float valueDescaled, in float minV, in float scaleV )
{
    const vec3 zeroColor = vec3( 1.0, 1.0, 1.0 );
    const vec3 negColor = vec3( 1.0, 1.0, 0.0 );
    const vec3 posColor= vec3( 0.0, 1.0, 1.0 );

    // the descaled value can be in interval [minV,minV+Scale]. But as we want linear scaling where the pos and neg colors are scaled linearly
    // agains each other, and we want to handle real negative values correctly. For only positive values, use their interval mid-point.
    float isNegative = ( -1.0 * clamp( sign( minV ), -1.0, 0.0 ) ); // this is 1.0 if minV is smaller than zero
    float mid = ( 1.0 - isNegative ) * 0.5 * scaleV;    // if negative, the mid point always is 0.0
    // the width of the interval is its original width/2 if there are no negative values in the dataset
    float width = ( isNegative * max( abs( minV ), abs( minV + scaleV ) ) ) + ( ( 1.0 - isNegative ) * mid );

    // pos-neg mix factor
    float share = ( valueDescaled - mid ) / width;

    // use neg color for shares < 0.0 and pos color for the others
    return vec4( zeroColor - ( abs( clamp( share, -1.0 , 0.0 ) * negColor ) + ( clamp( share, 0.0 , 1.0 ) * posColor ) ),
                 clipIfValue( valueDescaled, mid ) ); // clip near mid-point
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
    return vec4( abs( valueDescaled / m ),
                 // if the vector length is near zero -> clip
                 clamp( sign( length( valueDescaled ) - 0.01 ), 0.0, 1.0 )
               );
}

/**
 * The method decides whether the value should be clipped because of the threshold value. This is different for vector and scalar data, which is
 * why this method handles it transparently.
 *
 * \param valueDescaled the descaled data. Scalar or vector.
 * \param colormap if this is a vector colormap, thresholding is done using vector length.
 *
 * \return 0.0 if clipped
 */
float clipThreshold( in vec3 valueDescaled, in int colormap, in float thresholdV )
{
    float isVec = float( colormap == 6 );
    return isVec * clamp( sign( length( valueDescaled ) - thresholdV ), 0.0, 1.0 )
                +
          ( 1.0 - isVec ) * clamp( sign( valueDescaled.r - 1.001 * thresholdV ), 0.0, 1.0 );
}

/**
 * This method applies a colormap to the specified value an mixes it with the specified color. It uses the proper colormap and works on scaled
 * values.
 *
 * \param color this color gets mixed using alpha value with the new colormap color
 * \param value the value to map, <b>scaled</b>
 * \param minV the minimum of the original value
 * \param scaleV the scaler used to downscale the original value to [0-1]
 * \param thresholdV a threshold in original space (you need to downscale it to [0-1] if you want to use it to scaled values.
 * \param alpha the alpha blending value
 * \param colormap the colormap index to use
 */
vec4 colormap( in vec3 value, float minV, float scaleV, float thresholdV, float alpha, int colormap, bool active )
{
    // descale value
    vec3 valueDescaled = vec3( minV ) + ( value * scaleV );

    // this is the final color returned by the colormapping algorithm. This is the correct value for the gray colormap
    vec4 cmapped = grayscale( value, valueDescaled, minV, scaleV );

    // negative to positive shading in red-blue
    if ( colormap == 3 )
    {
        cmapped = negative2positive( valueDescaled.r, minV, scaleV );
    }
    else if ( colormap == 6 )
    {
        cmapped = vector( valueDescaled, minV, scaleV );
    }

    // build final color
    return vec4( cmapped.rgb, cmapped.a *           // did the colormap use a alpha value?
                              alpha *               // did the user specified an alpha?
                              clipThreshold( valueDescaled, colormap, thresholdV ) * // clip due to threshold?
                              float( active ) );    // is it active?
}

#endif // WGECOLORMAPS_GLSL

