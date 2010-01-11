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

#include <sstream>
#include <string>

#include <boost/signals2/signal.hpp>

#include <osgUtil/Optimizer>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>

#include <osg/Material>
#include <osg/Geode>
#include <osg/BlendFunc>
#include <osg/Depth>
#include <osg/Projection>
#include <osg/MatrixTransform>
#include <osg/Camera>
#include <osg/io_utils>
#include <osg/ShapeDrawable>

#include <osgText/Text>

/**
 * class to handle events with a pick
 */
class WPickHandler: public osgGA::GUIEventHandler
{
public:
    /**
     * Virtual destructor needed because of virtual fuction.
     */
    virtual ~WPickHandler();

    bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa );

    virtual void pick( osgViewer::View* view, const osgGA::GUIEventAdapter& ea );
    virtual void unpick();

    /**
     * Gives information about the picked object.
     */
    std::string getHitResult();

    /**
     * returns the m_pickSignal to for registering to it.
     */
    boost::signals2::signal1< void, std::string >* getPickSignal();

protected:
    std::string m_hitResult; //!< Textual representation of the result of a pick.

private:
    boost::signals2::signal1<void, std::string > m_pickSignal; //!< One can register to this signal to receive pick events.
};

#endif  // WPICKHANDLER_H
