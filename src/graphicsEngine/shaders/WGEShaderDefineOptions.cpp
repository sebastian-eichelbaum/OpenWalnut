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

#include <stdarg.h>

#include <algorithm>

#include "../../common/exceptions/WPreconditionNotMet.h"

#include "WGEShaderDefineOptions.h"

WGEShaderDefineOptions::WGEShaderDefineOptions( std::string first,
                            std::string option2, std::string option3, std::string option4, std::string option5,
                            std::string option6, std::string option7, std::string option8, std::string option9,
                            std::string option10 ):
    WGEShaderPreprocessor(),
    m_options( 1, first ),
    m_idx( 0 )
{
    // init
    if ( !option2.empty() )
    {
        m_options.push_back( option2 );
    }
    if ( !option3.empty() )
    {
        m_options.push_back( option3 );
    }
    if ( !option4.empty() )
    {
        m_options.push_back( option4 );
    }
    if ( !option5.empty() )
    {
        m_options.push_back( option5 );
    }
    if ( !option6.empty() )
    {
        m_options.push_back( option6 );
    }
    if ( !option7.empty() )
    {
        m_options.push_back( option7 );
    }
    if ( !option8.empty() )
    {
        m_options.push_back( option8 );
    }
    if ( !option9.empty() )
    {
        m_options.push_back( option9 );
    }
    if ( !option10.empty() )
    {
        m_options.push_back( option10 );
    }
}

WGEShaderDefineOptions::~WGEShaderDefineOptions()
{
    // cleanup
}

std::string WGEShaderDefineOptions::process( const std::string& /*file*/, const std::string& code ) const
{
    if ( !getActive() )
    {
        return code;
    }

    return "#define " + getActiveOptionName() + "\n" + code;
}

size_t WGEShaderDefineOptions::getActiveOption() const
{
    return m_idx;
}

std::string WGEShaderDefineOptions::getActiveOptionName() const
{
    return m_options[ m_idx ];
}

void WGEShaderDefineOptions::activateOption( size_t idx )
{
    WPrecond( idx < m_options.size(), "Index invalid." );
    if ( idx != m_idx )
    {
        m_idx = idx;
        updated();
    }
}

void WGEShaderDefineOptions::addOption( std::string opt )
{
    if ( std::find( m_options.begin(), m_options.end(), opt ) == m_options.end() )
    {
        m_options.push_back( opt );

        // signal update
        updated();
    }
}

