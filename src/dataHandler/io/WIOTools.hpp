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

#ifndef WIOTOOLS_HPP
#define WIOTOOLS_HPP

#include <stdint.h>

#include <algorithm>
#include <cassert>

/**
 * Namespaces for several tools we may need while doing IO
 */
namespace wiotools
{
    /**
     * Checks if you are on a big endian machine or not.
     */
    inline bool isBigEndian()
    {
        union
        {
            uint32_t i;
            char c[4];
        } some = {0x01020305}; // NOLINT assigning an 32 bit unsigned integer

        return some.c[0] == 1;
    }


    /**
     * Transforms a value of type T into the opposite byte order.
     */
    template< class T > T switchByteOrder( const T value )
    {
        size_t numBytes = sizeof( T );
        T result = value;
        if( numBytes == 1 )
        {
            return result;
        }
        assert( numBytes % 2 == 0  && numBytes > 0 );
        char *s  = reinterpret_cast< char* >( &result );
        for( size_t i = 0; i < numBytes / 2; ++i )
        {
            std::swap( s[i], s[ ( numBytes - 1 ) - i ] );
        }
        return result;
    }

    /**
     * Transform a whole array of elements (of type T and size of sizeof(T))
     * into opposite byte order.
     */
    template< class T > void switchByteOrderOfArray( T *array, const size_t arraySize )
    {
        for( size_t i = 0; i < arraySize; ++i )
        {
            array[i] = switchByteOrder< T >( array[i] );
        }
    }
}  // end of namespace

#endif  // WIOTOOLS_HPP
