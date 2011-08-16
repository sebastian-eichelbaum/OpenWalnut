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

#include "WGENoOpManipulator.h"

WGENoOpManipulator::WGENoOpManipulator():
    osgGA::MatrixManipulator()
{
    // initialize members
}

WGENoOpManipulator::~WGENoOpManipulator()
{
    // cleanup
}

const char* WGENoOpManipulator::className() const
{
    return "WGENoOpManipulator";
}

void WGENoOpManipulator::setByMatrix( const osg::Matrixd& /* matrix */ )
{
    // do nothing here
}

void WGENoOpManipulator::setByInverseMatrix( const osg::Matrixd& /* matrix */ )
{
    // do nothing here
}

osg::Matrixd WGENoOpManipulator::getMatrix() const
{
    return osg::Matrixd::identity();
}

osg::Matrixd WGENoOpManipulator::getInverseMatrix() const
{
    return osg::Matrixd::identity();
}

void WGENoOpManipulator::home( const osgGA::GUIEventAdapter& /*ea*/, osgGA::GUIActionAdapter& /*us*/ )
{
    // No Op
}

void WGENoOpManipulator::init( const osgGA::GUIEventAdapter& /*ea*/, osgGA::GUIActionAdapter& /*us*/ )
{
    // No Op
}

bool WGENoOpManipulator::handle( const osgGA::GUIEventAdapter& /*ea*/, osgGA::GUIActionAdapter& /*us*/ )
{
    // No Op
    return true;
}
