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

#ifndef WSTRINGUTILS_H
#define WSTRINGUTILS_H

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

/**
 * Some utilities for string manipulation and output operations. Please note
 * that the overloaded ostream output operators aren't in a separate namespace
 * but the string manipulation functions. This is because of short use of e.g.
 * the <tt><<</tt> operator instead of <tt>string_utils::operator(cout,
 * myVector)</tt>.
 *
 * The reason for not using the Boost trimming functions is, that Boost
 * providing just Whitespace trimming depending on the current locale, but we
 * might want to trim other character sets too.
 *
 * The reason for not using the Boost case switching functions is that we want
 * those functions to return a <tt>std::string</tt> copy which is modified to
 * make some call chains ala: <tt>foo( rTrim( toLower( str ), "bar" ) );</tt>.
 * 
 * The reason for not using Boosts Tokenizer is, that this tokenizer, is much
 * most simplest to use :).
 */
namespace string_utils {
    /** We consider the following characters as whitespace:
     *  - <tt>\\r</tt> carriage return
     *  - <tt>\\n</tt> newline
     *  - <tt>\\t</tt> tab
     *  - <tt>' '</tt> space
     */
    extern const std::string WHITESPACE;

    /**
     * Trims any occurence of each character given in parameter t from the end
     * (or right side) of the given string.
     *
     * \param source String to trim
     * \param t String representing a set containg all trimmable characters
     * \return A copy of the trimmed string
     */

    std::string rTrim(const std::string& source, const std::string& t = WHITESPACE );

    /**
     * Trims any occurence of each character given in parameter t from the
     * start (or left side) of the given string.
     *
     * \param source String to trim
     * \param t String representing a set containg all trimmable characters
     * \return A copy of the trimmed string
     */
    std::string lTrim( const std::string& source, const std::string& t =
            WHITESPACE );

    /**
     * Trims any occurence of each character given in parameter t from both
     * ends (right and left side) of the given string.
     *
     * \param source String to trim
     * \param t String representing a set containg all trimmable characters
     * \return A copy of the trimmed string
     */
    std::string trim(const std::string& source, const std::string& t = WHITESPACE );

    /**
     * Transforms all characters in the given string into upper case
     * characters.
     *
     * \param source String to transpose.
     * \return A copy of the upper case only string
     */
    std::string toUpper(const std::string& source);

    /**
     * Transforms all characters in the given string into lower case
     * characters.
     *
     * \param source String to transpose.
     * \return A copy of the lower case only string
     */
    std::string toLower(const std::string& source);

    /**
     * Splits the given string into a vector of strings (so called tokens).
     *
     * \param source String to tokenize
     * \param compress If true, charactes matching between two tokens are
     * collapsed and handled as just one character.
     * \param t String representing a set containg all characters considered
     * as whitespace.
     * \return A vector of strings containing the tokens.
     */
    std::vector< std::string > tokenize( const std::string& source,
                                         bool compress = true,
                                         const std::string& t = WHITESPACE );
}

/**
 * Writes the given vector to an output stream such as cout, if and only if
 * its elements have an output operator defined.
 *
 * \param os The output stream where the elements are written to
 * \param v Vector containing the elements
 * \return The output stream again.
 */
template< class T > std::ostream& operator<<( std::ostream& os,
                                              const std::vector< T >& v )
{
    std::stringstream result;
    result << "[";
    // typename, since const_iterator is a so called "dependent name"
    typename std::vector< T >::const_iterator citer;
    for( citer = v.begin(); citer != v.end(); ++citer )
    {
        result << *citer << ", ";
    }
    os << string_utils::rTrim( result.str(), ", " ) << "]";
    return os;
}

#endif  // WSTRINGUTILS_H
