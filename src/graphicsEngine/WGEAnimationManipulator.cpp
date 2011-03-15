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

#include "WGEAnimationManipulator.h"
#include "WGraphicsEngine.h"

WGEAnimationManipulator::WGEAnimationManipulator():
    m_matrix( osg::Matrixd::identity() ),
    m_timer( osg::Timer() )
{
    // initialize
}

WGEAnimationManipulator::~WGEAnimationManipulator()
{
    // clean up
}

void WGEAnimationManipulator::setByMatrix( const osg::Matrixd& matrix )
{
    m_matrix = matrix;
}

void WGEAnimationManipulator::setByInverseMatrix( const osg::Matrixd& matrix )
{
    m_matrix.invert( matrix );
}

osg::Matrixd WGEAnimationManipulator::getMatrix() const
{
    return m_matrix;
}

osg::Matrixd WGEAnimationManipulator::getInverseMatrix() const
{
    return osg::Matrixd::inverse( m_matrix );
}

bool WGEAnimationManipulator::handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& /* us */ )
{
    switch( ea.getEventType() )
    {
    case osgGA::GUIEventAdapter::FRAME:
        handleFrame();
        return false;
    case osgGA::GUIEventAdapter::KEYDOWN:
        if ( ea.getKey() == ' ' )   // space resets
        {
            home( 0 );
            return true;
        }
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
    m_timer.setStartTick();;
}

void WGEAnimationManipulator::handleFrame()
{
    osg::Matrixd m1 = osg::Matrixd::identity();
    osg::Matrixd m2 = osg::Matrixd::identity();

    m_matrix = m1 * m2;
}

