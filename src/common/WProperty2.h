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

#ifndef WPROPERTY2_H
#define WPROPERTY2_H

#include <string>

#include "WPropertyTypes.h"

/**
 * Class managing a single named property.
 */
class WProperty2
{
public:

    /**
     * Create an empty instance just containing a name.
     *
     * \param name  the property name
     * \param description the property description
     */
    WProperty2( std::string name, std::string description );

    /**
     * Destructor.
     */
    virtual ~WProperty2();

    /**
     * Get the name of the property.
     *
     * \return the name.
     */
    std::string getName() const;

    /**
     * Get the description of the property.
     *
     * \return the description.
     */
    std::string getDescription() const;

    /**
     * Returns the actual type of the property. This is needed to identify all subclassed types.
     *
     * \return the type.
     */
    virtual PROPERTY_TYPE getType() const = 0;

protected:

    /**
     * Name of the property.
     */
    std::string m_name;

    /**
     * Description of the property.
     */
    std::string m_description;

private:
};

#endif  // WPROPERTY2_H

