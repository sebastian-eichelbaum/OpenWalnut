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

#ifndef WGELAYOUTABLEITEM_H
#define WGELAYOUTABLEITEM_H

#include <osg/Vec3>

/**
 * This class builds the base for graphical items that can be layouted using WGEItemLayouter instances. The layoutable item needs to overwrite
 * the getAnchor method, telling the layout algorithm where the items anchor point is situated. Depending on the layout, the item is placed there
 * or somewhere else with a leader-line to the anchor point.
 */
class WGELayoutableItem
{
public:

    /**
     * Default constructor.
     */
    WGELayoutableItem();

    /**
     * Destructor.
     */
    virtual ~WGELayoutableItem();

    /**
     * Gets the current anchor point of the item.
     *
     * \return the anchor.
     */
    virtual osg::Vec3 getAnchor() const = 0;

protected:

private:
};

#endif  // WGELAYOUTABLEITEM_H

