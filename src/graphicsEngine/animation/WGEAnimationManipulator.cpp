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

#include <cmath>
#include <iostream>

#include "WGEAnimationTimer.h"

#include "WGEAnimationManipulator.h"

WGEAnimationManipulator::WGEAnimationManipulator( WGEAnimationTimer::ConstSPtr timer ):
    m_matrix( osg::Matrixd::identity() ),
    m_timer( timer ),
    m_homeOffsetTime( timer->elapsed() ),
    m_paused( true )
{
    // initialize
}

WGEAnimationManipulator::~WGEAnimationManipulator()
{
    // clean up
}

void WGEAnimationManipulator::setByMatrix( const osg::Matrixd& matrix )
{
    m_matrix.invert( matrix );
}

void WGEAnimationManipulator::setByInverseMatrix( const osg::Matrixd& matrix )
{
    m_matrix = matrix;
}

osg::Matrixd WGEAnimationManipulator::getMatrix() const
{
    return osg::Matrixd::inverse( m_matrix );
}

osg::Matrixd WGEAnimationManipulator::getInverseMatrix() const
{
    return m_matrix;
}

bool WGEAnimationManipulator::handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& /* us */ )
{
    switch( ea.getEventType() )
    {
    case osgGA::GUIEventAdapter::FRAME:
        handleFrame();
        return false;
    case osgGA::GUIEventAdapter::KEYDOWN:
        // if ( ea.getKey() == ' ' )   // space resets
        // {
        //     home( 0 );
        //     return true;
        // }
    default:
        break;
    }
    return false;
}

void WGEAnimationManipulator::init( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us )
{
    home( ea, us );
}

void WGEAnimationManipulator::home( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& /* us */ )
{
    home( ea.getTime() );
}

void WGEAnimationManipulator::home( double /* currentTime */ )
{
    m_homeOffsetTime = m_timer->elapsed();
}

void WGEAnimationManipulator::setTimer( WGEAnimationTimer::ConstSPtr timer )
{
    m_timer = timer;
    home( 0 );
}

double degToRad( double deg )
{
    const double PI = 3.14159265;
    return ( deg * PI / 180.0 );
}

void WGEAnimationManipulator::handleFrame()
{
    const double PI = 3.14159265;

    // calculate the proper sec:frame coordinate:

    // time in seconds, it always relates to a 24 frames per second system
    double elapsed = m_timer->elapsed() - m_homeOffsetTime;

    double aSpeed = 22.5;   // 45.0 degree per second
    double angle = degToRad( aSpeed * elapsed );

    // this brings the BBox to the center, makes it larger and rotates the front towards the camera
    osg::Matrixd mBBScale = osg::Matrixd::scale( 1.75, 1.75, 1.75 );
    osg::Matrixd mBBTranslate = osg::Matrixd::translate( -159.0 / 2.0, -199.0 / 2.0, -159.0 / 2.0 );
    osg::Matrixd mBBRotate =  osg::Matrixd::rotate( -PI / 2.0, 1.0, 0.0, 0.0 ) *
                              osg::Matrixd::rotate(  PI, 0.0, 1.0, 0.0 );

    osg::Matrixd m1 = osg::Matrixd::rotate( angle, 1.0, 0.0, 0.0 );
    osg::Matrixd m2 = osg::Matrixd::rotate( angle, 0.0, 1.0, 0.0 );


    m_matrix = mBBTranslate * mBBScale *mBBRotate * m2;
}

