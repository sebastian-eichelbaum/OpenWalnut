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

#ifndef WPROPERTYGROUPWRAPPER_H
#define WPROPERTYGROUPWRAPPER_H

#include <string>

#include "core/common/WPropertyGroup.h"

#include "WPropertyWrapper.h"

/**
 * \class WPropertyGroupWrapper
 *
 * Encapsulates a property group.
 */
class WPropertyGroupWrapper
{
public:
    /**
     * Constructor.
     *
     * \param group The group.
     */
    explicit WPropertyGroupWrapper( boost::shared_ptr< WPropertyGroup > group );

    /**
     * Return the name of the property group.
     *
     * \return The name of the property group.
     */
    std::string getName() const;

    /**
     * Return the description of the property group.
     *
     * \return The description of the property group.
     */
    std::string getDescription() const;

    /**
     * Retrieve a property by name.
     *
     * \param name The name of the property.
     * \return The property with the given name.
     */
    WPropertyWrapper getProperty( std::string const& name );

    /**
     * Retrieve a property group by name.
     *
     * \param name The name of the group to look for.
     * \return The property group with the given name.
     */
    WPropertyGroupWrapper getGroup( std::string const& name );

private:
    //! The property group.
    boost::shared_ptr< WPropertyGroup > m_group;
};

#endif  // WPROPERTYGROUPWRAPPER_H
