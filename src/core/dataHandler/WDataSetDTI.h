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

#ifndef WDATASETDTI_H
#define WDATASETDTI_H

#include <boost/shared_ptr.hpp>

#include "../common/math/WTensorSym.h"
#include "WDataSetSingle.h"


/**
 * Represents a Diffusion-Tensor-Image dataset. Diffusion tensors are symmetric matrices.
 */
class WDataSetDTI : public WDataSetSingle
{
public:
    /**
     * Creates a new DTI dataset out of a value set and a grid.
     *
     * \param newValueSet Valueset having vectors of dimension 6.
     * \param newGrid
     */
    WDataSetDTI( boost::shared_ptr< WValueSetBase > newValueSet, boost::shared_ptr< WGrid > newGrid );

    /**
     * Destructs this dataset.
     */
    ~WDataSetDTI();

    /**
     * Creates a copy (clone) of this instance but allows one to change the valueset. Unlike copy construction, this is a very useful function if you
     * want to keep the dynamic type of your dataset even if you just have a WDataSetSingle.
     *
     * \param newValueSet the new valueset.
     *
     * \return the clone
     */
    virtual WDataSetSingle::SPtr clone( boost::shared_ptr< WValueSetBase > newValueSet ) const;

    /**
     * Creates a copy (clone) of this instance but allows one to change the grid. Unlike copy construction, this is a very useful function if you
     * want to keep the dynamic type of your dataset even if you just have a WDataSetSingle.
     *
     * \param newGrid the new grid.
     *
     * \return the clone
     */
    virtual WDataSetSingle::SPtr clone( boost::shared_ptr< WGrid > newGrid ) const;

    /**
     * Creates a copy (clone) of this instance. Unlike copy construction, this is a very useful function if you
     * want to keep the dynamic type of your dataset even if you just have a WDataSetSingle.
     *
     * \return the clone
     */
    virtual WDataSetSingle::SPtr clone() const;

    /**
     * Retrieves the i'th tensor.
     *
     * \warning Here is dynamical allocation used inside, this may be a problem when used with multithreading.
     *
     * \param index The position of the tensor to retrieve
     *
     * \return The new constructed symmetrical matrix as tensor.
     */
    WTensorSym< 2, 3, float > getTensor( size_t index ) const;

protected:
    /**
     * The prototype as singleton.
     */
    static boost::shared_ptr< WPrototyped > m_prototype;

private:
};

#endif  // WDATASETDTI_H
