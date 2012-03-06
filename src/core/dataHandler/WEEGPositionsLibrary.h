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

#ifndef WEEGPOSITIONSLIBRARY_H
#define WEEGPOSITIONSLIBRARY_H

#include <map>
#include <string>

#include "../common/exceptions/WOutOfBounds.h"
#include "../common/math/linearAlgebra/WLinearAlgebra.h"



/**
 * Class which contains the positions of EEG electrodes by label.
 * \ingroup dataHandler
 */
class WEEGPositionsLibrary // NOLINT
{
public:
    /**
     * Constructor
     *
     * \param positions mapping from labels to positions
     */
    explicit WEEGPositionsLibrary( const std::map< std::string, WPosition >& positions );

    /**
     * Get the position of an electrode with the given label
     *
     * \param label label of the electrode
     * \return position of the electrode
     */
    WPosition getPosition( std::string label ) const throw( WOutOfBounds );

protected:
private:
    std::map< std::string, WPosition > m_positions; //!< mapping from labels to positions
};

#endif  // WEEGPOSITIONSLIBRARY_H
