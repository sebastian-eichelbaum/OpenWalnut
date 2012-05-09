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

#include <limits>
#include <string>
#include <vector>

#include "../common/WLimits.h"
#include "WThreadedTrackingFunction.h"

namespace wtracking
{
    bool WTrackingUtility::followToNextVoxel( DataSetPtr dataset, JobType& job, DirFunc const& dirFunc )
    {
        // TODO(reichenbach): Give those WAsserts correct strings incase they occur
        WAssert( dataset->getGrid(), "" );
        Grid3DPtr g = boost::shared_dynamic_cast< WGridRegular3D >( dataset->getGrid() );
        WAssert( g, "" );
        WAssert( g->encloses( job.first ), "" );

        // find matching direction
        WVector3d dir = dirFunc( dataset, job );

        if( fabs( length( dir ) - 1.0 ) > TRACKING_EPS )
        {
            return false;
        }

        // find t such that job.first() + t * dir is a point on the boundary of the current voxel
        double t = getDistanceToBoundary( g, job.first, dir );
        WAssert( !wlimits::isInf( t ) && !wlimits::isNaN( t ), "Warning in WTrackingUtility::followToNextVoxel NaN's or INF's occured" );
        WAssert( t > 0.0, "" );
        WAssert( onBoundary( g, job.first + dir * t ), "" );

        if( !g->encloses( job.first + dir * t ) )
        {
            return false;
        }

        // increase t, so that job.first() + t * dir does not lie on any boundary anymore
        // ( so we can find the corresponding voxel without ambiguities )
        // ( this also means the resulting new position will never be in the same
        //   voxel as the starting position )
        int i = 0;
        while( onBoundary( g, job.first + dir * t ) && i < 50 )
        {
            t += TRACKING_EPS;
            if( !g->encloses( job.first + dir * t ) )
            {
                return false;
            }
            ++i;
        }
        if( i == 50 )
        {
            return false;
        }

        // set the new position
        job.first += t * dir;
        job.second = dir;
        return true;
    }

    bool WTrackingUtility::onBoundary( Grid3DPtr grid, WVector3d const& pos )
    {
        WVector3d v = pos - grid->getOrigin();
        double p;
        double z;
        WVector3d d = grid->getDirectionX();
        d = normalize( d );
        double c = dot( d, v ) / grid->getOffsetX() + 0.5;
        p = c >= 0.0 ? modf( c, &z ) : 1.0 + modf( c, &z );
        if( fabs( p ) < TRACKING_EPS || fabs( p - 1 ) < TRACKING_EPS )
        {
            return true;
        }
        d = grid->getDirectionY();
        d = normalize( d );
        c = dot( d,  v ) / grid->getOffsetY() + 0.5;
        p = c >= 0.0 ? modf( c, &z ) : 1.0 + modf( c, &z );
        if( fabs( p ) < TRACKING_EPS || fabs( p - 1 ) < TRACKING_EPS )
        {
            return true;
        }
        d = grid->getDirectionZ();
        d = normalize( d );
        c = dot( d, v ) / grid->getOffsetZ() + 0.5;
        p = c >= 0.0 ? modf( c, &z ) : 1.0 + modf( c, &z );
        if( fabs( p ) < TRACKING_EPS || fabs( p - 1 ) < TRACKING_EPS )
        {
            return true;
        }
        return false;
    }

    double WTrackingUtility::getDistanceToBoundary( Grid3DPtr grid, WVector3d const& pos, WVector3d const& dir )
    {
        // the input pos is at least TRACKING_EPS away from the boundary
        // so distance calculations don't get screwed
        WAssert( !onBoundary( grid, pos ), "" );

        double dist = std::numeric_limits< double >::infinity();

        WVector3d v = pos - grid->getOrigin();
        WVector3d p;
        double z;
        WVector3d o( grid->getOffsetX(), grid->getOffsetY(), grid->getOffsetZ() );
        WVector3d s[ 3 ] = { grid->getDirectionX() / o[ 0 ], grid->getDirectionY() / o[ 1 ], grid->getDirectionZ() / o[ 2 ] };
        double c = dot( s[ 0 ], v ) / o[ 0 ] + 0.5;
        p[ 0 ] = c >= 0.0 ? modf( c, &z ) : 1.0 + modf( c, &z );
        c = dot( s[ 1 ], v ) / o[ 1 ] + 0.5;
        p[ 1 ] = c >= 0.0 ? modf( c, &z ) : 1.0 + modf( c, &z );
        c = dot( s[ 2 ], v ) / o[ 2 ] + 0.5;
        p[ 2 ] = c >= 0.0 ? modf( c, &z ) : 1.0 + modf( c, &z );
        WVector3d d( dot( dir, s[ 0 ] ), dot( dir, s[ 1 ] ), dot( dir, s[ 2 ] ) );

        double t;
        for( int i = 0; i < 3; ++i )
        {
            WAssert( -TRACKING_EPS < p[ i ] && p[ i ] < o[ i ] + TRACKING_EPS, "" );
            for( double j = 0.0; j <= 1.0; j += 1.0 )
            {
                if( fabs( d[ i ] ) > TRACKING_EPS )
                {
                    t = ( j - p[ i ] ) * o[ i ] / d[ i ];
                    if( t > 0.0 && t < dist )
                    {
                        dist = t;
                    }
                }
            }
        }
        // distance absolute must be at least TRACKING_EPS!
        WAssert( dist >= TRACKING_EPS, "" );
        return dist;
    }

    //////////////////////////////////////////////////////////////////////////////////////////

    WThreadedTrackingFunction::WThreadedTrackingFunction( DataSetPtr dataset, DirFunc dirFunc,
            NextPositionFunc nextFunc,
            FiberVisitorFunc fiberVst, PointVisitorFunc pointVst,
            std::size_t seedPositions, std::size_t seedsPerPos,
            std::vector< int > v0, std::vector< int > v1 )
        : Base( dataset ),
        m_grid( boost::shared_dynamic_cast< GridType >( dataset->getGrid() ) ),
        m_directionFunc( dirFunc ),
        m_nextPosFunc( nextFunc ),
        m_fiberVisitor( fiberVst ),
        m_pointVisitor( pointVst ),
        m_maxPoints(),
        m_currentIndex()
    {
        // dataset != 0 is tested by the base constructor
        if( !m_grid )
        {
            throw WException( std::string( "Cannot find WGridRegular3D. Are you sure the dataset has the correct grid type?" ) );
        }

        m_maxPoints = static_cast< std::size_t >( 5 * pow( static_cast< double >( m_grid->size() ), 1.0 / 3.0 ) );

        m_currentIndex.getWriteTicket()->get() = IndexType( m_grid, v0, v1, seedPositions, seedsPerPos );
    }

    WThreadedTrackingFunction::~WThreadedTrackingFunction()
    {
    }

    bool WThreadedTrackingFunction::getJob( JobType& job )  // NOLINT
    {
        WSharedObject< IndexType >::WriteTicket t = m_currentIndex.getWriteTicket();
        if( t->get().done() )
        {
            return false;
        }
        else
        {
            job = t->get().job();
            ++t->get();
            return true;
        }
    }

    void WThreadedTrackingFunction::compute( DataSetPtr input, JobType const& job )
    {
        WVector3d e = m_directionFunc( input, job );
        JobType j = job;
        j.second = e;

        std::vector< WVector3d > fiber;
        if( fabs( length( e ) - 1.0 ) > TRACKING_EPS )
        {
            if( m_fiberVisitor )
            {
                m_fiberVisitor( fiber );
            }
            return;
        }

        fiber.push_back( j.first );
        if( m_pointVisitor )
        {
            m_pointVisitor( j.first );
        }

        // forward integration
        for( std::size_t k = 0; k < m_maxPoints; ++k )
        {
            if( fabs( length( j.second ) - 1.0 ) > TRACKING_EPS )
            {
                break;
            }
            if( !m_nextPosFunc( input, j, m_directionFunc ) )
            {
                break;
            }
            fiber.push_back( j.first );
            if( m_pointVisitor )
            {
                m_pointVisitor( j.first );
            }
        }

        // backward integration
        std::reverse( fiber.begin(), fiber.end() );
        j = job;
        j.second = e * -1.0;
        for( std::size_t k = 0; k < m_maxPoints; ++k )
        {
            if( fabs( length( j.second ) - 1.0 ) > TRACKING_EPS )
            {
                break;
            }
            if( !m_nextPosFunc( input, j, m_directionFunc ) )
            {
                break;
            }
            fiber.push_back( j.first );
            if( m_pointVisitor )
            {
                m_pointVisitor( j.first );
            }
        }

        // output result
        if( m_fiberVisitor )
        {
            m_fiberVisitor( fiber );
        }
    }

    WThreadedTrackingFunction::IndexType::IndexType()
        :   m_grid(),
        m_done( true )
    {
    }

    WThreadedTrackingFunction::IndexType::IndexType( GridPtr grid, std::vector< int > const& v0,
            std::vector< int > const& v1, std::size_t seedPositions,
            std::size_t seedsPerPosition )
        : m_grid( grid ),
        m_done( false )
    {
        if( v0.size() != 3 )
        {
            m_min[ 0 ] = m_min[ 1 ] = m_min[ 2 ] = seedPositions;
        }
        else
        {
            WAssert( v0[ 0 ] >= 1 && static_cast< std::size_t >( v0[ 0 ] ) < m_grid->getNbCoordsX() - 1, "" );
            WAssert( v0[ 1 ] >= 1 && static_cast< std::size_t >( v0[ 1 ] ) < m_grid->getNbCoordsY() - 1, "" );
            WAssert( v0[ 2 ] >= 1 && static_cast< std::size_t >( v0[ 2 ] ) < m_grid->getNbCoordsZ() - 1, "" );
            for( int i = 0; i < 3; ++i )
            {
                m_min[ i ] = v0[ i ] * seedPositions;
            }
        }
        if( v1.size() != 3 )
        {
            m_max[ 0 ] = ( m_grid->getNbCoordsX() - 1 ) * seedPositions;
            m_max[ 1 ] = ( m_grid->getNbCoordsY() - 1 ) * seedPositions;
            m_max[ 2 ] = ( m_grid->getNbCoordsZ() - 1 ) * seedPositions;
        }
        else
        {
            WAssert( v1[ 0 ] > v0[ 0 ] && static_cast< std::size_t >( v1[ 0 ] ) < grid->getNbCoordsX(), "" );
            WAssert( v1[ 1 ] > v0[ 1 ] && static_cast< std::size_t >( v1[ 1 ] ) < grid->getNbCoordsY(), "" );
            WAssert( v1[ 2 ] > v0[ 2 ] && static_cast< std::size_t >( v1[ 2 ] ) < grid->getNbCoordsZ(), "" );
            for( int i = 0; i < 3; ++i )
            {
                m_max[ i ] = v1[ i ] * seedPositions;
            }
        }
        m_offset = 1.0 / seedPositions;
        m_min[ 3 ] = 0;
        m_max[ 3 ] = seedsPerPosition;
        m_pos[ 0 ] = m_min[ 0 ];
        m_pos[ 1 ] = m_min[ 1 ];
        m_pos[ 2 ] = m_min[ 2 ];
        m_pos[ 3 ] = 0;
    }

    WThreadedTrackingFunction::IndexType& WThreadedTrackingFunction::IndexType::operator++ ()
    {
        if( !m_done )
        {
            for( int i = 3; i > -1; --i )
            {
                ++m_pos[ i ];
                if( m_pos[ i ] == m_max[ i ] )
                {
                    m_pos[ i ] = m_min[ i ];
                    m_done = i == 0;
                }
                else
                {
                    break;
                };
            }
        }
        return *this;
    }

    bool WThreadedTrackingFunction::IndexType::done()
    {
        return m_done;
    }

    WThreadedTrackingFunction::JobType WThreadedTrackingFunction::IndexType::job()
    {
        JobType job;
        job.second = WVector3d( 0.0, 0.0, 0.0 );
        job.first = m_grid->getOrigin() + m_grid->getDirectionX() * ( m_offset * ( 0.5 + m_pos[ 0 ] ) - 0.5 )
            + m_grid->getDirectionY() * ( m_offset * ( 0.5 + m_pos[ 1 ] ) - 0.5 )
            + m_grid->getDirectionZ() * ( m_offset * ( 0.5 + m_pos[ 2 ] ) - 0.5 );
        return job;
    }

}  // namespace wtracking
