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

#ifndef WCROSSHAIR_H
#define WCROSSHAIR_H

#include <boost/shared_ptr.hpp>
#include "../common/math/linearAlgebra/WPosition.h"



/**
 * This class stores the position of the crossing navigation slices,
 * which is also represented as crosshairs in the navigation widgets.
 * It can be used for placing things. An example are the ROIBoxes which
 * are placed at the crosshairs position at the moment I am writing this.
 */
class  WCrosshair
{
public:
    /**
     * Get the position of the crosshair.
     *
     * \return the position
     */
    WPosition getPosition();

    /**
     * Set the position of the crosshair for updating it.
     *
     * \param position The new position.
     */
    void setPosition( WPosition position );

protected:
private:
    WPosition m_position; //!< Stores the current position of the crosshair.
};

#endif  // WCROSSHAIR_H
