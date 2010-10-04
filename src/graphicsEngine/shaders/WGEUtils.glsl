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

/**
 * 
 * This makes threshold lying between min and max to lie between 0 and 1 where 0 corresponds to min and 1 to max
 * 
 * \param threshold threshold value used for cutting
 * \param min minumum of original space
 * \param max maximum of original space
 * 
 * \return 
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

