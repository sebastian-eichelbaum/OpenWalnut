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

#include <iostream>
#include <string>
#include <vector>

#include "WDataHandler.h"
#include "WSubject.h"
#include "exceptions/WDHNoSuchDataSet.h"

WDataHandler::WDataHandler()
{
}

boost::shared_ptr< WSubject > WDataHandler::getSubject( const unsigned int subjectId ) const
{
    if( subjectId >= m_subjects.size() )
        throw WDHNoSuchDataSet( "Index too large." );
    return m_subjects.at( subjectId );
}

boost::shared_ptr< WSubject >  WDataHandler::operator[]( size_t subjectId ) const
{
    return getSubject( subjectId );
}

void WDataHandler::addSubject( boost::shared_ptr< WSubject > newSubject )
{
    m_subjects.push_back( newSubject );
}


unsigned int WDataHandler::getNumberOfSubjects() const
{
    return m_subjects.size();
}

