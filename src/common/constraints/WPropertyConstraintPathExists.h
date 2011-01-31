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

#ifndef WPROPERTYCONSTRAINTPATHEXISTS_H
#define WPROPERTYCONSTRAINTPATHEXISTS_H

#include "../WPropertyTypes.h"
#include "WPropertyConstraintTypes.h"

/**
 * This class allows constraining properties to be existing filenames. This is especially useful for WPropFilename.
 */
template < typename T >
class WPropertyConstraintPathExists: public WPropertyVariable< T >::PropertyConstraint
{
public:
    /**
     * Constructor.
     */
    explicit WPropertyConstraintPathExists();

    /**
     * Destructor.
     */
    virtual ~WPropertyConstraintPathExists();

    /**
     * Checks whether the specified new value is larger or equal to the specified min value.
     *
     * \param property the property whose new value should be set.
     * \param value the new value to check
     *
     * \return true if the file/path exists
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
WPropertyConstraintPathExists< T >::WPropertyConstraintPathExists()
{
}

template < typename T >
WPropertyConstraintPathExists< T >::~WPropertyConstraintPathExists()
{
}

template < typename T >
bool WPropertyConstraintPathExists< T >::accept( boost::shared_ptr< WPropertyVariable< T > > /* property */, T value )
{
    return boost::filesystem::exists( value );
}

template < typename T >
PROPERTYCONSTRAINT_TYPE WPropertyConstraintPathExists< T >::getType()
{
    return PC_PATHEXISTS;
}

template < typename T >
boost::shared_ptr< typename WPropertyVariable< T >::PropertyConstraint > WPropertyConstraintPathExists< T >::clone()
{
    return boost::shared_ptr< typename WPropertyVariable< T >::PropertyConstraint >( new WPropertyConstraintPathExists< T >( *this ) );
}

#endif  // WPROPERTYCONSTRAINTPATHEXISTS_H

