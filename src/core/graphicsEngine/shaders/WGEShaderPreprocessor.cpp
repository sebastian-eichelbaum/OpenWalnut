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

#include "WGEShaderPreprocessor.h"

WGEShaderPreprocessor::WGEShaderPreprocessor():
    m_updateCondition( new WCondition() ),
    m_active( true )
{
    // initialize members
}

WGEShaderPreprocessor::~WGEShaderPreprocessor()
{
    // cleanup
}

WCondition::SPtr WGEShaderPreprocessor::getChangeCondition() const
{
    return m_updateCondition;
}

void WGEShaderPreprocessor::updated()
{
    m_updateCondition->notify();
}

void WGEShaderPreprocessor::setActive( bool active )
{
    bool update = m_active != active;
    m_active = active;
    if( update )
    {
        updated();
    }
}

bool WGEShaderPreprocessor::getActive() const
{
    return m_active;
}

