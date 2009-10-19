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

#include <string>

#include "WQtCheckBox.h"

WQtCheckBox::WQtCheckBox()
    : QCheckBox()
{
    m_name = "";
    connect( this, SIGNAL( toggled( bool ) ), this, SLOT( emitStateChanged() ) );
}

WQtCheckBox::~WQtCheckBox()
{
}

boost::signal2< void, std::string, bool >*WQtCheckBox::getboostSignal()
{
    return &m_boostSignal;
}

void WQtCheckBox::setName( std::string name )
{
    m_name = name;
}

void WQtCheckBox::emitStateChanged()
{
    m_boostSignal( m_name, this->isChecked() );
}
