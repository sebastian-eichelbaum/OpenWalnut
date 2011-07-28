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

#include "core/common/exceptions/WPreconditionNotMet.h"

#include "WSegmentationAlgo.h"

WSegmentationAlgo::WSegmentationAlgo()
    : m_properties(),
      m_propCondition( new WCondition() ),
      m_dataSet()
{
}

WSegmentationAlgo::~WSegmentationAlgo()
{
}

boost::shared_ptr< WCondition > WSegmentationAlgo::getCondition()
{
    return m_propCondition;
}

void WSegmentationAlgo::initProperties( WPropGroup group )
{
    if( !m_properties )
    {
        m_properties = group;
        properties();
    }
}

WSegmentationAlgo::DataSetPtr WSegmentationAlgo::segment( DataSetPtr dataset )
{
    WPrecond( dataset, "Invalid dataset." );
    m_dataSet = dataset;
    DataSetPtr result = applyOperation();
    m_dataSet = DataSetPtr();
    return result;
}

void WSegmentationAlgo::hideProperties( bool hide )
{
    WPrecond( m_properties, "Missing properties." );
    m_properties->setHidden( hide );
}
