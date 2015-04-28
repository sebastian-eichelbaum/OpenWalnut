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

#ifndef WRESAMPLEBYSEGLENGTHKEEPSHORTFIBERS_H
#define WRESAMPLEBYSEGLENGTHKEEPSHORTFIBERS_H

#include <core/common/datastructures/WFiber.h>
#include <core/common/WObjectNDIP.h>

#include "WResampling_I.h"

/**
 * Resamples fibers by segment length.
 * Does not discard any fibers.
 */
class WResampleBySegLengthKeepShortFibers : public WObjectNDIP< WResampling_I >
{
public:
    /**
     * Constructor.
     */
    WResampleBySegLengthKeepShortFibers();

protected:
    /**
     * The given fiber is resampled by segment length and a copy is returned.
     *
     * \param fib Fiber to resample.
     *
     * \return Copy of the resampled fiber.
     */
    virtual WFiber resample( WFiber fib ) const;

    /**
     * Number of new sample points all tracts are resampled to.
     */
    WPropDouble m_segLength;
private:
};

#endif  // WRESAMPLEBYSEGLENGTHKEEPSHORTFIBERS_H
