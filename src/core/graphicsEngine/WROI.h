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

#include <list>
#include <string>

#include <boost/signals2/signal.hpp>
#include <boost/signals2/connection.hpp>

#include <osg/Geode>

#include "../common/WProperties.h"

class WPickHandler;

/**
 * Superclass for different ROI (region of interest) types.
 */
class WROI : public osg::Geode
{
public:
    /**
     * Ref Pointer type.
     */
    typedef osg::ref_ptr< WROI > RefPtr;

    WROI();

    /**
     * Need virtual destructor because of virtual function.
     */
    virtual ~WROI();

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
     * \return the dirty flag
     */
    bool dirty();

    /**
     * sets the dirty flag
     */
    void setDirty();

    /**
     * Getter
     * \return the properties object for this roi
     */
    boost::shared_ptr< WProperties > getProperties();

    /**
     * Add a specified notifier to the list of default notifiers which get connected to each roi.
     *
     * \param notifier  the notifier function
     */
    void addROIChangeNotifier( boost::shared_ptr< boost::function< void() > > notifier );

    /**
     * Remove a specified notifier from the list of default notifiers which get connected to each roi.
     *
     * \param notifier  the notifier function
     */
    void removeROIChangeNotifier( boost::shared_ptr< boost::function< void() > > notifier );

    /**
     * Invert property.
     *
     * \return the property
     */
    WPropBool invertProperty();

    /**
     * The property for toggling ROI visibility.
     *
     * \return the property
     */
    WPropBool showProperty();

    /**
     * The active property
     *
     * \return the property.
     */
    WPropBool activeProperty();

    /**
     * The name property
     *
     * \return the property.
     */
    WPropString nameProperty();
protected:
    /**
     * initializes the roi's properties
     */
    void properties();

    /**
     * callback when a property gets changed
     */
    void propertyChanged();

    /**
     * signals a roi change to all subscribers
     */
    void signalRoiChange();


    osg::ref_ptr< WPickHandler > m_pickHandler; //!< A pointer to the pick handler used to get gui events for moving the box.

    /**
     * the property object for the module
     */
    boost::shared_ptr< WProperties > m_properties;

    /**
     * dirty flag, indicating the graphics needs updating, it is no longer used for bitfield updating
     * since these customers get the update notification via callback
     */
    WPropBool m_dirty;

    /**
     * indicates if the roi is active
     */
    WPropBool m_active;

    /**
     * indicates if the roi is visible in the scene
     */
    WPropBool m_show;

    /**
     * indicates if the roi is negated
     */
    WPropBool m_not;

    /**
     * name of the ROI.
     */
    WPropString m_name;

    /**
     * threshold for an arbitrary roi
     */
    WPropDouble m_threshold;

    /**
     * A color for painting the roi in the scene
     */
    WPropColor m_color;

    /**
     * The notifiers connected to added rois by default.
     */
    std::list< boost::shared_ptr< boost::function< void() > > > m_changeNotifiers;


    /**
     * Lock for associated notifiers set.
     */
    boost::shared_mutex m_associatedNotifiersLock;

private:
    /**
     *  updates the graphics
     */
    virtual void updateGFX() = 0;
};

#endif  // WROI_H
