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

#ifndef WSELECTIONMANAGER_H
#define WSELECTIONMANAGER_H

#include <boost/shared_ptr.hpp>

#include "WCrosshair.h"

/**
 * manages the several selection tools
 */
class WSelectionManager
{
public:
    /**
     * standard constructor
     */
    WSelectionManager();

    /**
     * destructor
     */
    virtual ~WSelectionManager();

    /**
     * getter
     */
    boost::shared_ptr< WCrosshair >getCrosshair();

    /**
     * function returns an index of the direction one is currently looking at the scene
     *
     * \return index
     */
    int getFrontSector();

protected:
private:
    boost::shared_ptr< WCrosshair >m_crosshair; //!< stores pointer to crosshair
};

#endif  // WSELECTIONMANAGER_H
