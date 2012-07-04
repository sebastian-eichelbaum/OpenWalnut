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
        boost::shared_ptr< WGrid > grid = boost::shared_ptr< WGrid >( new WGridRegular3D( 5, 3, 3 ) );
        boost::shared_ptr< std::vector< double > > data = boost::shared_ptr< std::vector< double > >( new std::vector< double >( grid->size() ) );
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
        boost::shared_ptr< std::vector< double > > data = boost::shared_ptr< std::vector< double > >( new std::vector< double >( grid->size() ) );
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
};

#endif  // WDATASETSCALAR_TEST_H
