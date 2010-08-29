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
#include <utility>

#include "../common/math/WPosition.h"
#include "WExportDataHandler.h"

/**
 * Base class to all grid types, e.g. a regular grid.
 * \ingroup dataHandler
 */
class OWDATAHANDLER_EXPORT WGrid // NOLINT
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
    virtual ~WGrid()
    {
    }

    /**
     * \return The number of position in this grid.
     */
    size_t size() const;

    /**
     * Returns the two positions representing the bounding box of the grid.
     */
    virtual std::pair< wmath::WPosition, wmath::WPosition > getBoundingBox() const = 0;

protected:

private:
    /**
     * Stores the number of positions.
     */
    size_t m_size;
};

#endif  // WGRID_H
