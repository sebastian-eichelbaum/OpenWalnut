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

#ifndef WDATACREATORRANDOM_H
#define WDATACREATORRANDOM_H

#include <boost/random.hpp>

#include "core/common/WObjectNDIP.h"

#include "WMDataCreatorScalar.h"

/**
 * Creates random values inside a given grid.
 */
class WDataCreatorRandom: public WObjectNDIP< WMDataCreatorScalar::DataCreatorInterface >
{
public:
    /**
     * Default constructor.
     */
    WDataCreatorRandom();

    /**
     * Destructor.
     */
    virtual ~WDataCreatorRandom();

    /**
     * Create the dataset. This needs to be implemented by all the creators you write.
     *
     * \param grid the grid on which the value set has to be build
     * \param type the value type in the value set
     *
     * \return the value set for the given grid
     */
     virtual WValueSetBase::SPtr operator()( WGridRegular3D::ConstSPtr grid, dataType type = W_DT_FLOAT );
protected:
private:
    /**
     * Random number generator.
     */
    boost::random::mt19937 m_rand;

    /**
     * A distribution. This one creates random numbers in [0, 1)
     */
    boost::random::uniform_real_distribution<> m_values01;
};

#endif  // WDATACREATORRANDOM_H

