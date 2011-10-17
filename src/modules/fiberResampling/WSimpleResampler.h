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

#ifndef WSIMPLERESAMPLER_H
#define WSIMPLERESAMPLER_H

#include <vector>

#include <boost/shared_ptr.hpp>

#include "core/common/datastructures/WFiber.h"

/**
 * Algorithm to resample a single tract into almost equidistant segments.
 */
class WSimpleResampler
{
friend class WSimpleResamplerTest;
public:
    /**
     * Resamples a tract.
     *
     * \param tract The tract to resample
     * \param pts The number of points the new tract should have
     *
     * \return The resampled tract
     */
    WFiber resample( const WFiber& tract, size_t pts ) const;

protected:
private:
};

#endif  // WSIMPLERESAMPLER_H
