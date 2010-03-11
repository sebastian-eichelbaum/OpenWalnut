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

#include <boost/shared_ptr.hpp>

#include "../../common/WLogger.h"
#include "../../dataHandler/WDataSetSingle.h"
#include "../../dataHandler/WGridRegular3D.h"
#include "WRasterAlgorithm.h"

WRasterAlgorithm::WRasterAlgorithm( boost::shared_ptr< WGridRegular3D > grid )
    : m_grid( grid ),
      m_values( grid->size(), 0.0 ),
      m_dirValues( 3 * grid->size(), 0.0 )
{
    // NOTE: I assume the Voxelizer class is only used by the WMVoxelizer module, hence the
    // source is "Voxelizer".
    wlog::debug( "Voxelizer" ) << "WRasterAlogrithm created " << m_values.size() << " values.";
}

WRasterAlgorithm::~WRasterAlgorithm()
{
}

boost::shared_ptr< WDataSetSingle > WRasterAlgorithm::generateDataSet() const
{
    boost::shared_ptr< WValueSet< double > > valueSet( new WValueSet< double >( 0, 1, m_values, W_DT_DOUBLE ) );
    return boost::shared_ptr< WDataSetSingle >( new WDataSetSingle( valueSet, m_grid ) );
}

boost::shared_ptr< WDataSetSingle > WRasterAlgorithm::generateVectorDataSet() const
{
    boost::shared_ptr< WValueSet< double > > valueSet( new WValueSet< double >( 1, 3, m_dirValues, W_DT_DOUBLE ) );
    return boost::shared_ptr< WDataSetSingle >( new WDataSetSingle( valueSet, m_grid ) );
}

