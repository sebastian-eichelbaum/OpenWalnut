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

#ifndef WTENSORMETA_H
#define WTENSORMETA_H

#include <vector> // for size_t

#include <boost/mpl/bool.hpp>

// forward declaration
template< typename Data_T, std::size_t k, std::size_t n, std::size_t N, std::size_t m >
struct WRecursiveTensorEvaluation;

/**
 * Multiplies gradient components and divides by multiplicities.
 *
 * This essentailly calls WRecursiveTensorEvaluation< ... >s evaluate function N times.
 *
 * If IterEnd equals boost::mpl::bool_< false >, iteration is aborted.
 */
template< typename IterEnd, typename Data_T, std::size_t k, std::size_t i, std::size_t N >
struct WRecursiveTensorEvaluationLoop
{
    /**
     * Multiply gradient components and divide by multiplicities.
     *
     * \param tens A pointer to the tensor components.
     * \param grad The gradient to evaluate the function at.
     * \param w The result up to now.
     *
     * \return The result.
     */
    static inline Data_T evaluate( Data_T const*& tens, Data_T const* grad, Data_T w )
    {
        return WRecursiveTensorEvaluation< Data_T, k - 1, i, N, 1 >::evaluate( tens, grad, w * grad[ i ] )
               + WRecursiveTensorEvaluationLoop< boost::mpl::bool_< ( i + 1 < N ) >, Data_T, k, i + 1, N >::evaluate( tens, grad, w );
    }
};

/**
 * Specialization for boost::mpl::bool_< false >, aborts iteration.
 */
template< typename Data_T, std::size_t k, std::size_t i, std::size_t N >
struct WRecursiveTensorEvaluationLoop< boost::mpl::bool_< false >, Data_T, k, i, N >
{
    /**
     * Does nothing.
     *
     * \return 0
     */
    static inline Data_T evaluate( Data_T const*&, Data_T const*, Data_T )
    {
        return 0.0;
    }
};

/**
 * Recursive evaluation of a spherical function for a gradient.
 */
template< typename Data_T, std::size_t k, std::size_t n, std::size_t N, std::size_t m >
struct WRecursiveTensorEvaluation
{
    /**
     * Multiply gradient components and divide by multiplicities.
     *
     * \param tens A pointer to the tensor components.
     * \param grad The gradient to evaluate the function at.
     * \param w The result up to now.
     *
     * \return The result.
     */
    static inline Data_T evaluate( Data_T const*& tens, Data_T const* grad, Data_T w )
    {
        Data_T ret = WRecursiveTensorEvaluation< Data_T, k - 1, n, N, m + 1 >::evaluate( tens, grad, w * grad[ n ] / ( m + 1 ) );
        return ret + WRecursiveTensorEvaluationLoop< boost::mpl::bool_< ( n + 1 < N ) >, Data_T, k, n + 1, N >::evaluate( tens, grad, w );
    }
};

/**
 * Recursive evaluation of a spherical function for a gradient.
 */
template< typename Data_T, std::size_t n, std::size_t N, std::size_t m >
struct WRecursiveTensorEvaluation< Data_T, 0, n, N, m >
{
    /**
     * Multiply the accumulated weight by the tensor component.
     *
     * \param tens A pointer to the tensor components.
     * \param w The result up to now.
     *
     * \return The result.
     */
    static inline Data_T evaluate( Data_T const*& tens, Data_T const*, Data_T w )
    {
        return w * *( tens++ );
    }
};

#endif  // WTENSORMETA_H
