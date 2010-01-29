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

#ifndef WROI_H
#define WROI_H

#include <string>

#include <osg/Geode>

#include "../common/WColor.h"

class WPickHandler;

/**
 * Superclass for different ROI (region of interest) types.
 */
class WROI
{
public:
    /**
     * Need virtual destructor because of virtual function.
     */
    virtual ~WROI();
protected:
    osg::ref_ptr< WPickHandler > m_pickHandler; //!< A pointer to the pick handler used to get gui events for moving the box.
    osg::Geode* m_geode; //!< The graphical representation of the ROI.

private:
    bool m_isNot; //!< Indivated whether the region of interest is inside the WROI (false) oroutside (true).
    bool m_isModified; //!< Indicates whether a changed ROI has already taken effect. Means: if true, still some updates needed.
    WColor m_color; //!< The selected onject (Fibers, region on surface, ...) will have this color if m_useColor.
    bool m_useColor; //!< Indicated whether m_color should be used for display.

    /**
     * updates the graphics. Remember that this shoudl only be called from a node callback
     */
    virtual void updateGFX() = 0;
};

#endif  // WROI_H
