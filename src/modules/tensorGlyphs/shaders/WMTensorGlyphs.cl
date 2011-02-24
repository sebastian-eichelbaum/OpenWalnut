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

#define NumOfCoeffs ( ( Order + 1 ) * ( Order + 2 ) / 2 )

//---------------------------------------------------------------------------------------------------------------------

inline int multinomial( int k2, int k3 )
{
    int k1 = Order - k2 - k3;
    int value = 1;

    for ( int i = 1; i <= k2; i++ )
    {
        value = value * ( Order + 1 - i ) / i;
    }

    for ( int i = 1; i <= k3; i++ )
    {
        value = value * ( Order + 1 - k2 - i ) / i;
    }

    return value;
}

//---------------------------------------------------------------------------------------------------------------------

kernel void scaleGlyphs( global float* tensorData, uint numOfTensors )
{
    if ( get_global_id( 0 ) >= numOfTensors )
    {
        return;
    }

    global float* tensor = tensorData + NumOfCoeffs * get_global_id( 0 );

    float scale = 0.0f;

    int i = NumOfCoeffs - 1;

    for ( int z = Order; z >= 0; z-- )
    {
        for ( int y = Order - z ; y >= 0; y-- )
        {
            float m = multinomial( y, z );

            scale += tensor[ i ] * tensor[ i ] * m;
            tensor[ i ] *= m;

            i--;
        }
    }

    if ( scale == 0.0f )
    {
        return;
    }

    scale = 0.5f * rsqrt( scale );

    for ( int i = 0; i < NumOfCoeffs; i++ )
    {
        tensor[ i ] *= scale;
    }
}

//---------------------------------------------------------------------------------------------------------------------

float evalGlyph( float4 position, global float* tensor )
{
    float x;
    float tmp;

    float value = 0.0f;

    int i = NumOfCoeffs - 1;

    for ( int z = Order; z >= 0; z-- )
    {
        value *= position.z;
        tmp = 0.0f;
        x = 1.0f;

        for ( int y = Order - z ; y >= 0; y-- )
        {
            tmp *= position.y;
            tmp += tensor[ i ] * x;
            x *= position.x;

            i--;
        }

        value += tmp;
    }

    return fabs( value );
}

//---------------------------------------------------------------------------------------------------------------------

bool glyphIsZero( global float* tensor )
{
    for ( int i = 0; i < NumOfCoeffs; i++ )
    {
        if ( tensor[ i ] != 0.0f )
        {
            return false;
        }
    }

    return true;
}

//---------------------------------------------------------------------------------------------------------------------

float4 findIntersectionPoint( float4 point, float4 direction, global float* tensor, float precision )
{
    if ( glyphIsZero( tensor ) )
    {
        return ( float4 )1.0f;
    }

    float lowerBound = 0.0f;
    float prevStep = 0.0f;

    float len = rsqrt( point.x * point.x + point.y * point.y + point.z * point.z );
    float value = evalGlyph( ( float4 )( point.xyz * len, 0.0f ), tensor );

    len = 1.0f / len;
    value = len - value;

    while ( lowerBound < 1.0f )
    {
        float stepSize = ( 0.5f * len + 0.12f ) * value;

        if ( stepSize == prevStep )
        {
            stepSize *= 0.5f;
        }

        point.xyz += stepSize * direction.xyz;
        lowerBound += stepSize;
        prevStep = -stepSize;

        len = rsqrt( point.x * point.x + point.y * point.y + point.z * point.z );
        value = evalGlyph( ( float4 )( point.x * len, point.y * len, point.z * len, 0.0f ), tensor );

        len = 1.0f / len;
        value = len - value;

        if ( value < precision )
        {
            return point;
        }
    }

    return ( float4 )1.0f;
}

//---------------------------------------------------------------------------------------------------------------------

float4 normal( float4 position, global float* tensor )
{
    float x;
    float tmp;

    float value = 0.0f;
    float value1 = 0.0f;
    float value2 = 0.0f;
    float value3 = 0.0f;

    int i = NumOfCoeffs - 1;

    for ( int z = Order; z >= 0; z-- )
    {
        value *= position.z;
        tmp = 0.0f;
        x = 1.0f;

        for ( int y = Order - z ; y >= 0; y-- )
        {
            tmp *= position.y;
            tmp += tensor[ i ] * x;
            x *= position.x;

            i--;
        }

        value += tmp;
    }

    i = NumOfCoeffs - 1;

    for ( int z = Order; z >= 0; z-- )
    {
        value1 *= position.z;
        tmp = 0.0f;
        x = 1.0f;

        for ( int y = Order - z ; y >= 0; y-- )
        {
            tmp *= position.y;

            if ( ( Order - z - y ) != 0 )
            {
                tmp += ( Order - z - y ) * tensor[ i ] * x;
                x *= position.x;
            }

            i--;
        }

        value1 += tmp;
    }

    i = NumOfCoeffs - 1;

    for ( int z = Order; z >= 0; z-- )
    {
        value2 *= position.z;
        tmp = 0.0f;
        x = 1.0f;

        for ( int y = Order - z ; y >= 1; y-- )
        {
            tmp *= position.y;
            tmp += y * tensor[ i ] * x;
            x *= position.x;

            i--;
        }

        value2 += tmp;

        i--;
    }

    i = NumOfCoeffs - 1;

    for ( int z = Order; z >= 1; z-- )
    {
        value3 *= position.z;
        tmp = 0.0f;
        x = 1.0f;

        for ( int y = Order - z ; y >= 0; y-- )
        {
            tmp *= position.y;
            tmp += tensor[ i ] * x;
            x *= position.x;

            i--;
        }

        value3 += z * tmp;
    }

    if ( value == 0.0f )
    {
        return ( float4 )( 0.0f, 0.0f, 0.0f, 0.0f );
    }

    value = 1.0f / value;

    return normalize( ( 1 + Order ) * position - ( float4 )( value1 * value, value2 * value, value3 * value, 0.0f ) );
}

//---------------------------------------------------------------------------------------------------------------------

float4 letThereBeLightByDirection( float4 position, float4 direction, global float* tensor )
{
    if ( length( position ) == 0.0f )
    {
        return ( float4 )( 0.0f, 0.0f, 0.0f, 1.0f );
    }

    position = normalize( position );

    float4 color = ( float4 )( position.x * position.x, position.y * position.y, position.z * position.z, 1.0f );

    float cosViewNormal = -dot( direction, normal( position, tensor ) );

    float cosViewReflection = fmax( 2.0f * cosViewNormal * cosViewNormal - 1.0f, 0.0f );

    cosViewReflection *= cosViewReflection;
    cosViewReflection *= cosViewReflection;
    cosViewReflection *= cosViewReflection;
    cosViewReflection *= cosViewReflection;

    return ( 0.7f * color + ( 0.3f * cosViewNormal + 0.4f * cosViewReflection ) * ( float4 )( 1.0f, 1.0f, 1.0f, 1.0f ) );
}

//---------------------------------------------------------------------------------------------------------------------

float4 letThereBeLightByMagnitude( float4 position, float4 direction, global float* tensor )
{
    float t = 2.0f * length( position );

    if ( length( position ) == 0.0f )
    {
        return ( float4 )( 0.0f, 0.0f, 0.0f, 1.0f );
    }

    position = normalize( position );

    float4 color = ( 1.0f - t ) * ( float4 )( 1.0f, 0.0f, 0.0f, 1.0f ) + t * ( float4 )( 1.0f, 1.0f, 0.0f, 1.0f );

    float cosViewNormal = -dot( direction, normal( position, tensor ) );

    float cosViewReflection = fmax( 2.0f * cosViewNormal * cosViewNormal - 1.0f, 0.0f );

    cosViewReflection *= cosViewReflection;
    cosViewReflection *= cosViewReflection;
    cosViewReflection *= cosViewReflection;
    cosViewReflection *= cosViewReflection;

    return ( 0.7f * color + ( 0.3f * cosViewNormal + 0.4f * cosViewReflection ) * ( float4 )( 1.0f, 1.0f, 1.0f, 1.0f ) );
}

//---------------------------------------------------------------------------------------------------------------------

void intersectBox( float4 lowerB, float4 upperB, float4 initialPoint, float4 direction, float4* cell, float t, float* tNew )
{
    float tLower;
    float tUpper;

    float4 tmp;

    float4 lower = ( lowerB - initialPoint ) / direction;
    float4 upper = ( upperB - initialPoint ) / direction;

    tmp = fmax( lower, upper );
    tUpper = fmin( fmin( tmp.x, tmp.y ), tmp.z );
    tmp = fmin( lower, upper );
    tLower = fmax( fmax( fmax( tmp.x, tmp.y ), tmp.z ), t );

    if ( ( tLower <= *tNew ) && ( tLower < tUpper ) )
    {
        tmp = floor( initialPoint + tLower * direction );

        if ( direction.x < 0.0f )
        {
            tmp.x = fmin( upperB.x - 1.0f, tmp.x );
        }
        else
        {
            tmp.x = fmax( lowerB.x, tmp.x );
        }

        if ( direction.y < 0.0f )
        {
            tmp.y = fmin( upperB.y - 1.0f, tmp.y );
        }
        else
        {
            tmp.y = fmax( lowerB.y, tmp.y );
        }

        if ( direction.z < 0.0f )
        {
            tmp.z = fmin( upperB.z - 1.0f, tmp.z );
        }
        else
        {
            tmp.z = fmax( lowerB.z, tmp.z );
        }

        if ( tmp.x >= lowerB.x && tmp.x < upperB.x &&
             tmp.y >= lowerB.y && tmp.y < upperB.y &&
             tmp.z >= lowerB.z && tmp.z < upperB.z )
        {
            ( *cell ).xyz = tmp.xyz;
            ( *cell ).w = tUpper;

            *tNew = tLower;
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------

bool traverse( float4 initialPoint, float4 direction, float4* cell, float4 slices )
{
    float4 tMax = fabs( ( *cell + step( 0.0f, direction ) - initialPoint ) / direction );
    float t = fmin( fmin( tMax.x, tMax.y ), tMax.z );

    if ( t < ( *cell ).w )
    {
        ( *cell ).xyz -= convert_float4( isequal( ( float4 )t, tMax ) ).xyz * sign( direction ).xyz;

        return true;
    }
    else
    {
        float tNew = INFINITY;

        intersectBox
        (
            ( float4 )( slices.x, 0.0f, 0.0f, 0.0f ),
            ( float4 )( slices.x + 1.0f, NumOfTensors.y, NumOfTensors.z, 0.0f ),
            initialPoint, direction, cell, t, &tNew
        );

        intersectBox
        (
            ( float4 )( 0.0f, slices.y, 0.0f, 0.0f ),
            ( float4 )( NumOfTensors.x, slices.y + 1.0f, NumOfTensors.z, 0.0f ),
            initialPoint, direction, cell, t, &tNew
        );

        intersectBox
        (
            ( float4 )( 0.0f, 0.0f,slices.z, 0.0f ),
            ( float4 )( NumOfTensors.x, NumOfTensors.y, slices.z + 1.0f, 0.0f ),
            initialPoint, direction, cell, t, &tNew
        );

        return ( tNew != INFINITY );
    }
}

//---------------------------------------------------------------------------------------------------------------------

global float* getTensor( global float* sliceX, global float* sliceY, global float* sliceZ, float4 cell, float4 slices )
{
    if ( cell.x == slices.x )
    {
        intptr_t offset = ( ( intptr_t )cell.y ) + ( ( intptr_t )cell.z ) * ( ( intptr_t )NumOfTensors.y );

        return ( sliceX + NumOfCoeffs * offset );
    }
    else
    {
        if ( cell.y == slices.y )
        {
            intptr_t offset = ( ( intptr_t )cell.z ) + ( ( intptr_t )cell.x ) * ( ( intptr_t )NumOfTensors.z );

            return ( sliceY + NumOfCoeffs * offset );
        }
        else
        {
            intptr_t offset = ( ( intptr_t )cell.x ) + ( ( intptr_t )cell.y ) * ( ( intptr_t )NumOfTensors.x );

            return ( sliceZ + NumOfCoeffs * offset );
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------

kernel void renderGlyphs( float4 initialPoint, float4 direction, float4 x, float4 y,
                          float near, float far, uint width, uint height, float4 slices,
                          global float* sliceX, global float* sliceY, global float* sliceZ,
                          write_only image2d_t color, write_only image2d_t depth, int mode )
{
    int2 pixel = ( int2 )( get_global_id( 0 ), get_global_id( 1 ) );

    if ( pixel.x >= width )
    {
        return;
    }

    if ( pixel.y >= height )
    {
        return;
    }

    float dirLen = rsqrt( direction.x * direction.x + direction.y * direction.y + direction.z * direction.z );
    float precision = 0.6f * length( x ) / width;

    initialPoint += ( ( float )pixel.x / ( float )width ) * x + ( ( float )pixel.y / ( float )height ) * y;
    direction *= dirLen;

    float4 cell = ( float4 )( floor( initialPoint ).xyz ,0.0f );

    while ( traverse( initialPoint, direction, &cell, slices ) )
    {
        float4 centerToInPoint = initialPoint - ( float4 )( cell.x + 0.5f, cell.y + 0.5f, cell.z + 0.5f, 0.0f );

        centerToInPoint -= ( dot( centerToInPoint, direction ) + 0.5f ) * direction;

        global float* tensor = getTensor( sliceX, sliceY, sliceZ, cell, slices );

        float4 intPoint = findIntersectionPoint( centerToInPoint, direction, tensor, precision );

        if ( intPoint.w != 1.0f )
        {
            float fragDepth = ( dot( intPoint - initialPoint, direction ) * dirLen - 1.0f ) / ( far - near ) * near;

            if ( mode == 0 )
            {
                write_imagef( color, pixel, letThereBeLightByMagnitude( intPoint, direction, tensor ) );
            }
            else
            {
                write_imagef( color, pixel, letThereBeLightByDirection( intPoint, direction, tensor ) );
            }

            write_imagef( depth, pixel, fragDepth );

            return;
        }
    }

    write_imagef( depth, pixel, 1.0f );
}

//---------------------------------------------------------------------------------------------------------------------