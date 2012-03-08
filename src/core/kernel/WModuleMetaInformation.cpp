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

#include "../common/WException.h"
#include "../common/WLogger.h"
#include "../common/WStructuredTextParser.h"

#include "WModuleMetaInformation.h"

/**
 * This creates a dummy entry for a module with name.
 *
 * \param name name of the module
 *
 * \return the object
 */
/*WStructuredTextParser::ObjectType createModuleRootElement( std::string name )
{
    WStructuredTextParser::KeyValueType namePair;
    namePair.m_name = "name";
    namePair.m_value = name;

    WStructuredTextParser::ObjectType module;
    module.m_name = "module";
    module.m_nodes.push_back( WStructuredTextParser::MemberType( namePair ) );

    return module;
}*/

WModuleMetaInformation::WModuleMetaInformation( std::string name ):
    m_name( name )
  //  m_metaData( createModuleRootElement( name ) )
{
    // initialize members
}

WModuleMetaInformation::WModuleMetaInformation( std::string name, boost::filesystem::path /* metafile */ ):
    m_name( name )
    //m_metaData( createModuleRootElement( name ) )
{
    try
    {
      ///  m_metaData = WStructuredTextParser::parseFromFile( metafile );
    }
    catch( const WException& e )
    {
        wlog::error( "Module (" + name + ")" ) << "Meta file load failed. Message: " << e.what();

        // reset meta data and add, at least a name filed.
        //m_metaData = WStructuredTextParser::SyntaxTree( createModuleRootElement( name ) );
    }
}

WModuleMetaInformation::~WModuleMetaInformation()
{
    // cleanup
}

