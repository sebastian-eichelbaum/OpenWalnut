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

#include <vector>
#include <string>

#include "WLogger.h"

#include "WProjectFileIO.h"

WProjectFileIO::WProjectFileIO():
    m_errors()
{
    // initialize
}

WProjectFileIO::~WProjectFileIO()
{
    // cleanup!
}

void WProjectFileIO::done()
{
    // do nothing here. Overwrite this method if your specific parser needs to do some post processing.
}

bool WProjectFileIO::hadErrors() const
{
    return m_errors.size();
}

const std::vector< std::string >& WProjectFileIO::getErrors() const
{
    return m_errors;
}

bool WProjectFileIO::hadWarnings() const
{
    return m_warnings.size();
}

const std::vector< std::string >& WProjectFileIO::getWarnings() const
{
    return m_warnings;
}

void WProjectFileIO::addError( std::string description )
{
    wlog::error( "Project Loader" ) << description;
    m_errors.push_back( description );
}

void WProjectFileIO::addWarning( std::string description )
{
    wlog::warn( "Project Loader" ) << description;
    m_warnings.push_back( description );
}

