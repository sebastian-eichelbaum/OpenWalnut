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

#include "WDataSetSingle.h"
#include "WValueSet.hpp"
#include "WGrid.h"

WDataSetSingle::WDataSetSingle( boost::shared_ptr<WValueSetBase> newValueSet,
                                boost::shared_ptr<WGrid> newGrid,
                                boost::shared_ptr< WMetaInfo > newMetaInfo )
    : WDataSet( newMetaInfo )
{
    assert( newValueSet );
    assert( newGrid );
    assert( newMetaInfo );
    assert( newValueSet->size() == newGrid->size() );

    m_valueSet = newValueSet;
    m_grid = newGrid;
}

WDataSetSingle::~WDataSetSingle()
{
    // TODO(wiebel): Auto-generated destructor stub
}

boost::shared_ptr<WValueSetBase> WDataSetSingle::getValueSet() const
{
    return m_valueSet;
}

boost::shared_ptr<WGrid> WDataSetSingle::getGrid() const
{
    return m_grid;
}
