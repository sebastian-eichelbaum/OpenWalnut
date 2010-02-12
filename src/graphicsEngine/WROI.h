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

#include <boost/signals2/signal.hpp>
#include <boost/signals2/connection.hpp>

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

    /**
     * getter for the boost signal object that indicates a modified box
     *
     * \return signal object
     */
    boost::signals2::signal0< void >* getSignalIsModified();

    /**
     * sets the NOT flag
     *
     * \param isNot
     */
    void setNot( bool isNot = true );

    /**
     * getter for NOT flag
     *
     * \return the flag
     */
    bool isNot();

protected:
    osg::ref_ptr< WPickHandler > m_pickHandler; //!< A pointer to the pick handler used to get gui events for moving the box.
    osg::ref_ptr< osg::Geode> m_geode; //!< The graphical representation of the ROI.
    bool m_isModified; //!< Indicates whether a changed ROI has already taken effect. Means: if true, still some updates needed.

    /**
     * boost signal object to indicate box manipulation
     */
    boost::signals2::signal0< void >m_signalIsModified;

    bool m_isNot; //!< Indivated whether the region of interest is inside the WROI (false) oroutside (true).

    WColor m_color; //!< The selected onject (Fibers, region on surface, ...) will have this color if m_useColor.

    bool m_useColor; //!< Indicated whether m_color should be used for display.

private:
    /**
     *  updates the graphics
     */
    virtual void updateGFX() = 0;
};

#endif  // WROI_H
