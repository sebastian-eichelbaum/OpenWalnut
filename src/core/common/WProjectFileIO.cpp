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

#include "../kernel/WProjectFile.h"

#include "WProjectFileIO.h"

WProjectFileIO::WProjectFileIO():
    m_errors(),
    m_applyOrder( POST_MODULES )
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

void WProjectFileIO::printProperties( std::ostream& output, boost::shared_ptr< WProperties > props, std::string indent, //NOLINT
                                      std::string prefix, unsigned int index, std::string indexPrefix )
{
    // lock, unlocked if l looses focus
    WProperties::PropertySharedContainerType::ReadTicket l = props->getProperties();

    output << indent << "// Property Group: " << props->getName() << std::endl;

    // iterate of them and print them to output
    for( WProperties::PropertyConstIterator iter = l->get().begin(); iter != l->get().end(); ++iter )
    {
        // information properties do not get written
        if( ( *iter )->getPurpose () == PV_PURPOSE_INFORMATION )
        {
            continue;
        }
        if( ( *iter )->getType() != PV_GROUP )
        {
            output << indent + "    " << "PROPERTY:(" << indexPrefix << index << "," << prefix + ( *iter )->getName() << ")="
                   << ( *iter )->getAsString() << std::endl;
        }
        else
        {
            // it is a group -> recursively print it
            if( prefix.empty() )
            {
                printProperties( output, ( *iter )->toPropGroup(), indent + "    ", ( *iter )->getName() + "/", index, indexPrefix );
            }
            else
            {
                printProperties( output, ( *iter )->toPropGroup(), indent + "    ", prefix + ( *iter )->getName() + "/", index, indexPrefix );
            }
        }
    }

    output << indent << "// Property Group END: " << props->getName() << std::endl;
}

void WProjectFileIO::setProject( WProjectFile* project )
{
    m_project = project;
}

WProjectFile* WProjectFileIO::getProject() const
{
    return m_project;
}

WProjectFileIO::ApplyOrder WProjectFileIO::getApplyOrder() const
{
    return m_applyOrder;
}

void WProjectFileIO::setApplyOrder( WProjectFileIO::ApplyOrder order )
{
    m_applyOrder = order;
}

