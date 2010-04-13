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

#ifndef WBINOM_H
#define WBINOM_H

#include <string>

namespace wmath
{
/**
 * Implements compile-time calculation of binomial coefficients.
 *
 *
 * WBinom< n, k >::value = n! / ( k!(n-k)! ).
 *
 * \note For k > n, compilation fails.
 */
template< std::size_t n, std::size_t k >
struct WBinom
{
    enum
    {
        value = WBinom< n - 1, k - 1 >::value + WBinom< n - 1, k >::value
    };
};

/**
 * Specialization for n = k.
 */
template< std::size_t n >
struct WBinom< n, n >
{
    enum
    {
        value = 1
    };
};

/**
 * Specialization for k = 0.
 */
template< std::size_t n >
struct WBinom< n, 0 >
{
    enum
    {
        value = 1
    };
};

/**
 * This specialization of the WBinom struct is needed to avoid
 * infinite recursion in case of k > n. The compiler should abort
 * compilation with an error message.
 */
template< std::size_t k >
struct WBinom< 0, k >
{
};

} // namespace wmath

#endif  // WBINOM_H
