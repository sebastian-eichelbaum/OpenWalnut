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

#ifndef WGAUSSPROCESS_H
#define WGAUSSPROCESS_H

#include "../../common/datastructures/WFiber.h"
#include "../../common/math/WMatrix.h"
#include "../WDataSetDTI.h"

/**
 * Represents a basic gaussian process with its mean- and covariance function. Basically this aims
 * to implement a part of the gaussian process framework as presented by Wasserman et. al:
 * http://dx.doi.org/10.1016/j.neuroimage.2010.01.004
 */
class WGaussProcess
{
public:
    /**
     * Default constructor.
     */
    WGaussProcess();

    /**
     * Constructs a gaussian process out of a fiber with the help of underlying diffusion tensor
     * information.
     *
     * \param tract One deterministic tractogram
     * \param tensors All 2nd order diffusion tensors
     */
    WGaussProcess( const wmath::WFiber& tract, const WDataSetDTI& tensors );

    /**
     * Default destructor.
     */
    virtual ~WGaussProcess();

    /**
     * Adds up two gaussian processes with adding the mean- and covariance fucntions.
     *
     * \param other The other gaussian process
     *
     * \return The new Gaussian Process adding this two processes
     */
    WGaussProcess operator+( const WGaussProcess& other ) const;

protected:
private:
};

#endif  // WGAUSSPROCESS_H
