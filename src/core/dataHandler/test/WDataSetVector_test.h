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

#ifndef WDATASETVECTOR_TEST_H
#define WDATASETVECTOR_TEST_H

#include <vector>

#include <boost/array.hpp>

#include <cxxtest/TestSuite.h>

#include "../../common/WLogger.h"
#include "../WDataSetVector.h"

/**
 * Test basic functionality of WDataSetVector.
 */
class WDataSetVectorTest : public CxxTest::TestSuite
{
public:
    /**
     * Constructs unit test environment.
     */
    void setUp( void )
    {
        WLogger::startup();
    }

    /**
     * An interpolate of an vector is as if every components were interpolated
     */
    void testInterpolate( void )
    {
        boost::shared_ptr< WGrid > grid = boost::shared_ptr< WGrid >( new WGridRegular3D( 5, 3, 3 ) );
        boost::shared_ptr< std::vector< double > > data = boost::shared_ptr< std::vector< double > >( new std::vector< double >( grid->size() * 3 ) );
        for( size_t i = 0; i < grid->size() * 3; ++i )
        {
            ( *data )[i] = i;
        }
        boost::shared_ptr< WValueSet< double > > valueSet( new WValueSet< double >( 1, 3, data, W_DT_DOUBLE ) );
        WDataSetVector ds( valueSet, grid );

        bool success = false;

        TS_ASSERT_EQUALS( ds.interpolate( WPosition( 0, 0, 0 ), &success )[0], ( *data )[0] );
        TS_ASSERT_EQUALS( ds.interpolate( WPosition( 0, 0, 0 ), &success )[1], ( *data )[1] );
        TS_ASSERT_EQUALS( ds.interpolate( WPosition( 0, 0, 0 ), &success )[2], ( *data )[2] );
        TS_ASSERT( success );
        TS_ASSERT_DELTA( ds.interpolate( WPosition( 1, 0, 0 ), &success )[0], ( *data )[3], 1e-9 );
        TS_ASSERT_DELTA( ds.interpolate( WPosition( 1, 0, 0 ), &success )[1], ( *data )[4], 1e-9 );
        TS_ASSERT_DELTA( ds.interpolate( WPosition( 1, 0, 0 ), &success )[2], ( *data )[5], 1e-9 );
        TS_ASSERT( success );
        TS_ASSERT_DELTA( ds.interpolate( WPosition( 0, 1, 0 ), &success )[0], ( *data )[15], 1e-9 );
        TS_ASSERT_DELTA( ds.interpolate( WPosition( 0, 1, 0 ), &success )[1], ( *data )[16], 1e-9 );
        TS_ASSERT_DELTA( ds.interpolate( WPosition( 0, 1, 0 ), &success )[2], ( *data )[17], 1e-9 );
        TS_ASSERT( success );
        TS_ASSERT_DELTA( ds.interpolate( WPosition( 1, 1, 0 ), &success )[0], ( *data )[18], 1e-9 );
        TS_ASSERT_DELTA( ds.interpolate( WPosition( 1, 1, 0 ), &success )[1], ( *data )[19], 1e-9 );
        TS_ASSERT_DELTA( ds.interpolate( WPosition( 1, 1, 0 ), &success )[2], ( *data )[20], 1e-9 );
        TS_ASSERT( success );
        TS_ASSERT_DELTA( ds.interpolate( WPosition( 0, 0, 1 ), &success )[0], ( *data )[45], 1e-9 );
        TS_ASSERT_DELTA( ds.interpolate( WPosition( 0, 0, 1 ), &success )[1], ( *data )[46], 1e-9 );
        TS_ASSERT_DELTA( ds.interpolate( WPosition( 0, 0, 1 ), &success )[2], ( *data )[47], 1e-9 );
        TS_ASSERT( success );
        TS_ASSERT_DELTA( ds.interpolate( WPosition( 1, 0, 1 ), &success )[0], ( *data )[48], 1e-9 );
        TS_ASSERT_DELTA( ds.interpolate( WPosition( 1, 0, 1 ), &success )[1], ( *data )[49], 1e-9 );
        TS_ASSERT_DELTA( ds.interpolate( WPosition( 1, 0, 1 ), &success )[2], ( *data )[50], 1e-9 );
        TS_ASSERT( success );
        TS_ASSERT_DELTA( ds.interpolate( WPosition( 0, 1, 1 ), &success )[0], ( *data )[60], 1e-9 );
        TS_ASSERT_DELTA( ds.interpolate( WPosition( 0, 1, 1 ), &success )[1], ( *data )[61], 1e-9 );
        TS_ASSERT_DELTA( ds.interpolate( WPosition( 0, 1, 1 ), &success )[2], ( *data )[62], 1e-9 );
        TS_ASSERT( success );
        TS_ASSERT_DELTA( ds.interpolate( WPosition( 1, 1, 1 ), &success )[0], ( *data )[63], 1e-9 );
        TS_ASSERT_DELTA( ds.interpolate( WPosition( 1, 1, 1 ), &success )[1], ( *data )[64], 1e-9 );
        TS_ASSERT_DELTA( ds.interpolate( WPosition( 1, 1, 1 ), &success )[2], ( *data )[65], 1e-9 );
        TS_ASSERT( success );

        TS_ASSERT_DELTA( ds.interpolate( WPosition( 0.3, 0.4, 0.5 ), &success )[0], 29.4, 1e-9 );
        TS_ASSERT_DELTA( ds.interpolate( WPosition( 0.3, 0.4, 0.5 ), &success )[1], 30.4, 1e-9 );
        TS_ASSERT_DELTA( ds.interpolate( WPosition( 0.3, 0.4, 0.5 ), &success )[2], 31.4, 1e-9 );
        TS_ASSERT( success );
        TS_ASSERT_DELTA( ds.interpolate( WPosition( 0.5, 0.5, 0.5 ), &success )[0], 31.5, 1e-9 );
        TS_ASSERT_DELTA( ds.interpolate( WPosition( 0.5, 0.5, 0.5 ), &success )[1], 32.5, 1e-9 );
        TS_ASSERT_DELTA( ds.interpolate( WPosition( 0.5, 0.5, 0.5 ), &success )[2], 33.5, 1e-9 );
        TS_ASSERT( success );
    }

    /**
     * Checks if the reorientation of the vectors is applied in eigenVectorInterpolate().
     \verbatim
                         v_6( 1, 0, 0 )               v_7( 1, 0, 0 )
                       /----------------------------/
                 z A  /|                           /|
                   | / |                          / |
                   |/  |                         /  |
                   /---+------------------------/   |
            v_4( 1, 0, 0 )               v_5( 1, 0, 0 )
                   |   |                        |   |
                   |   |                        |   |
                   |   |                        |   |
                   |   |    y                   |   |
                   |   |   /                    |   |
                   |   |  /                     |   |
                   |   | /                      |   |
                   |   | v_2( 1, 0, 0 )         |   | v_3( 1, 0, 0 )
                   |   /------------------------+---/
                   |  /                         |  /
                   | /                          | /
                   |/                           |/
                   /----------------------------/------------------> x
            v_0( -1, 0, 0)                v_1( 1, 0, 0 )

     \endverbatim
     */
    void testEigenVectorInterpolate( void )
    {
        boost::shared_ptr< WGrid > grid = boost::shared_ptr< WGrid >( new WGridRegular3D( 2, 2, 2 ) );
        boost::shared_ptr< std::vector< double > > data( new std::vector< double > );
        boost::array< WPosition, 8 > d = { { WPosition( -1, 0, 0 ), // NOLINT braces
                                             WPosition(  1, 0, 0 ),
                                             WPosition(  1, 0, 0 ),
                                             WPosition(  1, 0, 0 ),
                                             WPosition(  1, 0, 0 ),
                                             WPosition(  1, 0, 0 ),
                                             WPosition(  1, 0, 0 ),
                                             WPosition(  1, 0, 0 ) } }; // NOLINT braces

        for( size_t i = 0; i < grid->size(); ++i )
        {
            data->push_back( d[i][0] );
            data->push_back( d[i][1] );
            data->push_back( d[i][2] );
        }
        boost::shared_ptr< WValueSet< double > > valueSet( new WValueSet< double >( 1, 3, data, W_DT_DOUBLE ) );
        WDataSetVector ds( valueSet, grid );

        bool success = false;
        TS_ASSERT_EQUALS( ds.interpolate( WPosition( 0.0, 0.0, 0.0 ), &success ), d[0] );
        TS_ASSERT( success );
        TS_ASSERT_DELTA( ds.interpolate( WPosition( 0.9999, 0.9999, 0.9999 ), &success )[0], d[7][0], 1e-9 );
        TS_ASSERT_DELTA( ds.interpolate( WPosition( 0.9999, 0.9999, 0.9999 ), &success )[1], d[7][1], 1e-9 );
        TS_ASSERT_DELTA( ds.interpolate( WPosition( 0.9999, 0.9999, 0.9999 ), &success )[2], d[7][2], 1e-9 );
        TS_ASSERT( success );
        WPosition expected( 0.75, 0.0, 0.0 );
        TS_ASSERT_EQUALS( ds.interpolate( WPosition( 0.5, 0.5, 0.5 ), &success ), expected );
        TS_ASSERT( success );
        TS_ASSERT_EQUALS( ds.eigenVectorInterpolate( WPosition( 0.0, 0.0, 0.0 ), &success ), d[0] );
        TS_ASSERT( success );
        expected = WPosition( -1.0, 0.0, 0.0 );
        TS_ASSERT_DELTA( ds.eigenVectorInterpolate( WPosition( 0.9999, 0.9999, 0.9999 ), &success )[0], expected[0], 1e-9 );
        TS_ASSERT_DELTA( ds.eigenVectorInterpolate( WPosition( 0.9999, 0.9999, 0.9999 ), &success )[1], expected[1], 1e-9 );
        TS_ASSERT_DELTA( ds.eigenVectorInterpolate( WPosition( 0.9999, 0.9999, 0.9999 ), &success )[2], expected[2], 1e-9 );
        TS_ASSERT( success );
        expected = WPosition( -1.0, 0.0, 0.0 );
        TS_ASSERT_EQUALS( ds.eigenVectorInterpolate( WPosition( 0.5, 0.5, 0.5 ), &success ), expected );
        TS_ASSERT( success );
    }

    /**
     * Using interpolate on Positions on the boundary of the grid the success flag is true but there should not be any segfaults.
     * See ticket #313 for more informations.
     */
    void testBoundary_ticket313( void )
    {
        boost::shared_ptr< WGridRegular3D > grid = boost::shared_ptr< WGridRegular3D >( new WGridRegular3D( 3, 4, 5 ) );
        bool success = false;
        boost::shared_ptr< std::vector< double > > data = boost::shared_ptr< std::vector< double > >( new std::vector< double >( grid->size() * 3 ) );
        for( size_t i = 0; i < grid->size() * 3; ++i )
        {
            ( *data )[i] = i;
        }
        boost::shared_ptr< WValueSet< double > > valueSet( new WValueSet< double >( 1, 3, data, W_DT_DOUBLE ) );
        WDataSetVector ds( valueSet, grid );
        ds.interpolate( WPosition( 2.0, 3.0, 4.0 ), &success );
        TS_ASSERT( !success );
    }
};

#endif  // WDATASETVECTOR_TEST_H
