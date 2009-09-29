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

#include "WSubject.h"
#include "exceptions/WDHNoSuchDataSet.h"


WSubject::WSubject()
    : m_name( "Not named yet" ),
      m_dataSets( 0 )
{
}

WSubject::WSubject( std::string name )
    : m_name( name ),
      m_dataSets( 0 )
{
}

std::string WSubject::getName() const
{
    return m_name;
}

boost::shared_ptr< WDataSet > WSubject::getDataSet( const unsigned int dataSetId ) const
{
    if( dataSetId >= m_dataSets.size() )
        throw WDHNoSuchDataSet( "Index too large." );
    return m_dataSets.at( dataSetId );
}

boost::shared_ptr< const WDataSet > WSubject::operator[]( const unsigned int dataSetId ) const
{
    return getDataSet( dataSetId );
}

void WSubject::addDataSet( boost::shared_ptr< WDataSet > newDataSet )
{
    m_dataSets.push_back( newDataSet );
}

unsigned int WSubject::getNumberOfDataSets() const
{
    return m_dataSets.size();
}
