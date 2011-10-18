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

#ifndef WTYPETRAITS_H
#define WTYPETRAITS_H

#include <stdint.h>

/**
 * All kinds of type traits and policies like type priorities and type combinations.
 */
namespace WTypeTraits
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Type promitions
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * Class for checking the "better" type if two integral types are known.
     *
     * \tparam T1 the first type
     * \tparam T2 the second type
     */
    template < typename T1, typename T2 >
    class TypePromotion;    // leaving this one empty is a good idea in most cases as there is no "natural order" in all c++ types.

    /**
     * Class for checking the "better" type if two integral types are known. Specialization if both types are equal
     *
     * \tparam T the types
     */
    template < typename T >
    class TypePromotion< T, T >
    {
    public:
        typedef T Result;       //!< if both types are the same, the "better" type is obvious.
    };

    // we assume macros to be evil but it helps us here!
#define CREATEPROMOTION( T1, T2, ResultType )               \
    template <>                                             \
    class TypePromotion< T1, T2 >                           \
    {                             /*NOLINT*/                \
    public:                                                 \
        typedef ResultType Result;                          \
    };                            /*NOLINT*/                \
                                                            \
    template <>                                             \
    class TypePromotion< T2, T1 >                           \
    {                             /*NOLINT*/                \
    public:                                                 \
        typedef ResultType Result;                          \
    };                            /*NOLINT*/                \

    // Create the promotions we need. Please check this list. But do not change arbitrarily if you need a different mapping. Instead, specialize
    // the template TypePromotion locally.

    // Exclusion of this macro stuff from doxygen:
    // \cond HIDDEN_SYMBOLS

    // long double is the better choice for these
    CREATEPROMOTION( long double, double,   long double )
    CREATEPROMOTION( long double, float,    long double )
    CREATEPROMOTION( long double, int64_t,  long double )
    CREATEPROMOTION( long double, int32_t,  long double )
    CREATEPROMOTION( long double, int16_t,  long double )
    CREATEPROMOTION( long double, int8_t,   long double )
    CREATEPROMOTION( long double, uint64_t, long double )
    CREATEPROMOTION( long double, uint32_t, long double )
    CREATEPROMOTION( long double, uint16_t, long double )
    CREATEPROMOTION( long double, uint8_t,  long double )

    // double is the better choice for these
    CREATEPROMOTION( double, float,    double )
    CREATEPROMOTION( double, int64_t,  double )
    CREATEPROMOTION( double, int32_t,  double )
    CREATEPROMOTION( double, int16_t,  double )
    CREATEPROMOTION( double, int8_t,   double )
    CREATEPROMOTION( double, uint64_t, double )
    CREATEPROMOTION( double, uint32_t, double )
    CREATEPROMOTION( double, uint16_t, double )
    CREATEPROMOTION( double, uint8_t,  double )

    // float is the better choice for these (?)
    CREATEPROMOTION( float, int64_t,   float )
    CREATEPROMOTION( float, int32_t,   float )
    CREATEPROMOTION( float, int16_t,   float )
    CREATEPROMOTION( float, int8_t,    float )
    CREATEPROMOTION( float, uint64_t,  float )
    CREATEPROMOTION( float, uint32_t,  float )
    CREATEPROMOTION( float, uint16_t,  float )
    CREATEPROMOTION( float, uint8_t,   float )

    // int64_t is the better choice for these (?)
    CREATEPROMOTION( int64_t, int32_t,  int64_t )
    CREATEPROMOTION( int64_t, int16_t,  int64_t )
    CREATEPROMOTION( int64_t, int8_t,   int64_t )
    CREATEPROMOTION( int64_t, uint64_t, double ) // ?
    CREATEPROMOTION( int64_t, uint32_t, int64_t )
    CREATEPROMOTION( int64_t, uint16_t, int64_t )
    CREATEPROMOTION( int64_t, uint8_t,  int64_t )

    // int32_t is the better choice for these (?)
    CREATEPROMOTION( int32_t, int16_t,  int32_t )
    CREATEPROMOTION( int32_t, int8_t,   int32_t )
    CREATEPROMOTION( int32_t, uint64_t, double ) // ?
    CREATEPROMOTION( int32_t, uint32_t, int64_t ) // ?
    CREATEPROMOTION( int32_t, uint16_t, int32_t )
    CREATEPROMOTION( int32_t, uint8_t,  int32_t )

    // int16_t is the better choice for these (?)
    CREATEPROMOTION( int16_t, int8_t,   int16_t )
    CREATEPROMOTION( int16_t, uint64_t, double ) // ?
    CREATEPROMOTION( int16_t, uint32_t, int64_t ) // ?
    CREATEPROMOTION( int16_t, uint16_t, int32_t )  // ?
    CREATEPROMOTION( int16_t, uint8_t,  int16_t )

    // int8_t is the better choice for these (?)
    CREATEPROMOTION( int8_t, uint64_t, double ) // ?
    CREATEPROMOTION( int8_t, uint32_t, int64_t ) // ?
    CREATEPROMOTION( int8_t, uint16_t, int32_t )  // ?
    CREATEPROMOTION( int8_t, uint8_t,  int16_t ) // ?

    // uint64_t is the better choice for these (?)
    CREATEPROMOTION( uint64_t, uint32_t, uint64_t )
    CREATEPROMOTION( uint64_t, uint16_t, uint64_t )
    CREATEPROMOTION( uint64_t, uint8_t,  uint64_t )

    // uint32_t is the better choice for these (?)
    CREATEPROMOTION( uint32_t, uint16_t, uint32_t )
    CREATEPROMOTION( uint32_t, uint8_t,  uint32_t )

    // uint16_t is the better choice for these (?)
    CREATEPROMOTION( uint16_t, uint8_t, uint16_t )

    // uint8_t is the better choice for these (?)
    // promoted already

    // Exclusion of this macro stuff from doxygen: end
    // \endcond
}

#endif  // WTYPETRAITS_H

