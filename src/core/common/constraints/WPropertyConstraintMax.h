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

#ifndef WPROPERTYCONSTRAINTMAX_H
#define WPROPERTYCONSTRAINTMAX_H

#include "../WPropertyTypes.h"
#include "WPropertyConstraintTypes.h"

/**
 * This class allows constraining properties using a maximum value and the corresponding <= operator.
 */
template< typename T >
class WPropertyConstraintMax: public WPropertyVariable< T >::PropertyConstraint
{
public:
    /**
     * Constructor.
     *
     * \param max the maximum value which the new property value should have.
     */
    explicit WPropertyConstraintMax( T max );

    /**
     * Destructor.
     */
    virtual ~WPropertyConstraintMax();

    /**
     * Checks whether the specified new value is smaller or equal to the specified max value.
     *
     * \param property the property whose new value should be set.
     * \param value the new value to check
     *
     * \return true if value <= m_max
     */
    virtual bool accept( boost::shared_ptr< WPropertyVariable< T > > property, const T& value );

    /**
     * Returns the current max value.
     *
     * \return the max value.
     */
    T getMax();

    /**
     * Allows simple identification of the real constraint type.
     *
     * \return the type
     */
    virtual PROPERTYCONSTRAINT_TYPE getType();

    /**
     * Method to clone the constraint and create a new one with the correct dynamic type.
     *
     * \return the constraint.
     */
    virtual boost::shared_ptr< typename WPropertyVariable< T >::PropertyConstraint > clone();

private:

    /**
     * The maximal value the property should have
     */
    T m_max;
};

template < typename T >
WPropertyConstraintMax< T >::WPropertyConstraintMax( T max ):
    m_max( max )
{
}

template < typename T >
WPropertyConstraintMax< T >::~WPropertyConstraintMax()
{
}

template < typename T >
bool WPropertyConstraintMax< T >::accept( boost::shared_ptr< WPropertyVariable< T > > /* property */, const T& value )
{
    return value <= m_max;
}

template < typename T >
T WPropertyConstraintMax< T >::getMax()
{
    return m_max;
}

template < typename T >
PROPERTYCONSTRAINT_TYPE WPropertyConstraintMax< T >::getType()
{
    return PC_MAX;
}

template < typename T >
boost::shared_ptr< typename WPropertyVariable< T >::PropertyConstraint > WPropertyConstraintMax< T >::clone()
{
    return boost::shared_ptr< typename WPropertyVariable< T >::PropertyConstraint >( new WPropertyConstraintMax< T >( *this ) );
}

#endif  // WPROPERTYCONSTRAINTMAX_H
