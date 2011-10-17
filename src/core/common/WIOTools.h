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

#ifndef WIOTOOLS_H
#define WIOTOOLS_H

#include <stdint.h>

#include <algorithm>
#include <string>

// Use filesystem version 2 for compatibility with newer boost versions.
#ifndef BOOST_FILESYSTEM_VERSION
    #define BOOST_FILESYSTEM_VERSION 2
#endif
#include <boost/filesystem.hpp>

#include "WExportCommon.h"
#include "WAssert.h"

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
    WAssert( numBytes % 2 == 0  && numBytes > 0, "odd number of bytes whilte switching byte order" );
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
 * \param name Path to be checked on existence
 */
inline bool fileExists( const std::string& name )
{
    return boost::filesystem::exists( boost::filesystem::path( name ) );
}

/**
 * Generate a file name with full path for a temp file.
 * \return The file name.
 */
boost::filesystem::path tempFileName();

/**
 * Get the contens of a file as a string.
 *
 * \param path Filename of the file to read.
 *
 * \throw WFileNotFound If file cannot be opened for reading
 *
 * \note The string is copied, which may result in performance issues when files are getting big.
 *
 * \return The file content in as string.
 */
std::string OWCOMMON_EXPORT readFileIntoString( const boost::filesystem::path& path );

/**
 * Get the contens of a file as a string.
 *
 * \param name Filename of the file to read.
 *
 * \throw WFileNotFound If file cannot be opened for reading
 *
 * \note The string is copied, which may result in performance issues when files are getting big.
 *
 * \return The file content in as string.
 */
std::string OWCOMMON_EXPORT readFileIntoString( const std::string& name );

/**
 * Writes the contens of a string to the given path.
 *
 * \param path The path of the file where all is written to
 * \param content Payload written into that file
 *
 * \throw WFileOpenFailed If file cannot be opened for writing
 */
void OWCOMMON_EXPORT writeStringIntoFile( const boost::filesystem::path& path, const std::string& content );

/**
 * Writes the contens of a string to the given path.
 *
 * \param name The path of the file where all is written to
 * \param content Payload written into that file
 *
 * \throw WFileOpenFailed If file cannot be opened for writing
 */
void OWCOMMON_EXPORT writeStringIntoFile( const std::string& name, const std::string& content );

#endif  // WIOTOOLS_H
