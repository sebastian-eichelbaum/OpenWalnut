//---------------------------------------------------------------------------
//
// Project: OpenWalnut
//
// Copyright 2009 SomeCopyrightowner
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
 * the "<<" operator instead of string_utils::operator(cout, myVector).
 *
 * The reason for not using the Boost trimming functions is, that Boost
 * providing just Whitespace trimming depending on the current locale, but we
 * might want to trim other character sets too.
 *
 * The reason for not using the Boost case switching functions is that we want
 * those functions to return a std::string copy which is modified to make some
 * call chains ala: foo( rTrim( toLower( str ), "bar" ) );
 *
 * The reason for not using Boosts Tokenizer is, that this tokenizer, is much
 * most simplest to use :).
 */
namespace string_utils {
    /**
     * We consider the following characters as whitespace:
     * \r  carriage return
     * \n  newline
     * \t  tab
     * ' ' space
     */
    extern const std::string WHITESPACE;

    /**
     * Trims any character in \param t from the end (or right side) of the
     * string. A modified copy of the original is returned.
     */
    std::string rTrim(const std::string &source, const std::string& t = WHITESPACE );

    /**
     * Trims any character in \param t from the beginnig (or leftt side) of the
     * string. A modified copy of the original is returned.
     */
    std::string lTrim( const std::string& source, const std::string& t = WHITESPACE );

    /**
     * Trims any character in \param t from both sides (beginnig and end) of
     * the string. A modified copy of the original is returned.
     */
    std::string trim(const std::string& source, const std::string& t = WHITESPACE );

    /**
     * Transforms all characters into upper case characters. A modified copy of
     * the original is returned.
     */
    std::string toUpper(const std::string& source);

    /**
     * Transforms all characters into lower case characters. A modified copy of
     * the original is returned.
     */
    std::string toLower(const std::string& source);

    /**
     * Split a string into a vector of strings (so called tokens). If compress
     * parameter is true, charactes matching between two tokens are collapsed.
     * The t parameter defines a character set of characters which are
     * considered as whitespace between the tokens. The orignial string remains
     * intact.
     */
    std::vector< std::string > tokenize( const std::string& source,
                                         bool compress = true,
                                         const std::string& t = WHITESPACE );
}

/**
 * Writes every vector to an output stream such as cout, if its elements
 * have an output operator defined.
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
