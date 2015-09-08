//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2015 OpenWalnut Community, Nemtics, BSV@Uni-Leipzig
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

#ifndef WDATASETDTITOSCALAR_I_H
#define WDATASETDTITOSCALAR_I_H

#include "core/common/math/linearAlgebra/WVectorFixed.h"
#include "core/common/math/WTensorSym.h"
#include "core/common/WFlag.h"
#include "core/common/WProgress.h"
#include "core/dataHandler/WDataSetDTI.h"
#include "core/dataHandler/WDataSetScalar.h"
#include "core/dataHandler/WDataSetVector.h"

/**
 * Interface to compute various measures on tensors. Prominent examples are the
 * Fractional Anisotropy (FA), trace, Relative Anisotropy, Mean Diffusivity, Radial Diffusivity, etc.
 * As many scalar measures may use either the tensor components directly or rely on derived
 * measures such as eigenvalues, both are needed for generating the scalar.
 */
class WDataSetDTIToScalar_I
{
public:
    /**
     * This runs the given strategy on the given dataset.
     *
     * \param progress the progress instance you should increment each time you fill the value for one voxel.
     * \param shutdown Possibility to abort in case of shutdown.
     * \param tensors The tensor components
     * \param evals The Eigenvalues
     *
     * \return The scalar dataset, for which each tensor a scalar value is derived according to the subclass implementation.
     */
    virtual WDataSetScalar::SPtr operator()( WProgress::SPtr progress, WBoolFlag const &shutdown,
        WDataSetDTI::SPtr tensors, WDataSetVector::SPtr evals );

    /**
     * Destructor
     */
    virtual ~WDataSetDTIToScalar_I();

protected:
    /**
     * Actual scalar computation.
     *
     * \param evals Eigenvalues
     * \param tensor Tensor components
     *
     * \return Scalar value derived either from tensor components, eigenvalues or both.
     */
    virtual double tensorToScalar( const WVector3d& evals, const WTensorSym< 2, 3, float >& tensor ) = 0;
};

#endif  // WDATASETDTITOSCALAR_I_H
