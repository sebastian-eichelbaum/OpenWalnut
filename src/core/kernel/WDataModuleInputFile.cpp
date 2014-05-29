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

#include "WDataModuleInputFile.h"

WDataModuleInputFile::WDataModuleInputFile( boost::filesystem::path fname ):
    m_filename( fname )
{
    // initialize members
}

WDataModuleInputFile::WDataModuleInputFile( std::string fname ):
    m_filename( fname )
{
    // initialize members
}

WDataModuleInputFile::~WDataModuleInputFile()
{
    // cleanup
}

boost::filesystem::path WDataModuleInputFile::getFilename() const
{
    return m_filename;
}

std::string WDataModuleInputFile::asString() const
{
    return m_filename.filename().string();
}

std::string WDataModuleInputFile::serialize() const
{
    return m_filename.string();
}

std::string WDataModuleInputFile::getName() const
{
    return "FILE";
}

