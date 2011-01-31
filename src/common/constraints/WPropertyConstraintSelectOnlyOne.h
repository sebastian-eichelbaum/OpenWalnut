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

#ifndef WPROPERTYCONSTRAINTSELECTONLYONE_H
#define WPROPERTYCONSTRAINTSELECTONLYONE_H

#include "../WPropertyTypes.h"
#include "WPropertyConstraintTypes.h"

/**
 * This class allows constraining selection properties to not allow selection of multiple items.
 */
template < typename T >
class WPropertyConstraintSelectOnlyOne: public WPropertyVariable< T >::PropertyConstraint
{
public:
    /**
     * Constructor.
     */
    explicit WPropertyConstraintSelectOnlyOne();

    /**
     * Destructor.
     */
    virtual ~WPropertyConstraintSelectOnlyOne();

    /**
     * Checks whether the specified value is a directory or not.
     *
     * \param property the property whose new value should be set.
     * \param value the new value to check
     *
     * \return true if the file/path is a directory
     */
    virtual bool accept( boost::shared_ptr< WPropertyVariable< T > > property, T value );

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
};

template < typename T >
WPropertyConstraintSelectOnlyOne< T >::WPropertyConstraintSelectOnlyOne()
{
}

template < typename T >
WPropertyConstraintSelectOnlyOne< T >::~WPropertyConstraintSelectOnlyOne()
{
}

template < typename T >
bool WPropertyConstraintSelectOnlyOne< T >::accept( boost::shared_ptr< WPropertyVariable< T > > /* property */, T value )
{
    return ( value.size() <= 1 );
}

template < typename T >
PROPERTYCONSTRAINT_TYPE WPropertyConstraintSelectOnlyOne< T >::getType()
{
    return PC_SELECTONLYONE;
}

template < typename T >
boost::shared_ptr< typename WPropertyVariable< T >::PropertyConstraint > WPropertyConstraintSelectOnlyOne< T >::clone()
{
    return boost::shared_ptr< typename WPropertyVariable< T >::PropertyConstraint >( new WPropertyConstraintSelectOnlyOne< T >( *this ) );
}

#endif  // WPROPERTYCONSTRAINTSELECTONLYONE_H

