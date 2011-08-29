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
#include <iostream>
#include <sstream>

#include "WTerminalColor.h"

WTerminalColor::WTerminalColor():
    m_attrib( Default ),
    m_foreground( FGBlack ),
    m_background( BGBlack ),
    m_enabled( false )
{
    m_colorString = "";
    m_colorResetString = "";

    generateControlStrings();
}

WTerminalColor::WTerminalColor( TerminalColorAttribute attrib, TerminalColorForeground foreground, TerminalColorBackground background ):
    m_attrib( attrib ),
    m_foreground( foreground ),
    m_background( background ),
    m_enabled( true )
{
    m_colorString = "";
    m_colorResetString = "";

    generateControlStrings();
}

WTerminalColor::~WTerminalColor()
{
    // cleanup
}

void WTerminalColor::generateControlStrings()
{
    m_colorString = "";
    m_colorResetString = "";

#if defined( __linux__ ) || defined( __APPLE__ )
    if( m_enabled && ( m_attrib != Default ) )
    {
        std::ostringstream ss;
        char cStart = 0x1B;
        ss << cStart << "[" << m_attrib << ";" << m_foreground;

        // handle an unset background specially
        if( m_background == BGNone )
        {
            ss << "m";
        }
        else
        {
            ss << ";" << m_background << "m";
        }

        m_colorString = ss.str();

        // build reset string
        std::ostringstream ss2;
        ss2 << cStart << "[0m";
        m_colorResetString = ss2.str();
    }
#endif
}

std::ostream& WTerminalColor::operator<<( std::ostream& ostr ) const
{
    return ostr << m_colorString;
}

std::string WTerminalColor::operator!() const
{
    return m_colorResetString;
}

std::string WTerminalColor::operator()() const
{
    return m_colorString;
}

std::string WTerminalColor::operator+( const std::string& istr ) const
{
    return m_colorString + istr;
}

void WTerminalColor::setEnabled( bool enabled )
{
    m_enabled = enabled;

    generateControlStrings();
}

bool WTerminalColor::isEnabled() const
{
    return m_enabled;
}

std::string WTerminalColor::operator()( const std::string s ) const
{
    return m_colorString + s + m_colorResetString;
}

