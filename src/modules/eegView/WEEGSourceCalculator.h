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

#ifndef WEEGSOURCECALCULATOR_H
#define WEEGSOURCECALCULATOR_H

#include <cstddef>

#include <vector>

#include <boost/shared_ptr.hpp>

#include "core/common/math/linearAlgebra/WLinearAlgebra.h"
#include "core/dataHandler/WEEG2.h"
#include "WEEGEvent.h"


/**
 * Class which calculates a source position from an EEG recording at a given
 * time position
 */
class WEEGSourceCalculator
{
public:
    /**
     * Constructor
     *
     * \param eeg pointer to the EEG dataset
     */
    explicit WEEGSourceCalculator( const boost::shared_ptr< const WEEG2 > eeg );

    /**
     * Does the calculation
     *
     * \param event an event marking a time position
     * \return the calculated position
     */
    WPosition calculate( const boost::shared_ptr< const WEEGEvent > event ) const;

protected:
private:
    /**
     * pointer to the EEG dataset
     */
    const boost::shared_ptr< const WEEG2 > m_eeg;

    /**
     * vector which saves for each channel whether there exists a position in
     * the EEG dataset or not
     */
    std::vector< bool > m_hasPosition;

    /**
     * how many channels with positions exist in the EEG dataset
     */
    std::size_t m_numPositions;
};

#endif  // WEEGSOURCECALCULATOR_H
