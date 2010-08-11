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

#include <fstream>
#include <vector>

#include "../../common/WAssert.h"
#include "../../common/math/WMatrix.h"
#include "../../common/math/WLinearAlgebraFunctions.h"

#include "WMoriThread.h"


WMoriThread::WFiberAccumulator::WFiberAccumulator()
    : m_fiberMutex(),
      m_points( new std::vector< float >() ),
      m_fiberIndices( new std::vector< size_t >() ),
      m_fiberLengths( new std::vector< size_t >() ),
      m_pointToFiber( new std::vector< size_t >() )
{
}

WMoriThread::WFiberAccumulator::~WFiberAccumulator()
{
}

void WMoriThread::WFiberAccumulator::add( std::vector< wmath::WVector3D > const& in )
{
    boost::unique_lock< boost::mutex > lock( m_fiberMutex );

    if( in.size() > 0 )
    {
        m_fiberIndices->push_back( m_points->size() / 3 );
        m_fiberLengths->push_back( in.size() );

        for( size_t k = 0; k < in.size(); ++k )
        {
            m_points->push_back( in[ k ][ 0 ] );
            m_points->push_back( in[ k ][ 1 ] );
            m_points->push_back( in[ k ][ 2 ] );

            m_pointToFiber->push_back( m_fiberIndices->size() - 1 );
        }
    }
}

boost::shared_ptr< WDataSetFibers > WMoriThread::WFiberAccumulator::buildDataSet()
{
    boost::unique_lock< boost::mutex > lock( m_fiberMutex );

    boost::shared_ptr< WDataSetFibers > res = boost::shared_ptr< WDataSetFibers >( new WDataSetFibers( m_points, m_fiberIndices,
                                                                                                       m_fiberLengths, m_pointToFiber ) );

    m_points = boost::shared_ptr< std::vector< float > >( new std::vector< float >() );
    m_fiberIndices = boost::shared_ptr< std::vector< size_t > >( new std::vector< size_t >() );
    m_fiberLengths = boost::shared_ptr< std::vector< size_t > >( new std::vector< size_t >() );
    m_pointToFiber = boost::shared_ptr< std::vector< size_t > >( new std::vector< size_t >() );

    return res;
}

WMoriThread::WMoriThread( boost::shared_ptr< WGridRegular3D > grid,
                          boost::shared_ptr< WDataSetSingle > eigenField,
                          double const minFA,
                          unsigned int const minPoints,
                          double const minCos,
                          boost::shared_ptr< WProgress > progr,
                          uint32_t const index,
                          boost::shared_ptr< WFiberAccumulator > fiberAccu )
    : m_grid( grid ),
      m_eigenSet( boost::shared_dynamic_cast< WValueSet< double > >( eigenField->getValueSet() ) ),
      m_minFA( minFA ),
      m_minPoints( minPoints ),
      m_minCos( minCos ),
      m_progress( progr ),
      m_index( index ),
      m_fiberAccu( fiberAccu ),
      m_eps( 0.00000001 ),
      m_gridSize( 3 ),
      m_gridTransform( 3, 3 ),
      m_invGridTransform( 3, 3 )
{
    WAssert( m_eigenSet, "" );

    m_gridSize[ 0 ] = m_grid->getNbCoordsX();
    m_gridSize[ 1 ] = m_grid->getNbCoordsY();
    m_gridSize[ 2 ] = m_grid->getNbCoordsZ();

    wmath::WMatrix< double > mTemp = m_grid->getTransformationMatrix();

    m_gridTransform( 0, 0 ) = mTemp( 0, 0 );
    m_gridTransform( 0, 1 ) = mTemp( 0, 1 );
    m_gridTransform( 0, 2 ) = mTemp( 0, 2 );
    m_gridTransform( 1, 0 ) = mTemp( 1, 0 );
    m_gridTransform( 1, 1 ) = mTemp( 1, 1 );
    m_gridTransform( 1, 2 ) = mTemp( 1, 2 );
    m_gridTransform( 2, 0 ) = mTemp( 2, 0 );
    m_gridTransform( 2, 1 ) = mTemp( 2, 1 );
    m_gridTransform( 2, 2 ) = mTemp( 2, 2 );

    m_invGridTransform = wmath::invertMatrix3x3( m_gridTransform );
    m_gridTranslation = wmath::WVector3D( mTemp( 0, 3 ), mTemp( 1, 3 ), mTemp( 2, 3 ) );
}

WMoriThread::~WMoriThread()
{
}

void WMoriThread::threadMain()
{
    size_t sx, tx, sy, ty, sz, tz;

    sx = ( m_index & 1u ? m_gridSize[ 0 ] / 2 : 1 );
    tx = ( m_index & 1u ? m_gridSize[ 0 ] - 1 : m_gridSize[ 0 ] / 2 );
    sy = ( m_index & 2u ? m_gridSize[ 1 ] / 2 : 1 );
    ty = ( m_index & 2u ? m_gridSize[ 1 ] - 1 : m_gridSize[ 1 ] / 2 );
    sz = ( m_index & 4u ? m_gridSize[ 2 ] / 2 : 1 );
    tz = ( m_index & 4u ? m_gridSize[ 2 ] - 1 : m_gridSize[ 2 ] / 2 );

    wmath::WValue< size_t > coords( 3 );

    for( coords[ 2 ] = sz; coords[ 2 ] < tz; coords[ 2 ] += 2 )
    {
        for( coords[ 1 ] = sy; coords[ 1 ] < ty; coords[ 1 ] += 2 )
        {
            for( coords[ 0 ] = sx; coords[ 0 ] < tx; coords[ 0 ] += 2 )
            {
                ++*m_progress;

                std::vector< wmath::WVector3D > vf;
                trackFiber( coords, vf );
                if( vf.size() > 0 )
                {
                    m_fiberAccu->add( vf );
                }
            }
        }
    }
}

void WMoriThread::trackFiber( wmath::WValue< size_t > const& coords, std::vector< wmath::WVector3D >& points ) const
{
    wmath::WVector3D pos( coords[ 0 ], coords[ 1 ], coords[ 2 ] );

    wmath::WVector3D e = getEigenAt( coords );

    if( e.norm() < m_eps )
    {
        return;
    }

    e = wmath::multMatrixWithVector3D( m_invGridTransform, e );
    e /= e.norm();

    points.push_back( pos );

    trackFiberRec( pos, e, 1, coords, points, 200 );
    trackFiberRec( pos, e * -1.0, -1, coords, points, 200 );

    if( points.size() < m_minPoints )
    {
        points.clear();
    }

    for( std::vector< wmath::WVector3D >::iterator it = points.begin(); it != points.end(); ++it )
    {
        *it = wmath::multMatrixWithVector3D( m_gridTransform, *it ) + m_gridTranslation;
    }
}

void WMoriThread::trackFiberRec( wmath::WVector3D const& pos,
                                 wmath::WVector3D const& eigen,
                                 int dir,
                                 wmath::WValue< size_t > const& coords,
                                 std::vector< wmath::WVector3D > & points,
                                 size_t const iter ) const
{
    if( iter == 0 )
    {
        return;
    }

    if( !testFA( coords ) )
    {
        return;
    }

    wmath::WVector3D p = getNextPosition( pos, coords, eigen );

    if( dir == 1 )
    {
        if( ( points.back() - pos ).norm() > m_eps )
        {
            points.push_back( p );
        }
    }
    else
    {
        if( ( points.front() - pos ).norm() > m_eps )
        {
            points.insert( points.begin(), p );
        }
    }

    wmath::WValue< size_t > newCoords( 3 );
    if( getNextVoxel( p, coords, newCoords ) )
    {
        clamp( p, newCoords );

        wmath::WVector3D e = getEigenAt( newCoords );

        if( e.norm() < m_eps )
        {
            return;
        }

        WAssert( !( e.norm() < m_eps ), "" );

        e = wmath::multMatrixWithVector3D( m_invGridTransform, e );
        e /= e.norm();

        if( e.dotProduct( eigen ) < m_minCos )
        {
            e *= -1.0;

            if( e.dotProduct( eigen ) < m_minCos )
            {
                return;
            }
        }

        for( int k = 0; k < 3; ++k )
        {
            if( newCoords[ k ] > coords[ k ] && e[ k ] < 0.0 )
            {
                return;
            }
            if( newCoords[ k ] < coords[ k ] && e[ k ] > -0.0 )
            {
                return;
            }
        }

        trackFiberRec( p, e, dir, newCoords, points, iter - 1 );
    }
}

void WMoriThread::clamp( wmath::WVector3D& v, wmath::WValue< size_t > const& coords ) const
{
    for( int k = 0; k < 3; ++k )
    {
        v[ k ] = ( std::min )( ( std::max )( v[ k ], -0.5 + coords[ k ] ), 0.5 + coords[ k ] );
    }
}

wmath::WVector3D WMoriThread::getNextPosition( wmath::WVector3D const& pos, wmath::WValue< size_t > const& coords,
                                               wmath::WVector3D const& eigen ) const
{
    double t = 100000.0;

    WAssert( eigen.norm() > m_eps, "" );

    for( unsigned int l = 0; l < 3; ++l )
    {
        t = ( std::min )( t, distanceToBorder( pos[ l ], coords[ l ], eigen[ l ] ) );
    }

    WAssert( t < 100000.0, "" );
    WAssert( t >= 0.0, "" );

    return pos + ( eigen * t );
}

double WMoriThread::distanceToBorder( double const& pos, size_t const& coord, double const& eigen ) const
{
    double p = pos - coord + 0.5;

    WAssert( 0.0 <= p, "" );
    WAssert( p <= 1.0, "" );

    if( fabs( eigen ) < m_eps )
    {
        return 100000000.0;
    }

    if( fabs( p ) < m_eps )
    {
        return 1.0 / eigen;
    }
    if( fabs( p - 1.0 ) < m_eps )
    {
        return -1.0 / eigen;
    }
    return ( std::max )( ( 1.0 - p ) / eigen, ( 0.0 - p ) / eigen );
}

bool WMoriThread::getNextVoxel( wmath::WVector3D const& pos,
                                wmath::WValue< size_t > const& oldCoords,
                                wmath::WValue< size_t >& newCoords ) const
{
    newCoords = oldCoords;

    wmath::WVector3D p( pos[ 0 ] - oldCoords[ 0 ] + 0.5,
                        pos[ 1 ] - oldCoords[ 1 ] + 0.5,
                        pos[ 2 ] - oldCoords[ 2 ] + 0.5 );

    for( int k = 0; k < 3; ++k )
    {
        if( fabs( p[ k ] - 1.0 ) < m_eps )
        {
            newCoords[ k ] += 1;
        }
        if( fabs( p[ k ] ) < m_eps )
        {
            newCoords[ k ] -= 1;
        }
    }

    WAssert( newCoords != oldCoords, "" );

    for( int k = 0; k < 3; ++k )
    {
        if( newCoords[ k ] < 1 || newCoords[ k ] > m_gridSize[ k ] - 1 )
        {
            return false;
        }
    }
    return true;
}

wmath::WVector3D WMoriThread::getEigenAt( wmath::WValue< size_t > const& coords ) const
{
    wmath::WVector3D res( 0.0, 0.0, 0.0 );

    for( int k = 0; k < 3; ++k )
    {
        if( coords[ k ] < 1 || coords[ k ] >= m_gridSize[ k ] - 1 )
        {
            return res;
        }
    }
    std::size_t c = coords[ 0 ] + coords[ 1 ] * m_gridSize[ 0 ] + coords[ 2 ] * m_gridSize[ 0 ] * m_gridSize[ 1 ];
    double const* ptr = m_eigenSet->rawData();
    return wmath::WVector3D( ptr[ 4 * c ], ptr[ 4 * c + 1 ], ptr[ 4 * c + 2 ] );
}

bool WMoriThread::testFA( wmath::WValue< size_t > const& coords ) const
{
    std::size_t c = coords[ 0 ] + coords[ 1 ] * m_gridSize[ 0 ] + coords[ 2 ] * m_gridSize[ 0 ] * m_gridSize[ 1 ];
    double const* ptr = m_eigenSet->rawData();
    return m_minFA < ptr[ 4 * c + 3 ];
}
