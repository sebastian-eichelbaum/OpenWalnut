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
#include <string>
#include <vector>

#include "../../common/exceptions/WPreconditionNotMet.h"
#include "WGEShaderDefineOptions.h"

WGEShaderDefineOptions::WGEShaderDefineOptions( std::string first,
                            std::string option2, std::string option3, std::string option4, std::string option5,
                            std::string option6, std::string option7, std::string option8, std::string option9,
                            std::string option10 ):
    WGEShaderPreprocessor(),
    m_options( 1, first ),
    m_idx( 1, 0 )
{
    // init
    if( !option2.empty() )
    {
        m_options.push_back( option2 );
    }
    if( !option3.empty() )
    {
        m_options.push_back( option3 );
    }
    if( !option4.empty() )
    {
        m_options.push_back( option4 );
    }
    if( !option5.empty() )
    {
        m_options.push_back( option5 );
    }
    if( !option6.empty() )
    {
        m_options.push_back( option6 );
    }
    if( !option7.empty() )
    {
        m_options.push_back( option7 );
    }
    if( !option8.empty() )
    {
        m_options.push_back( option8 );
    }
    if( !option9.empty() )
    {
        m_options.push_back( option9 );
    }
    if( !option10.empty() )
    {
        m_options.push_back( option10 );
    }
}

WGEShaderDefineOptions::WGEShaderDefineOptions( std::vector< std::string > options ):
    WGEShaderPreprocessor(),
    m_options( options ),
    m_idx( 1, 0 )
{
    WPrecond( options.size() >= 1, "You need to specify at least one option." );
}

WGEShaderDefineOptions::~WGEShaderDefineOptions()
{
    // cleanup
}

std::string WGEShaderDefineOptions::process( const std::string& /*file*/, const std::string& code ) const
{
    if( !getActive() )
    {
        return code;
    }

    // add a define for every active option
    std::stringstream ss;
    for( IdxList::const_iterator iter = m_idx.begin(); iter != m_idx.end(); ++iter )
    {
        ss << "#define " + getOptionName( *iter ) << std::endl;
    }

    // add the original code again
    ss << code;
    return ss.str();
}

const WGEShaderDefineOptions::IdxList& WGEShaderDefineOptions::getActiveOptions() const
{
    return m_idx;
}

std::string WGEShaderDefineOptions::getOptionName( size_t idx ) const
{
    WPrecond( idx < m_options.size(), "Index invalid." );
    return m_options[ idx ];
}

void WGEShaderDefineOptions::activateOption( size_t idx, bool exclusive )
{
    WPrecond( idx < m_options.size(), "Index invalid." );

    // if this option is already active, avoid an update even if exclusive is true
    if( ( m_idx.size() == 1 ) && ( m_idx[ 0 ] == idx ) )
    {
        return;
    }

    if( exclusive )
    {
        m_idx.clear();
    }

    // is the option already active?
    if( std::find( m_idx.begin(), m_idx.end(), idx ) == m_idx.end() )
    {
        m_idx.push_back( idx );
        updated();
    }
}

void WGEShaderDefineOptions::deactivateOption( size_t idx )
{
    IdxList::iterator iter = std::find( m_idx.begin(), m_idx.end(), idx );
    if( iter != m_idx.end() )
    {
        m_idx.erase( iter );
        updated();
    }
}

void WGEShaderDefineOptions::activateAllOptions()
{
    // simply add all
    for( size_t i = 0; i < m_options.size(); ++i )
    {
        m_idx.push_back( i );
    }

    updated();
}

void WGEShaderDefineOptions::deactivateAllOptions()
{
    // clear active list
    m_idx.clear();
    updated();
}

void WGEShaderDefineOptions::addOption( std::string opt )
{
    WPrecond( !opt.empty(), "Options need to have a non-empty name." );
    if( std::find( m_options.begin(), m_options.end(), opt ) == m_options.end() )
    {
        m_options.push_back( opt );

        // signal update
        updated();
    }
}

void WGEShaderDefineOptions::setActivationList( const IdxList& newList )
{
    if( m_idx != newList )
    {
        m_idx = newList;
        updated();
    }
}

