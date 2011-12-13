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

#ifndef WPROPERTYCONSTRAINTMIN_H
#define WPROPERTYCONSTRAINTMIN_H

#include "../WPropertyTypes.h"
#include "WPropertyConstraintTypes.h"

/**
 * This class allows constraining properties using a minimum value and the corresponding >= operator.
 */
template< typename T >
class WPropertyConstraintMin: public WPropertyVariable< T >::PropertyConstraint
{
public:
    /**
     * Constructor.
     *
     * \param min the minimum value which the new property value should have.
     */
    explicit WPropertyConstraintMin( T min );

    /**
     * Destructor.
     */
    virtual ~WPropertyConstraintMin();

    /**
     * Checks whether the specified new value is larger or equal to the specified min value.
     *
     * \param property the property whose new value should be set.
     * \param value the new value to check
     *
     * \return true if value >= m_min
     */
    virtual bool accept( boost::shared_ptr< WPropertyVariable< T > > property, const T& value );

    /**
     * Returns the current min value.
     *
     * \return the min value.
     */
    T getMin();

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
     * The minimal value the property should have
     */
    T m_min;
};

template < typename T >
WPropertyConstraintMin< T >::WPropertyConstraintMin( T min ):
    m_min( min )
{
}

template < typename T >
WPropertyConstraintMin< T >::~WPropertyConstraintMin()
{
}

template < typename T >
bool WPropertyConstraintMin< T >::accept( boost::shared_ptr< WPropertyVariable< T > > /* property */, const T& value )
{
    return value >= m_min;
}

template < typename T >
T WPropertyConstraintMin< T >::getMin()
{
    return m_min;
}

template < typename T >
PROPERTYCONSTRAINT_TYPE WPropertyConstraintMin< T >::getType()
{
    return PC_MIN;
}

template < typename T >
boost::shared_ptr< typename WPropertyVariable< T >::PropertyConstraint > WPropertyConstraintMin< T >::clone()
{
    return boost::shared_ptr< typename WPropertyVariable< T >::PropertyConstraint >( new WPropertyConstraintMin< T >( *this ) );
}

#endif  // WPROPERTYCONSTRAINTMIN_H

