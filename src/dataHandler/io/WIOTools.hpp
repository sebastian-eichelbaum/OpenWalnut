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

#include <cstdio>
#include <algorithm>
#include <cassert>
#include <string>

#include <boost/filesystem.hpp>

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
     *
     * \param value The value where byte swapping should be applied to
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
     *
     * \param array Array containing the data
     * \param arraySize The number of elements which is not the number of
     * bytes but e.g. the number of floats
     */
    template< class T > void switchByteOrderOfArray( T *array, const size_t arraySize )
    {
        for( size_t i = 0; i < arraySize; ++i )
        {
            array[i] = switchByteOrder< T >( array[i] );
        }
    }

    /**
     * \param name File name to get the extension or suffix from.
     * \return filename suffix
     */
    inline std::string getSuffix( std::string name )
    {
        return boost::filesystem::path( name ).extension();
    }

    /**
     * Checks if a given path already exists or not
     *
     * \param path Path to be checked on existence
     */
    inline bool fileExists( std::string path )
    {
        return boost::filesystem::exists( boost::filesystem::path( path ) );
    }

    /**
     * Generate a file name with full path for a temp file. Watch out on all
     * platforms!
     */
    inline std::string tempFileName()
    {
        // REGARDING THE COMPILER WARNING
        // 1. mkstemp is only available for POSIX systems
        // 2. reason: the warning generated here is due to a race condition
        //    while tmpnam invents the fileName it may be created by another process
        // 3. file names like "/tmp/pansen" or "C:\pansen" are platform dependent
        return std::string( std::tmpnam( NULL ) );
    }
}  // end of namespace
#endif  // WIOTOOLS_HPP
