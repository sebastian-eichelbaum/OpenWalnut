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

#ifndef WDATASET_H
#define WDATASET_H

#include <boost/shared_ptr.hpp>

class WMetaInfo;

/**
 * Base class for all data set types. This class has a number of
 * subclasses specifying the different types of data sets.
 * Two of dataset type reprent single and time-dependent
 * datasets (compound of several time steps) respectively.
 * This bas class contains the pointer to the meta information.
 *
 */
class WDataSet
{
public:
    explicit WDataSet( boost::shared_ptr< WMetaInfo > newMetaInfo );

    boost::shared_ptr< WMetaInfo > getMetaInfo() const;

protected:
    boost::shared_ptr< WMetaInfo > m_metaInfo;
private:
};

#endif  // WDATASET_H
