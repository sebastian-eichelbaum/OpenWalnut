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

#ifndef WDATASETSCALAR_TEST_H
#define WDATASETSCALAR_TEST_H

#include <vector>

#include <cxxtest/TestSuite.h>

#include <boost/random.hpp>

#include "../../common/WLogger.h"

#include "../WDataSetScalar.h"

/**
 * Tests for the data set type containing only scalars.
 */
class WDataSetScalarTest : public CxxTest::TestSuite
{
public:
    /**
     * Setup logger and other stuff for each test.
     */
    void setUp()
    {
        WLogger::startup();
    }

    /**
     * Test if the interpolate function works reasonable.
     */
    void testInterpolate( void )
    {
        // create dummies, since they are needed in almost every test
        boost::shared_ptr< WGrid > grid( new WGridRegular3D( 5, 3, 3 ) );
        boost::shared_ptr< std::vector< double > > data( new std::vector< double >( grid->size() ) );
        for( size_t i = 0; i < grid->size(); ++i )
        {
            ( *data )[i] = i;
        }
        boost::shared_ptr< WValueSet< double > > valueSet( new WValueSet< double >( 0, 1, data, W_DT_DOUBLE ) );
        WDataSetScalar ds( valueSet, grid );

        bool success = false;

        TS_ASSERT_EQUALS( ds.interpolate( WPosition::zero(), &success ), ( *data )[0] );
        TS_ASSERT( success );
        TS_ASSERT_DELTA( ds.interpolate( WPosition( 1, 0, 0 ), &success ), ( *data )[1], 1e-9 );
        TS_ASSERT( success );
        TS_ASSERT_DELTA( ds.interpolate( WPosition( 0, 1, 0 ), &success ), ( *data )[5], 1e-9 );
        TS_ASSERT( success );
        TS_ASSERT_DELTA( ds.interpolate( WPosition( 1, 1, 0 ), &success ), ( *data )[6], 1e-9 );
        TS_ASSERT( success );
        TS_ASSERT_DELTA( ds.interpolate( WPosition( 0, 0, 1 ), &success ), ( *data )[15], 1e-9 );
        TS_ASSERT( success );
        TS_ASSERT_DELTA( ds.interpolate( WPosition( 1, 0, 1 ), &success ), ( *data )[16], 1e-9 );
        TS_ASSERT( success );
        TS_ASSERT_DELTA( ds.interpolate( WPosition( 0, 1, 1 ), &success ), ( *data )[20], 1e-9 );
        TS_ASSERT( success );
        TS_ASSERT_DELTA( ds.interpolate( WPosition( 1, 1, 1 ), &success ), ( *data )[21], 1e-9 );
        TS_ASSERT( success );

        TS_ASSERT_DELTA( ds.interpolate( WPosition( 0.3, 0.4, 0.5 ), &success ), 9.8, 1e-9 );
        TS_ASSERT( success );
        TS_ASSERT_DELTA( ds.interpolate( WPosition( 0.5, 0.5, 0.5 ), &success ), 10.5, 1e-9 );
        TS_ASSERT( success );
    }

    /**
     * Test if the interpolate function works also for rotated grids reasonable.
     */
    void testInterpolateInRotatedGrid( void )
    {
        // rotation around z with 45 degrees
        WMatrix< double > mat( 4, 4 );
        mat.makeIdentity();
        mat( 0, 0 ) =  1.0 / sqrt( 2.0 );
        mat( 0, 1 ) =  1.0 / sqrt( 2.0 );
        mat( 1, 0 ) = -1.0 / sqrt( 2.0 );
        mat( 1, 1 ) =  1.0 / sqrt( 2.0 );

        WGridTransformOrtho v( mat );

        boost::shared_ptr< WGridRegular3D > grid( new WGridRegular3D( 5, 3, 3, v ) );
        boost::shared_ptr< std::vector< double > > data( new std::vector< double >( grid->size() ) );
        for( size_t i = 0; i < grid->size(); ++i )
        {
            ( *data )[i] = i;
        }
        boost::shared_ptr< WValueSet< double > > valueSet( new WValueSet< double >( 0, 1, data, W_DT_DOUBLE ) );
        WDataSetScalar ds( valueSet, grid );

        bool success = false;

        TS_ASSERT_EQUALS( ds.interpolate( WPosition::zero(), &success ), ( *data )[0] );
        TS_ASSERT( success );
        TS_ASSERT_DELTA( ds.interpolate( grid->getTransform().positionToWorldSpace( WPosition( 1, 0, 0 ) ), &success ), ( *data )[1], 1e-9 );
        TS_ASSERT( success );
        TS_ASSERT_DELTA( ds.interpolate( grid->getTransform().positionToWorldSpace( WPosition( 0, 1, 0 ) ), &success ), ( *data )[5], 1e-9 );
        TS_ASSERT( success );
        TS_ASSERT_DELTA( ds.interpolate( grid->getTransform().positionToWorldSpace( WPosition( 1, 1, 0 ) ), &success ), ( *data )[6], 1e-9 );
        TS_ASSERT( success );
        TS_ASSERT_DELTA( ds.interpolate( grid->getTransform().positionToWorldSpace( WPosition( 0, 0, 1 ) ), &success ), ( *data )[15], 1e-9 );
        TS_ASSERT( success );
        TS_ASSERT_DELTA( ds.interpolate( grid->getTransform().positionToWorldSpace( WPosition( 1, 0, 1 ) ), &success ), ( *data )[16], 1e-9 );
        TS_ASSERT( success );
        TS_ASSERT_DELTA( ds.interpolate( grid->getTransform().positionToWorldSpace( WPosition( 0, 1, 1 ) ), &success ), ( *data )[20], 1e-9 );
        TS_ASSERT( success );
        TS_ASSERT_DELTA( ds.interpolate( grid->getTransform().positionToWorldSpace( WPosition( 1, 1, 1 ) ), &success ), ( *data )[21], 1e-9 );
        TS_ASSERT( success );

        TS_ASSERT_DELTA( ds.interpolate( grid->getTransform().positionToWorldSpace( WPosition( 0.3, 0.4, 0.5 ) ), &success ), 9.8, 1e-9 );
        TS_ASSERT( success );
        TS_ASSERT_DELTA( ds.interpolate( grid->getTransform().positionToWorldSpace( WPosition( 0.5, 0.5, 0.5 ) ), &success ), 10.5, 1e-9 );
        TS_ASSERT( success );
    }

    /**
     * Check whether interpolation works for a translated dataset.
     */
    void testTranslatedGridInterpolation()
    {
        boost::random::mt19937 rng;
        boost::random::uniform_int_distribution<> urnd( 3, 20 );
        boost::random::uniform_real_distribution<> drnd( -1000.0, +1000.0 );

        WMatrix< double > mat( 4, 4 );
        mat.makeIdentity();
        mat( 0, 0 ) = 1.7;
        mat( 1, 1 ) = 1.7;
        mat( 2, 2 ) = 1.7;

        std::size_t sx = urnd( rng );
        std::size_t sy = urnd( rng );
        std::size_t sz = urnd( rng );

        boost::random::uniform_real_distribution<> prndx( 0.0, ( sx - 1.000000001 ) * mat( 0, 0 ) );
        boost::random::uniform_real_distribution<> prndy( 0.0, ( sy - 1.000000001 ) * mat( 1, 1 ) );
        boost::random::uniform_real_distribution<> prndz( 0.0, ( sz - 1.000000001 ) * mat( 2, 2 ) );

        WGridTransformOrtho v( mat );
        boost::shared_ptr< WGridRegular3D > grid( new WGridRegular3D( sx, sy, sz, v ) );
        boost::shared_ptr< std::vector< double > > data( new std::vector< double >( grid->size() ) );

        for( std::size_t k = 0; k < grid->size(); ++k )
        {
            data->at( k ) = drnd( rng );
        }

        boost::shared_ptr< WValueSet< double > > valueSet( new WValueSet< double >( 0, 1, data, W_DT_DOUBLE ) );
        WDataSetScalar ds( valueSet, grid );

        for( std::size_t k = 0; k < 1000; ++k )
        {
            WMatrix< double > m = mat;

            m( 0, 3 ) = drnd( rng );
            m( 1, 3 ) = drnd( rng );
            m( 2, 3 ) = drnd( rng );

            WGridTransformOrtho t( m );
            boost::shared_ptr< WGridRegular3D > tGrid( new WGridRegular3D( sx, sy, sz, t ) );
            boost::shared_ptr< WValueSet< double > > tValueSet( new WValueSet< double >( 0, 1, data, W_DT_DOUBLE ) );
            WDataSetScalar tds( tValueSet, tGrid );

            // test random positions in the dataset
            for( std::size_t i = 0; i < 100; ++i )
            {
                WVector3d p( prndx( rng ), prndy( rng ), prndz( rng ) );
                WVector3d q( p[ 0 ] + m( 0, 3 ), p[ 1 ] + m( 1, 3 ), p[ 2 ] + m( 2, 3 ) );

                bool s1, s2;
                TS_ASSERT_DELTA( ds.interpolate( p, &s1 ), tds.interpolate( q, &s2 ), 1e-9 );
                TS_ASSERT( s1 );
                TS_ASSERT( s2 );
            }
        }
    }
};

#endif  // WDATASETSCALAR_TEST_H
