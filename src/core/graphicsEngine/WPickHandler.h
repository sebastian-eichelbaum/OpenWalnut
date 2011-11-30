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

#ifndef WPICKHANDLER_H
#define WPICKHANDLER_H

//#include <sstream>
#include <string>

#include <boost/signals2/signal.hpp>

//#include <osgUtil/Optimizer>
//#include <osgDB/ReadFile>
#include <osgViewer/View>
//#include <osgViewer/CompositeViewer>

//#include <osg/Material>
//#include <osg/Geode>
//#include <osg/BlendFunc>
//#include <osg/Depth>
//#include <osg/Projection>
//#include <osg/MatrixTransform>
//#include <osg/Camera>
//#include <osg/io_utils>
//#include <osg/ShapeDrawable>
//#include <osgText/Text>

#include "WPickInfo.h"
#include "WExportWGE.h"

/**
 * Class to handle events with a pick.
 *
 * The handler ignores any geometry whose name starts with an underscore ("_").
 */
class WGE_EXPORT WPickHandler: public osgGA::GUIEventHandler
{
public:

    /**
     * Constructor that initalizes members with sensible defaults.
     */
    WPickHandler();

    /**
     * Constructor that initalizes members with sensible defaults and sets the name of the viewer
     *
     * \param viewerName name of the viewer
     */
    explicit WPickHandler( std::string viewerName );

    /**
     * Deals with the events found by the osg.
     * \param ea Event class for storing Keyboard, mouse and window events.
     * \param aa Interface by which GUIEventHandlers may request actions of the GUI system
     *
     * \return true if the event was handled.
     */
    bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa );

    /**
     * Send a pick signal with the pick information as string
     * \param view the view in which we pick.
     * \param ea Event class for storing Keyboard, mouse and window events.
     */
    virtual void pick( osgViewer::View* view, const osgGA::GUIEventAdapter& ea );

    /**
     * Send a pick signal with the string "unpick"
     */
    virtual void unpick();

    /**
     * Gives information about the picked object.
     *
     * \return info object for this hit
     */
    WPickInfo getHitResult();

    /**
     * returns the m_pickSignal to for registering to it.
     */
    boost::signals2::signal1< void, WPickInfo >* getPickSignal();

    /**
     * setter for paint mode
     * \param mode the paint mode
     */
    void setPaintMode( int mode );

protected:
    /**
     * Virtual destructor needed because of virtual function.
     *
     * This desctructor is protected to avoid accidentally deleting
     * a instance of WPickHandler.
     * This follows the philosophy of OSG to avoid problems in multithreaded
     * environments, since these pointers are used deep in the OSG where
     * a deletion could cause a segfault.
     */
    virtual ~WPickHandler();

    WPickInfo m_hitResult; //!< Textual representation of the result of a pick.
    WPickInfo m_startPick; //!< indicates what was first picked. Should be "" after unpick.
    bool m_shift; //!< is shift pressed?
    bool m_ctrl; //!< is ctrl pressed?
    std::string m_viewerName; //!< which viewer sends the signal
    int m_paintMode; //!< the paint mode
    WPickInfo::WMouseButton m_mouseButton; //!< stores mouse button that initiated the pick


private:
    /**
     * Sets the current modifiers to the provided pickInfo
     *
     * \param pickInfo This pickInfo will be updated.
     */
    void updatePickInfoModifierKeys( WPickInfo* pickInfo );

    boost::signals2::signal1< void, WPickInfo > m_pickSignal; //!< One can register to this signal to receive pick events.
};

#endif  // WPICKHANDLER_H
