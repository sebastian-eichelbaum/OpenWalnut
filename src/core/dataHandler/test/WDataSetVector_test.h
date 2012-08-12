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
#include <cmath>

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
        boost::shared_ptr< WGridRegular3D > grid( new WGridRegular3D( 2, 2, 2 ) );
        boost::array< WPosition, 8 > d = { { WPosition(  0, 1, 2 ), WPosition(  3, 4, 5 ), WPosition(  6, 7, 8 ), // NOLINT braces
            WPosition(  9,10,11 ), WPosition( 12,13,14 ), WPosition( 15,16,17 ), WPosition( 18,19,20 ), WPosition( 21,22,23 ) } }; // NOLINT braces

        boost::shared_ptr< std::vector< double > > data( new std::vector< double > );
        for( size_t i = 0; i < grid->size() * 3; ++i )
        {
            data->push_back( i );
        }

        double almost1 = 1 - wlimits::DBL_EPS;
        boost::array< WPosition, 8 > gridPos = { { WPosition( 0, 0, 0 ), WPosition( almost1, 0, 0 ), WPosition( 0, almost1, 0 ), // NOLINT braces
            WPosition( almost1, almost1, 0 ), WPosition( 0, 0, almost1 ), WPosition(  almost1, 0, almost1 ),
            WPosition( 0, almost1, almost1 ), WPosition(  almost1, almost1, almost1 ) } }; // NOLINT braces

        boost::shared_ptr< WValueSet< double > > valueSet( new WValueSet< double >( 1, 3, data, W_DT_DOUBLE ) );
        WDataSetVector ds( valueSet, grid );

        bool success = false;
        for( size_t i = 0; i < 8; ++i )
        {
            if( !delta( ds.interpolate( gridPos[i], &success ), d[i], 1e-9 ) )
            {
                std::stringstream ss;
                ss << "i:" << i << " gridPos(i):" << gridPos[i] << " d(i):" << d[i] << " interpol:" << ds.interpolate( gridPos[i], &success ) << "\n";
                TS_FAIL( ss.str() );
            }
            TS_ASSERT( success );
        }

        TS_ASSERT( delta( ds.interpolate( WPosition( 0.3, 0.4, 0.5 ), &success ), WPosition( 9.3, 10.3, 11.3 ), 1e-9 ) );
        TS_ASSERT( success );
        TS_ASSERT( delta( ds.interpolate( WPosition( 0.5, 0.5, 0.5 ), &success ), WPosition( 10.5, 11.5, 12.5 ), 1e-9 ) );
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
        TS_ASSERT( delta( ds.interpolate( WPosition( 0.9999, 0.9999, 0.9999 ), &success ), d[7], 1e-9 ) );
        TS_ASSERT( success );
        TS_ASSERT_EQUALS( ds.interpolate( WPosition( 0.5, 0.5, 0.5 ), &success ), WPosition( 0.75, 0.0, 0.0 ) );
        TS_ASSERT( success );
        TS_ASSERT_EQUALS( ds.eigenVectorInterpolate( WPosition( 0.0, 0.0, 0.0 ), &success ), d[0] );
        TS_ASSERT( success );
        TS_ASSERT( delta( ds.eigenVectorInterpolate( WPosition( 0.9999, 0.9999, 0.9999 ), &success ), WPosition( -1.0, 0.0, 0.0 ), 1e-9 ) );
        TS_ASSERT( success );
        TS_ASSERT_EQUALS( ds.eigenVectorInterpolate( WPosition( 0.5, 0.5, 0.5 ), &success ), WPosition( -1.0, 0.0, 0.0 ) );
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

    /**
     * When the grid for this dataset was rotated the interpolation should still work.
     */
    void testRotatedGridInterpolate( void )
    {
        // rotation around z with 45 degrees
        WMatrix< double > mat( 4, 4 );
        mat.makeIdentity();
        mat( 0, 0 ) =  1.0 / sqrt( 2.0 );
        mat( 0, 1 ) =  1.0 / sqrt( 2.0 );
        mat( 1, 0 ) = -1.0 / sqrt( 2.0 );
        mat( 1, 1 ) =  1.0 / sqrt( 2.0 );

        WGridTransformOrtho v( mat );

        boost::shared_ptr< WGridRegular3D > grid( new WGridRegular3D( 2, 2, 2, v ) );
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
        WPosition pos = grid->getTransform().positionToWorldSpace( WPosition( 0.0, 0.0, 0.0 ) );
        TS_ASSERT_EQUALS( ds.interpolate( pos, &success ), d[0] );
        TS_ASSERT( success );
        pos = grid->getTransform().positionToWorldSpace( WPosition( 0.9999, 0.9999, 0.9999 ) );
        TS_ASSERT( delta( ds.interpolate( pos, &success ), d[7], 1e-9 ) );
        TS_ASSERT( success );
        pos = grid->getTransform().positionToWorldSpace( WPosition( 0.5, 0.5, 0.5 ) );
        TS_ASSERT_EQUALS( ds.interpolate( pos, &success ), WPosition( 0.75, 0.0, 0.0 ) );
        TS_ASSERT( success );
        pos = grid->getTransform().positionToWorldSpace( WPosition( 0.0, 0.0, 0.0 ) );
        TS_ASSERT_EQUALS( ds.eigenVectorInterpolate( pos, &success ), d[0] );
        TS_ASSERT( success );
        pos = grid->getTransform().positionToWorldSpace( WPosition( 0.9999, 0.9999, 0.9999 ) );
        TS_ASSERT( delta( ds.eigenVectorInterpolate( pos, &success ), WPosition( -1.0, 0.0, 0.0 ), 1e-9 ) );
        TS_ASSERT( success );
        pos = grid->getTransform().positionToWorldSpace( WPosition( 0.5, 0.5, 0.5 ) );
        TS_ASSERT_EQUALS( ds.eigenVectorInterpolate( pos, &success ), WPosition( -1.0, 0.0, 0.0 ) );
        TS_ASSERT( success );
    }

private:
    /**
     * Computes if both vectors are almost similar and their components do not differ from a certain given delta.
     *
     * \param lhs First vector
     * \param rhs Second vector
     * \param d The given delta
     *
     * \return True if and only if all components differing at most by the given delta.
     */
    bool delta( WVector3d lhs, WVector3d rhs, double d )
    {
        bool result = true;
        for( int i = 0; result && ( i < 3 ); ++i )
        {
            result = result && ( std::abs( lhs[i] - rhs[i] ) <= d );
            if( !result )
            {
                std::cout.precision( 10 );
                std::cout.setf( std::ios::fixed, std::ios::floatfield );
                std::cout << "delta failed! => lhs:" << lhs << " rhs:" << rhs << " failed: abs(lhs[" << i << "] - rhs["
                    << i << "])=" << std::abs( lhs[i] - rhs[i] ) << ", but should be: " << d << "\n";
            }
        }
        return result;
    }
};

#endif  // WDATASETVECTOR_TEST_H
