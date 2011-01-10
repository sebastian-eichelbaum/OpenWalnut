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

WGEShaderDefineOptions::WGEShaderDefineOptions( std::string first ):
    WGEShaderPreprocessor(),
    m_options( 1, first ),
    m_idx( 0 )
{
    // init
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

