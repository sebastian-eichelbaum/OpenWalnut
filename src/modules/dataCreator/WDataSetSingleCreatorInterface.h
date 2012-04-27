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

#ifndef WDATASETSINGLECREATORINTERFACE_H
#define WDATASETSINGLECREATORINTERFACE_H

#include <core/common/WProgress.h>
#include <core/dataHandler/WValueSetBase.h>
#include <core/dataHandler/WValueSet.h>
#include <core/dataHandler/WGridRegular3D.h>
#include <core/dataHandler/WDataHandlerEnums.h>

/**
 * Define the interface which is injected into an \ref WObjectNDIP. Remember that \ref WObjectNDIP is a template class deriving from its
 * template type. This way we can inject methods into the base class. It avoids derivation from \ref WObjectNDIP.
 *
 * This class is especially useful for all dataset types that are WDataSetSingle types. So, strategies written using this interface can be
 * utilized for vector and scalar data for example.
 */
class WDataSetSingleCreatorInterface
{
public:
    /**
     * Create the dataset. This needs to be implemented by all the creators you write. This method is designed to be applicable to all kinds of
     * WDataSetSingle that use WValueSetBase. Your implementation does not need to support all types. If you do not support any order/dimension
     * combination, throw an exception (like by using \ref WAssert).
     *
     * \param order the tensor order of the values stored in this WValueSet
     * \param dimension the tensor dimension of the values stored in this WValueSet
     * \param progress the progress instance you should increment each time you fill the value for one voxel.
     * \param grid the grid on which the value set has to be build
     * \param type the value type in the value set
     *
     * \return the value set for the given grid
     */
    virtual WValueSetBase::SPtr operator()( WProgress::SPtr progress,
                                            WGridRegular3D::ConstSPtr grid, unsigned char order = 0, unsigned char dimension = 1,
                                            dataType type = W_DT_FLOAT ) = 0;

    /**
     * Destructor
     */
    virtual ~WDataSetSingleCreatorInterface();
};

#endif  // WDATASETSINGLECREATORINTERFACE_H

