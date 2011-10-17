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

#include "WGEShaderPropertyDefineOptions.h"

WGEShaderPropertyDefineOptions< WPropSelection >::SPtr WGEShaderPropertyDefineOptionsTools::createSelection(
           std::string propName, std::string propDescription,
           WProperties::SPtr propGroup,
           std::vector< WGEShaderPropertyDefineOptionsTools::NameDescriptionDefineTuple > defines )
{
    // the item selection:
    boost::shared_ptr< WItemSelection > selection( new WItemSelection() );
    std::vector< std::string > definesOnly;

    // add to the properties possible selection items list and option list
    for( std::vector< WGEShaderPropertyDefineOptionsTools::NameDescriptionDefineTuple >::const_iterator i = defines.begin(); i != defines.end();
          ++i )
    {
        selection->addItem( ( *i ).get< 0 >(), ( *i ).get< 1 >() );
        definesOnly.push_back( ( *i ).get< 2 >() );
    }

    WPropSelection prop = propGroup->addProperty( propName, propDescription, selection->getSelectorFirst() );
    // create the corresponding WGEShaderPropertyDefineOptions instance
    WGEShaderPropertyDefineOptions<>::SPtr defOptions( new WGEShaderPropertyDefineOptions<>( prop, definesOnly ) );
    return defOptions;
}

