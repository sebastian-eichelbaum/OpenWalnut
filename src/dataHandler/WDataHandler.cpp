//---------------------------------------------------------------------------
//
// Project: OpenWalnut
//
// Copyright 2009 SomeCopyrightowner
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
#include <vector>

#include "WDataHandler.h"
#include "WSubject.h"
#include "exceptions/WNoSuchDataSetException.h"
#include "WLoaderManager.h"

WDataHandler::WDataHandler()
{
}


boost::shared_ptr< const WSubject > WDataHandler::getSubject( const unsigned int subjectId ) const
{
    if( subjectId >= m_subjects.size() )
        throw WNoSuchDataSetException( "Index too large." );
    return m_subjects.at( subjectId );
}


void WDataHandler::addSubject( boost::shared_ptr< WSubject > newSubject )
{
    m_subjects.push_back( newSubject );
}


unsigned int WDataHandler::getNumberOfSubjects() const
{
    return m_subjects.size();
}


void WDataHandler::loadDataSets( std::vector< std::string > fileNames )
{
    WLoaderManager lm;
    for ( size_t i = 0 ; i < fileNames.size() ; ++i)
    {
        // TODO( wiebel ): need to associate the dataset to its subject
        lm.load( fileNames[i], shared_from_this() );
    }
}
