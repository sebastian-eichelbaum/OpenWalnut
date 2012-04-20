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
#include "WModule.h"

#include "WModuleMetaInformation.h"

WModuleMetaInformation::WModuleMetaInformation( std::string name ):
    m_name( name ),
    m_loaded( false )
{
    // initialize members
}

WModuleMetaInformation::WModuleMetaInformation( boost::shared_ptr< WModule > module ):
    m_name( module->getName() ),
    m_loaded( false )
{
    // check whether file exists
    boost::filesystem::path metafile = module->getLocalPath() / "META";
    if( !boost::filesystem::exists( metafile ) )
    {
        return;
    }

    // try loading it
    try
    {
        m_metaData = WStructuredTextParser::StructuredValueTree( metafile );
        m_loaded = true;
    }
    catch( const WException& e )
    {
        wlog::error( "Module (" + m_name + ")" ) << "Meta file load failed. Message: " << e.what();
    }
}

WModuleMetaInformation::~WModuleMetaInformation()
{
    // cleanup
}

