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

#include <osg/Vec3>

#include "WPickHandler.h"
#include "WPickInfo.h"

WPickHandler::~WPickHandler()
{
}

WPickInfo WPickHandler::getHitResult()
{
    return m_hitResult;
}

wmath::WPosition WPickHandler::getHitPosition()
{
    return m_hitPosGlobal;
}

boost::signals2::signal1< void, WPickInfo >* WPickHandler::getPickSignal()
{
    return &m_pickSignal;
}

bool WPickHandler::handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
{
    switch ( ea.getEventType() )
    {
        case osgGA::GUIEventAdapter::DRAG : // Mouse pushed an dragged
        case osgGA::GUIEventAdapter::PUSH : // Mousebutton pushed
        {
            unsigned int buttonMask = ea.getButtonMask();
            if( buttonMask == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON )
            {
                osgViewer::View* view = static_cast< osgViewer::View* >( &aa );
                if ( view )
                {
                    pick( view, ea );
                }
            }
            return false;
        }
        case osgGA::GUIEventAdapter::RELEASE : // Mousebutton released
        {
            osgViewer::View* view = static_cast< osgViewer::View* >( &aa );
            if ( view )
            {
                unpick();
            }
            return false;
        }
        case osgGA::GUIEventAdapter::KEYDOWN : // Key on keyboard pushed.
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

void WPickHandler::unpick( )
{
    if( m_hitResult != WPickInfo( "", wmath::WPosition(), WPickInfo::NONE ) )
    {
        m_hitResult = WPickInfo( "unpick", wmath::WPosition(), WPickInfo::NONE );
        m_lastPick = WPickInfo( "", wmath::WPosition(), WPickInfo::NONE );
    }
    m_pickSignal( getHitResult() );
}

std::string extractSuitableName( osgUtil::LineSegmentIntersector::Intersections::iterator hitr )
{
    if( !hitr->nodePath.empty()  && !( hitr->nodePath.back()->getName().empty() ) )
    {
        return hitr->nodePath.back()->getName();
    }
    else if ( hitr->drawable.valid() )
    {
        return  hitr->drawable->className();
    }
    assert( 0 && "This should not happen. Tell \"wiebel\" if it does." );
    return ""; // This line will not be reached.
}

void WPickHandler::pick( osgViewer::View* view, const osgGA::GUIEventAdapter& ea )
{
    osgUtil::LineSegmentIntersector::Intersections intersections;
    m_hitResult = WPickInfo();
    float x = ea.getX();
    float y = ea.getY();


    if ( view->computeIntersections( x, y, intersections ) )
    {
        bool lastPickIsStillInList = false;
        osgUtil::LineSegmentIntersector::Intersections::iterator hitr = intersections.begin();


        while( ( hitr != intersections.end() ) && !lastPickIsStillInList ) // got to lastPicked if it can be found in intersections list
        {
            WPickInfo pickInfoTmp( extractSuitableName( hitr ), wmath::WPosition(), WPickInfo::NONE );
            lastPickIsStillInList |= ( pickInfoTmp.getName() == m_lastPick.getName() );

            if( !lastPickIsStillInList ) // if iteration not finished yet go on in list
            {
                ++hitr;
            }
        }

        if( !lastPickIsStillInList ) // if lastPicked is not found use the first in intersections list
        {
            hitr = intersections.begin();
        }

        wmath::WPosition pickPos;
        pickPos[0] = hitr->getLocalIntersectPoint()[0];
        pickPos[1] = hitr->getLocalIntersectPoint()[1];
        pickPos[2] = hitr->getLocalIntersectPoint()[2];

        WPickInfo pickInfo( extractSuitableName( hitr ), pickPos, WPickInfo::NONE );

        m_hitResult = pickInfo;

        osg::Vec3 globalHit = hitr->getWorldIntersectPoint();
        m_hitPosGlobal = wmath::WPosition( globalHit[0], globalHit[1], globalHit[2] );

        m_lastPick = pickInfo;
    }
    m_pickSignal( getHitResult() );
}
