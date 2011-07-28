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

#include "core/common/WLogger.h"
#include "core/dataHandler/WDataSetScalar.h"
#include "core/dataHandler/WGridRegular3D.h"
#include "WRasterAlgorithm.h"

WRasterAlgorithm::WRasterAlgorithm( boost::shared_ptr< WGridRegular3D > grid )
    : m_grid( grid ),
      m_values( grid->size(), 0.0 )
{
    // NOTE: I assume the Voxelizer class is only used by the WMVoxelizer module, hence the
    // source is "Voxelizer".
    wlog::debug( "Voxelizer" ) << "WRasterAlogrithm created " << m_values.size() << " values.";
}

WRasterAlgorithm::~WRasterAlgorithm()
{
}

boost::shared_ptr< WDataSetScalar > WRasterAlgorithm::generateDataSet() const
{
    boost::shared_ptr< WValueSet< double > > valueSet( new WValueSet< double >( 0,
                                                                                1,
                                                                                boost::shared_ptr< std::vector< double > >(
                                                                                    new std::vector< double >( m_values ) ),
                                                                                W_DT_DOUBLE ) );
    return boost::shared_ptr< WDataSetScalar >( new WDataSetScalar( valueSet, m_grid ) );
}

void WRasterAlgorithm::addParameterizationAlgorithm( boost::shared_ptr< WRasterParameterization > algorithm )
{
    boost::unique_lock< boost::shared_mutex > lock =  boost::unique_lock< boost::shared_mutex >( m_parameterizationsLock );
    m_parameterizations.push_back( algorithm );
    lock.unlock();
}

void WRasterAlgorithm::newLine( const WLine& line )
{
    // NOTE: the list already is locked (in raster method, hopefully)
    for( size_t i = 0; i < m_parameterizations.size(); ++i )
    {
        m_parameterizations[ i ]->newLine( line );
    }
}

void WRasterAlgorithm::newSegment( const WPosition& start, const WPosition& end )
{
    // NOTE: the list already is locked (in raster method, hopefully)
    for( size_t i = 0; i < m_parameterizations.size(); ++i )
    {
        m_parameterizations[ i ]->newSegment( start, end );
    }
}

void WRasterAlgorithm::parameterizeVoxel( const WValue< int >& voxel, size_t voxelIdx, const int axis, const double value,
                                    const WPosition& start,
                                    const WPosition& end )
{
    // NOTE: the list already is locked (in raster method, hopefully)
    for( size_t i = 0; i < m_parameterizations.size(); ++i )
    {
        m_parameterizations[ i ]->parameterizeVoxel( voxel, voxelIdx, axis, value, start, end );
    }
}

void WRasterAlgorithm::finished()
{
    // lock the parameterization list for reading
    boost::shared_lock< boost::shared_mutex > lock =  boost::shared_lock< boost::shared_mutex >( m_parameterizationsLock );

    // NOTE: the list already is locked (in raster method, hopefully)
    for( size_t i = 0; i < m_parameterizations.size(); ++i )
    {
        m_parameterizations[ i ]->finished();
    }

    lock.unlock();
}

