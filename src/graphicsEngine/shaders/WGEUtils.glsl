//
// This makes threshold lying between min and max to lie between 0 and 1
// where 0 correspond to min and 1 to max
//
float scaleZeroOne( in float threshold, in float min, in float max )
{
    return ( threshold - min ) / ( max - min );
}
