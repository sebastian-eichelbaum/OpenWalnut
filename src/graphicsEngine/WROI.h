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

#include "../common/WProperties.h"


#include "WExportWGE.h"

class WPickHandler;

/**
 * Superclass for different ROI (region of interest) types.
 */
class WGE_EXPORT WROI : public osg::Geode
{
public:
    WROI();

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

    /**
     * getter
     *
     * \return the active flag
     */
    bool active();

    /**
     * setter
     *
     * \param active
     */
    void setActive( bool active );

    /**
     * hides the roi in the scene
     */
    void hide();

    /**
     * unhides the roi in the scene
     */
    void unhide();

    /**
     * Getter for modified flag
     * \param reset if true the dirty flag will be set to false
     * \return the dirty flag
     */
    bool dirty( bool reset = false );

    /**
     * Getter
     * \return the properties object for this roi
     */
    boost::shared_ptr< WProperties > getProperties();

protected:
    /**
     * initializes the roi's properties
     */
    void properties();

    /**
     * callback when a property gets changed
     */
    void propertyChanged();

    osg::ref_ptr< WPickHandler > m_pickHandler; //!< A pointer to the pick handler used to get gui events for moving the box.

    /**
     * boost signal object to indicate box manipulation
     */
    boost::signals2::signal0< void >m_signalIsModified;

    /**
     * the property object for the module
     */
    boost::shared_ptr< WProperties > m_properties;

    WPropBool m_dirty; //!< dirty flag, indicates the bit fields need updating

    /**
     * indicates if the roi is active
     */
    WPropBool m_active;

    /**
     * indicates if the roi is negated
     */
    WPropBool m_not;

    /**
     * threshold for an arbitrary roi
     */
    WPropDouble m_threshold;

    /**
     * A color for painting the roi in the scene
     */
    WPropColor m_color;


private:
    /**
     *  updates the graphics
     */
    virtual void updateGFX() = 0;
};

#endif  // WROI_H
