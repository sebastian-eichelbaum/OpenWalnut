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

#ifndef WDATASETSINGLE_H
#define WDATASETSINGLE_H

#include <boost/shared_ptr.hpp>
#include "WDataSet.h"

class WValueSet;
class WGrid;

/**
 * A data set consisting of a set of values based on a grid.
 */
class WDataSetSingle : WDataSet
{
public:
    WDataSetSingle( boost::shared_ptr<WValueSet> newValueSet,
                    boost::shared_ptr<WGrid> newGrid,
                    boost::shared_ptr<WMetaInfo> newMetaInfo );
    virtual ~WDataSetSingle();
    boost::shared_ptr<WValueSet> getValueSet() const;
    boost::shared_ptr<WGrid> getGrid() const;

private:
    boost::shared_ptr< WGrid > m_grid;
    boost::shared_ptr< WValueSet > m_valueSet;
};

#endif  // WDATASETSINGLE_H
