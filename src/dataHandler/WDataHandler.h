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

#ifndef WDATAHANDLER_H
#define WDATAHANDLER_H

#include <boost/shared_ptr.hpp>

#include <vector>

class WDataSet;

/**
 * Provides the environment for storing and accessing data sets.
 * \ingroup dataHandler
 */
class WDataHandler
{
public:
    boost::shared_ptr< WDataSet > getDataSet( unsigned int dataSetId ) const;
    void addDataSet( boost::shared_ptr< WDataSet > newDataSet );
    unsigned int getNumberOfDataSets() const;

protected:

private:
    std::vector< boost::shared_ptr< WDataSet > > m_dataSets;
};

/**
  \defgroup dataHandler Data Handler

  \brief
  This module implements the data storage facility of OpenWalnut.

*/


#endif  // WDATAHANDLER_H
