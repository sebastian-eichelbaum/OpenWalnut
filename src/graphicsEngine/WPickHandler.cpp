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

#include <iostream>
#include <string>

#include "WPickHandler.h"

WPickHandler::WPickHandler()
{
}

WPickHandler::~WPickHandler()
{
}

std::string WPickHandler::getHitResult()
{
    return m_hitResult;
}

bool WPickHandler::handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
{
    switch ( ea.getEventType() )
    {
        case( osgGA::GUIEventAdapter::PUSH ):
        {
            osgViewer::View* view = static_cast< osgViewer::View* >( &aa );
            if ( view )
                pick( view, ea );
            return false;
        }
        case( osgGA::GUIEventAdapter::KEYDOWN ):
        {
            if ( ea.getKey() == 'c' )
            {
                osgViewer::View* view = static_cast< osgViewer::View* >( &aa );
                osg::ref_ptr< osgGA::GUIEventAdapter > event = new osgGA::GUIEventAdapter( ea );
                event->setX( ( ea.getXmin() + ea.getXmax() ) * 0.5 );
                event->setY( ( ea.getYmin() + ea.getYmax() ) * 0.5 );
                if ( view )
                {
                    pick( view, *event );
                }
            }
            return false;
        }
        default:
            return false;
    }
}

void WPickHandler::pick( osgViewer::View* view, const osgGA::GUIEventAdapter& ea )
{
    osgUtil::LineSegmentIntersector::Intersections intersections;

    m_hitResult = "";
    float x = ea.getX();
    float y = ea.getY();

    if ( view->computeIntersections( x, y, intersections ) )
    {
        osgUtil::LineSegmentIntersector::Intersections::iterator hitr = intersections.begin();
        std::ostringstream os;
        if ( !hitr->nodePath.empty() && !( hitr->nodePath.back()->getName().empty() ) )
        {
            // the geodes are identified by name.
            os << "Object \"" << hitr->nodePath.back()->getName() << "\"" << std::endl;
        }
        else if ( hitr->drawable.valid() )
        {
            os << "Object \"" << hitr->drawable->className() << "\"" << std::endl;
        }

        os << "        local coords vertex(" << hitr->getLocalIntersectPoint() << ")" << "  normal(" << hitr->getLocalIntersectNormal() << ")"
                << std::endl;
        os << "        world coords vertex(" << hitr->getWorldIntersectPoint() << ")" << "  normal(" << hitr->getWorldIntersectNormal() << ")"
                << std::endl;
        m_hitResult += os.str();
    }
}
