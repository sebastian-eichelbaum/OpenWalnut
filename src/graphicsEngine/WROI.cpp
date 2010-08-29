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

#include "WROI.h"

#include "WPickHandler.h"

WROI::WROI() :
    osg::Geode()
{
}

WROI::~WROI()
{
}

boost::signals2::signal0< void >* WROI::getSignalIsModified()
{
    return &m_signalIsModified;
}

void WROI::setNot( bool isNot )
{
    m_isNot = isNot;
    m_isModified = true;
}

bool WROI::isNot()
{
    return m_isNot;
}

bool WROI::isActive()
{
    return m_isActive;
}

void WROI::setActive( bool active )
{
    m_isActive = active;
    m_isModified = true;
}

bool WROI::isModified()
{
    bool tmp = m_isModified;
    m_isModified = false;
    return tmp;
}

void WROI::hide()
{
    setNodeMask( 0x0 );
}

void WROI::unhide()
{
    setNodeMask( 0xFFFFFFFF );
}
