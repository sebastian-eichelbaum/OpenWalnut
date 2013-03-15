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

#ifndef WGRID_H
#define WGRID_H

#include <cstddef>

#include <boost/shared_ptr.hpp>

#include "../common/WBoundingBox.h"


// forward declarations
class WPropertyGroup;

/**
 * Base class to all grid types, e.g. a regular grid.
 * \ingroup dataHandler
 */
class WGrid // NOLINT
{
public:
    /**
     * Constructs a new WGrid instance.
     * \param size number of positions in grid
     */
    explicit WGrid( size_t size );

    /**
     * Since WGrid is a base class and thus should be polymorphic we add
     * virtual destructor.
     */
    virtual ~WGrid();

    /**
     * The number of positions in this grid.
     *
     * \return \copybrief WGrid::size()
     */
    size_t size() const;

    /**
     * Axis aligned Bounding Box that encloses this grid.
     *
     * \return \copybrief WGrid::getBoundingBox()
     */
    virtual WBoundingBox getBoundingBox() const = 0;

    /**
     * Returns a pointer to the information properties object of the grid. The grid intends these properties to not be modified.
     *
     * \return the properties.
     */
    boost::shared_ptr< WPropertyGroup > getInformationProperties() const;

protected:
    /**
     * The property object for the grid containing only props whose purpose is "PV_PURPOSE_INFORMNATION". It is useful to define some property
     * to only be of informational nature. The GUI does not modify them.
     */
    boost::shared_ptr< WPropertyGroup > m_infoProperties;

private:
    /**
     * Stores the number of positions.
     */
    size_t m_size;
};

#endif  // WGRID_H
