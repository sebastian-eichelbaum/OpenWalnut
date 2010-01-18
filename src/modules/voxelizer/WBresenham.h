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

#ifndef WBRESENHAM_H
#define WBRESENHAM_H

#include <boost/shared_ptr.hpp>

#include "../../dataHandler/WGridRegular3D.h"
#include "../../math/WLine.h"
#include "WRasterAlgorithm.h"

/**
 * Implementes basic bresenham algorithm for rasterization.
 */
class WBresenham : public WRasterAlgorithm
{
public:
    /**
     * Initializes new raster algo.
     */
    explicit WBresenham( boost::shared_ptr< WGridRegular3D > grid );

    /**
     * Finishes this raster algo.
     */
    virtual ~WBresenham();

    /**
     * Rasterize the given line into the grid of dataset.
     * The value of the voxel which will be hit changes its value.
     *
     *\param line Polyline which is about to be rastered.
     */
    virtual void raster( const wmath::WLine& line );

protected:
    /**
     *
     *
     *\param start
     *\param stop
     */
    void rasterSegment( const wmath::WValue< int >& start, const wmath::WValue< int >& stop );

private:
};

#endif  // WBRESENHAM_H
