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

#ifndef WPROPERTYVARIABLE_H
#define WPROPERTYVARIABLE_H

#include <stdint.h>

#include <string>
#include <typeinfo>

#include "WProperty2.h"

/**
 * A named property class with a concrete type.
 */
template< typename T >
class WPropertyVariable: public WProperty2
{
public:

    /**
     * Create an empty instance just containing a name.
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     */
    WPropertyVariable( std::string name, std::string description, const T& initial ):
        WProperty2( name, description )
    {
        // initialize everything
    }

    /**
     * Destructor.
     */
    virtual ~WPropertyVariable()
    {
        // clean up
    }

    T& get() const
    {
        return m_value;
    }

    void set( T& val )
    {
        m_value = val;
    }

    virtual PROPERTY_TYPE getType() const
    {
        if ( typeid( T ) == typeid( int ) )
            return INT;
        else
            return INT;
    }

protected:

    /**
     * The actual variable handled by this property.
     */
    T m_value;

private:
};

#endif  // WPROPERTYVARIABLE_H
