//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2014 OpenWalnut Community, BSV@Uni-Leipzig and CNCF@MPI-CBS
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

#ifndef WRESAMPLING_I_H
#define WRESAMPLING_I_H

#include <core/common/WProgress.h>
#include <core/common/WFlag.h>
#include <core/dataHandler/WDataSetFibers.h>

/**
 * Interface for Resampling fibers.
 */
class WResampling_I
{
public:
    /**
     * Resample each fiber within the given fiber dataset according to the given implementation of virtual resample().
     *
     * \param progress This will indicate progress.
     * \param shutdown Possibility to abort in case of shutdown.
     * \param fibers The fibers which should be resampled.
     *
     * \return Colored fibers
     */
    virtual WDataSetFibers::SPtr operator()( WProgress::SPtr progress, WBoolFlag const &shutdown, WDataSetFibers::SPtr fibers );

    /**
     * All overrided methods should resample the fiber in their specific way.
     *
     * \param fib The fiber to resample.
     *
     * \return Copy of the resampled fiber.
     */
    virtual WFiber resample( WFiber fib ) const = 0;

    /**
     * Destructor.
     */
    virtual ~WResampling_I();
protected:
private:
};

#endif  // WRESAMPLING_I_H
