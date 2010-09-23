/**
 * This makes threshold lying between min and max to lie between 0 and 1
 * where 0 correspond to min and 1 to max
 */
float scaleZeroOne( in float threshold, in float min, in float max )
{
    return ( threshold - min ) / ( max - min );
}

/**
 * Evaluates a given value whether it is zero, or more exactly, if it can be seen as zero.
 * 
 * \param value the value to check
 * \param epsilon the epsilon to use. Default is 0.001. Much lower is not recommended since floating point precision in GLSL is not that high.
 * 
 * \return true if zero.
 */
bool isZero( in float value, in float epsilon = 0.001 )
{
    return ( abs( value ) <= epsilon );
}

