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
    properties();
}

WROI::~WROI()
{
}

void WROI::properties()
{
    m_properties = boost::shared_ptr< WProperties >( new WProperties( "Properties", "This ROI's properties" ) );

    m_active = m_properties->addProperty( "active", "description", true, boost::bind( &WROI::propertyChanged, this ) );
    m_active->setHidden( true );

    m_dirty = m_properties->addProperty( "Dirty", "description", true, boost::bind( &WROI::propertyChanged, this ) );
    m_not = m_properties->addProperty( "NOT", "description", false, boost::bind( &WROI::propertyChanged, this ) );
}

void WROI::propertyChanged()
{
}

boost::shared_ptr<WProperties> WROI::getProperties()
{
    return m_properties;
}

boost::signals2::signal0< void >* WROI::getSignalIsModified()
{
    return &m_signalIsModified;
}

void WROI::setNot( bool isNot )
{
    m_not->set( isNot );
    m_dirty->set( true );
}

bool WROI::isNot()
{
    return m_not->get();
}

bool WROI::active()
{
    return m_active->get();
}

void WROI::setActive( bool active )
{
    m_active->set( active );
    m_dirty->set( true );
}

bool WROI::dirty( bool reset )
{
    if ( reset )
    {
        bool tmp = m_dirty->get();
        m_dirty->set( false );
        return tmp;
    }
    else
    {
        return m_dirty->get();
    }
}

void WROI::hide()
{
    setNodeMask( 0x0 );
}

void WROI::unhide()
{
    setNodeMask( 0xFFFFFFFF );
}
