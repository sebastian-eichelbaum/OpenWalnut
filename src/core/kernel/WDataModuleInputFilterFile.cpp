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

#include <boost/algorithm/string/predicate.hpp>

#include "WDataModuleInputFile.h"

#include "WDataModuleInputFilterFile.h"

WDataModuleInputFilterFile::WDataModuleInputFilterFile( std::string extension, std::string description ):
    WDataModuleInputFilter( description ),
    m_extension( extension )
{
    // initialize members
}

WDataModuleInputFilterFile::~WDataModuleInputFilterFile()
{
    // cleanup
}

bool WDataModuleInputFilterFile::matches( WDataModuleInput::ConstSPtr input ) const
{
    WDataModuleInputFile::ConstSPtr file = boost::dynamic_pointer_cast< const WDataModuleInputFile >( input );
    if( file )
    {
        return boost::algorithm::ends_with( file->getFilename().string(), m_extension );
    }
    return false;
}

const std::string& WDataModuleInputFilterFile::getExtension() const
{
    return m_extension;
}

