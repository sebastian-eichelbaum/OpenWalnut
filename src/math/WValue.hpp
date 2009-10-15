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

#ifndef WVALUE_H
#define WVALUE_H

#include <cassert>
#include <cmath>
#include <vector>

#include "../common/WStringUtils.hpp"

namespace wmath
{
/**
 * Base class for all higher level values like tensors, vectors, matrices and so on.
 */
template< typename T > class WValue
{
public:
    /**
     * Create a WValue with the given number of components.
     * The components will be set to zero if T is a type representing numbers.
     */
    explicit WValue( size_t nbComponents )
        : m_components( nbComponents )
    {
    }

    /**
     * Create a WValue as copy of the one given as parameter.
     */
    WValue( const WValue& newValue )
        : m_components( newValue.m_components )
    {
    }

    /**
     * Get number of components the value consists of.
     */
    size_t size() const
    {
        return m_components.size();
    }

    /**
     * Returns a reference to the i-th component in order
     * to provide access to the component.
     */
    T& operator[]( size_t i )
    {
        assert( i <  m_components.size() );
        return m_components[i];
    }

    /**
     * Returns a const reference to the i-th component in order
     * to provide read-only access to the component.
     */
    const T& operator[]( size_t i ) const
    {
        assert( i <  m_components.size() );
        return m_components[i];
    }

    /**
     * Compares two WValues and returns true if they contain the same data.
     */
    bool operator==( const WValue& rhs ) const
    {
        return ( m_components == rhs.m_components );
    }

    /**
     * Compares two WValues and returns true if they contain the different data.
     */
    bool operator!=( const WValue& rhs ) const
    {
        return ( m_components != rhs.m_components );
    }

    /**
     * Assigns the contents of its argument to the contents of this WValue.
     */
    WValue& operator=( const WValue& rhs )
    {
        m_components = rhs.m_components;
        return *this;
    }

    /**
     * Adds a the argument componentwise to the components of this WValue
     */
    WValue& operator+=( const WValue& rhs )
    {
        assert( m_components.size() == rhs.m_components.size() );
        for( unsigned int i = 0; i < m_components.size(); ++i )
            m_components[i] += rhs.m_components[i];
        return *this;
    }

    /**
     * Subtracts the argument componentwise from the components of this WValue
     */
    WValue& operator-=( const WValue& rhs )
    {
        assert( m_components.size() == rhs.m_components.size() );
        for( unsigned int i = 0; i < m_components.size(); ++i )
            m_components[i] -= rhs.m_components[i];
        return *this;
    }

    /**
     * Scales each component of this WValue with the given scalar argument
     */
    WValue& operator*=( double rhs )
    {
        for( unsigned int i = 0; i < m_components.size(); ++i )
            m_components[i] *= rhs;
        return *this;
    }

    /**
     * Scales each component of this WValue with the coressponding
     * component of the given argument WValue
     */
    WValue& operator*=( const WValue& rhs )
    {
        assert( m_components.size() == rhs.m_components.size() );
        for( unsigned int i = 0; i < m_components.size(); ++i )
            m_components[i] *= rhs.m_components[i];
        return *this;
    }

    /**
     * Componentwise addition.
     */
    const WValue operator+( const WValue& summand2 ) const
    {
        assert( m_components.size() == summand2.m_components.size() );
        WValue result( *this );
        result += summand2;
        return result;
    }

    /**
     * Componentwise subtraction.
     */
    const WValue operator-( const WValue& subtrahend ) const
    {
        assert( m_components.size() == subtrahend.m_components.size() );
        WValue result( *this );
        result -= subtrahend;
        return result;
    }

    /**
     * Componentwise multiplication.
     */
    const WValue operator*( const WValue& factor2 ) const
    {
        assert( m_components.size() == factor2.m_components.size() );
        WValue result( *this );
        result *= factor2;
        return result;
    }

    /**
     * Square root of sum of squares of elements.
     * This function return double instead of T
     * because norm includes a square root and thus
     * its computation automatically results in a
     * floating point number.
     */
    double norm() const
    {
        double normSquare = 0.0;

        for( unsigned int i = 0; i < m_components.size(); ++i )
            normSquare += m_components[i] * m_components[i];

        return sqrt( normSquare );
    }

    /**
     * Sum of squares of elements.
     * This function returns double instead of T
     * because normSquare includes many squares and thus
     * might return large values that might not fit into
     * T's range of values. Double prevents an overflow.
     * Additionally this is consistent with norm().
     */
    double normSquare() const
    {
        double normSquare = 0.0;

        for( unsigned int i = 0; i < m_components.size(); ++i )
            normSquare += m_components[i] * m_components[i];

        return normSquare;
    }

    /**
     * Make the norm of this WValue be 1 by dividing by WValue::norm()
     */
    void normalize()
    {
        double currentNorm = norm();
        for( unsigned int i = 0; i < m_components.size(); ++i )
            m_components[i] /= currentNorm;
    }

    /**
     * Return a normalized version of the current WValue without modifying it.
     */
    WValue normalized() const
    {
        WValue result = *this;
        result.normalize();
        return result;
    }

    /**
     * Writes a meaningful representation of that object to the given stream
     */
    friend std::ostream& operator<<( std::ostream& os, const WValue< T > &rhs )
    {
        // Caution: only directly referencing the operator<< template function will work here
        return string_utils::operator<<( os, rhs.m_components );
    }

protected:
private:
    /**
     * The components the value is composed of. This contains the actual data
     */
    std::vector< T > m_components;
};

/**
 * Multiplies a WValue with a scalar
 */
template< typename T > const WValue< T > operator*( const WValue< T >& lhs, double rhs )
{
    WValue< T > result( lhs );
    result *= rhs;
    return result;
}

/**
 * This functions only exists to make scalar multiplication commutative
 */
template< typename T > const WValue< T > operator*( double lhs, const WValue< T >& rhs )
{
    WValue< T > result( rhs );
    result *= lhs;
    return result;
}
}  // End of namepsace
#endif  // WVALUE_H
