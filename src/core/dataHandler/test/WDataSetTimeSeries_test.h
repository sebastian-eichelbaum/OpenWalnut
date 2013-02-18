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

#ifndef WDATASETTIMESERIES_TEST_H
#define WDATASETTIMESERIES_TEST_H

#include <vector>
#include <limits>
#include <algorithm>
#include <string>

#include <boost/shared_ptr.hpp>
#include <cxxtest/TestSuite.h>

#include "../../common/WLimits.h"
#include "../../common/WLogger.h"

#include "../WDataSetTimeSeries.h"


/**
 * Unit tests the time series class.
 */
class WDataSetTimeSeriesTest : public CxxTest::TestSuite
{
    //! a typedef
    typedef std::vector< boost::shared_ptr< WDataSetScalar const > > DataSetPtrVector;

    //! a typdef
    typedef std::vector< float > TimesVector;

public:
    /**
     * The input should be sorted correctly and all data should be stored correctly.
     * Also there should be only one grid for all datasets.
     */
    void testConstruction()
    {
        // test with 3 time slices
        {
            double data[] = { 1.0, 2.0, 3.0 };
            DataSetPtrVector d;
            TimesVector t;

            // test what happens when the input is empty
            TS_ASSERT_THROWS( WDataSetTimeSeries( d, t ), WException );

            // test what happens when the vector sizes don't match
            createData( data, 3, d, t );
            TS_ASSERT_THROWS_NOTHING( WDataSetTimeSeries( d, t ) );
            t.push_back( 4.0f );
            TS_ASSERT_THROWS( WDataSetTimeSeries( d, t ), WException );

            // test what happens when there is an invalid time value
            t.resize( 3 );
            TS_ASSERT_THROWS_NOTHING( WDataSetTimeSeries( d, t ) );
            t[ 2 ] = -0.0 / 0.0;
            TS_ASSERT_THROWS( WDataSetTimeSeries( d, t ), WException );

            // test what happens when there are equal time values
            t[ 2 ] = t[ 1 ];
            TS_ASSERT_THROWS( WDataSetTimeSeries( d, t ), WException );

            // test what happens if the grids don't match
            t[ 2 ] = 2.0;
            TS_ASSERT_THROWS_NOTHING( WDataSetTimeSeries( d, t ) );

            t.push_back( 4.0f );

            WMatrix< double > mat( 4, 4 );
            boost::shared_ptr< std::vector< double > > v = boost::shared_ptr< std::vector< double > >( new std::vector< double >( 27, 4 ) );
            mat.makeIdentity();
            mat( 0, 0 ) = 1.0;
            mat( 1, 1 ) = 0.5;
            mat( 2, 2 ) = 2.0;

            WGridTransformOrtho transform( mat );
            boost::shared_ptr< WGridRegular3D > g( new WGridRegular3D( 3, 3, 3, transform ) );

            boost::shared_ptr< WValueSet< double > > vs( new WValueSet< double >( 0, 1, v, W_DT_DOUBLE ) );
            d.push_back( boost::shared_ptr< WDataSetScalar const >( new WDataSetScalar( vs, g ) ) );
            TS_ASSERT_THROWS( WDataSetTimeSeries( d, t ), WException );

            // test what happens if the valuesets data types don't match
            d.resize( 3 );
            t.resize( 3 );
            TS_ASSERT_THROWS_NOTHING( WDataSetTimeSeries( d, t ) );

            t.push_back( 4.0f );
            boost::shared_ptr< std::vector< float > > v2 = boost::shared_ptr< std::vector< float > >( new std::vector< float >( 27, 4 ) );
            boost::shared_ptr< WValueSet< float > > vs2( new WValueSet< float >( 0, 1, v2, W_DT_FLOAT ) );
            d.push_back( boost::shared_ptr< WDataSetScalar const >( new WDataSetScalar( vs2, d.front()->getGrid() ) ) );
            TS_ASSERT_THROWS( WDataSetTimeSeries( d, t ), WException );
        }

        // test with 2 time slices
        {
            double data[] = { 1.0, 2.0 };
            DataSetPtrVector d;
            TimesVector t;

            // test what happens when the input is empty
            TS_ASSERT_THROWS( WDataSetTimeSeries( d, t ), WException );

            // test what happens when the vector sizes don't match
            createData( data, 2, d, t );
            TS_ASSERT_THROWS_NOTHING( WDataSetTimeSeries( d, t ) );
            t.push_back( 4.0f );
            TS_ASSERT_THROWS( WDataSetTimeSeries( d, t ), WException );

            // test what happens when there is an invalid time value
            t.resize( 2 );
            TS_ASSERT_THROWS_NOTHING( WDataSetTimeSeries( d, t ) );
            t[ 1 ] = 0.0f / 0.0f;
            TS_ASSERT_THROWS( WDataSetTimeSeries( d, t ), WException );

            // test what happens when there are equal time values
            t[ 1 ] = t[ 0 ];
            TS_ASSERT_THROWS( WDataSetTimeSeries( d, t ), WException );

            // test what happens if the grids don't match
            t[ 1 ] = 2.0;
            TS_ASSERT_THROWS_NOTHING( WDataSetTimeSeries( d, t ) );

            t.push_back( 4.0f );

            WMatrix< double > mat( 4, 4 );
            boost::shared_ptr< std::vector< double > > v = boost::shared_ptr< std::vector< double > >( new std::vector< double >( 27, 4 ) );
            mat.makeIdentity();
            mat( 0, 0 ) = 1.0;
            mat( 1, 1 ) = 0.5;
            mat( 2, 2 ) = 2.0;

            WGridTransformOrtho transform( mat );
            boost::shared_ptr< WGridRegular3D > g( new WGridRegular3D( 3, 3, 3, transform ) );

            boost::shared_ptr< WValueSet< double > > vs( new WValueSet< double >( 0, 1, v, W_DT_DOUBLE ) );
            d.push_back( boost::shared_ptr< WDataSetScalar const >( new WDataSetScalar( vs, g ) ) );
            TS_ASSERT_THROWS( WDataSetTimeSeries( d, t ), WException );

            // test what happens if the valuesets data types don't match
            d.resize( 2 );
            t.resize( 2 );
            TS_ASSERT_THROWS_NOTHING( WDataSetTimeSeries( d, t ) );

            t.push_back( 4.0f );
            boost::shared_ptr< std::vector< float > > v2 = boost::shared_ptr< std::vector< float > >( new std::vector< float >( 27, 4 ) );
            boost::shared_ptr< WValueSet< float > > vs2( new WValueSet< float >( 0, 1, v2, W_DT_FLOAT ) );
            d.push_back( boost::shared_ptr< WDataSetScalar const >( new WDataSetScalar( vs2, d.front()->getGrid() ) ) );
            TS_ASSERT_THROWS( WDataSetTimeSeries( d, t ), WException );
        }

        // test with 1 time slice
        {
            double data[] = { 1.0 };
            DataSetPtrVector d;
            TimesVector t;

            // test what happens when the input is empty
            TS_ASSERT_THROWS( WDataSetTimeSeries( d, t ), WException );

            // test what happens when the vector sizes don't match
            createData( data, 1, d, t );
            TS_ASSERT_THROWS_NOTHING( WDataSetTimeSeries( d, t ) );
            t.push_back( 4.0f );
            TS_ASSERT_THROWS( WDataSetTimeSeries( d, t ), WException );

            // test what happens when there is an invalid time value
            t.resize( 1 );
            TS_ASSERT_THROWS_NOTHING( WDataSetTimeSeries( d, t ) );
            t[ 0 ] = -0.0f / 0.0f;
            TS_ASSERT_THROWS( WDataSetTimeSeries( d, t ), WException );
        }

        // datasets should be sorted by time
        {
            double data[] = { 1.0, 2.0, 3.0 };
            DataSetPtrVector d;
            TimesVector t;

            createData( data, 3, d, t );
            std::swap( t[ 1 ], t[ 2 ] );
            WDataSetTimeSeries ts( d, t );

            TS_ASSERT_EQUALS( d[ 0 ], ts.m_dataSets[ 0 ].first );
            TS_ASSERT_EQUALS( d[ 2 ], ts.m_dataSets[ 1 ].first );
            TS_ASSERT_EQUALS( d[ 1 ], ts.m_dataSets[ 2 ].first );
        }
    }

    /**
     * The correct minimum and maximum times should be returned.
     */
    void testTimeMinMax()
    {
        double data[] = { 1.0, 2.0, 3.0 };
        DataSetPtrVector d;
        TimesVector t;

        createData( data, 3, d, t );
        {
            WDataSetTimeSeries ts( d, t );

            TS_ASSERT_EQUALS( ts.getMinTime(), 0.0f );
            TS_ASSERT_EQUALS( ts.getMaxTime(), 2.0f );
        }

        createData( data, 3, d, t );
        t[ 0 ] = 1.34f;
        t[ 2 ] = 1.43f;
        {
            WDataSetTimeSeries ts( d, t );

            TS_ASSERT_EQUALS( ts.getMinTime(), 1.0f );
            TS_ASSERT_EQUALS( ts.getMaxTime(), 1.43f );
        }
    }

    /**
     * Times that were provided on construction should be found. Times outside of the
     * interval [getMinTime(),getMaxTime()] should be rejected.
     */
    void testIsTimeSlice()
    {
        double data[] = { 1.0, 2.0, 3.0 };
        DataSetPtrVector d;
        TimesVector t;

        createData( data, 3, d, t );
        {
            WDataSetTimeSeries ts( d, t );
            TS_ASSERT( ts.isTimeSlice( 0.0f ) );
            TS_ASSERT( ts.isTimeSlice( 1.0f ) );
            TS_ASSERT( ts.isTimeSlice( 2.0f ) );
            TS_ASSERT( !ts.isTimeSlice( 5.0f ) );
            TS_ASSERT( !ts.isTimeSlice( 0.0f / 0.0f ) );
            TS_ASSERT( !ts.isTimeSlice( std::numeric_limits< float >::infinity() ) );
            TS_ASSERT( !ts.isTimeSlice( 1.00001f ) );
            TS_ASSERT( !ts.isTimeSlice( 2.345f ) );
            TS_ASSERT( !ts.isTimeSlice( 0.5234f ) );
            TS_ASSERT( !ts.isTimeSlice( -wlimits::FLT_EPS ) );
        }

        createData( data, 3, d, t );
        t[ 0 ] = 1.34f;
        t[ 2 ] = 1.43f;
        {
            WDataSetTimeSeries ts( d, t );
            TS_ASSERT( ts.isTimeSlice( 1.34f ) );
            TS_ASSERT( ts.isTimeSlice( 1.43f ) );
            TS_ASSERT( ts.isTimeSlice( 1.0f ) );
            TS_ASSERT( !ts.isTimeSlice( 5.0f ) );
            TS_ASSERT( !ts.isTimeSlice( 0.0f / 0.0f ) );
            TS_ASSERT( !ts.isTimeSlice( std::numeric_limits< float >::infinity() ) );
            TS_ASSERT( !ts.isTimeSlice( 1.00001f ) );
            TS_ASSERT( !ts.isTimeSlice( 2.345f ) );
            TS_ASSERT( !ts.isTimeSlice( 0.5234f ) );
            TS_ASSERT( !ts.isTimeSlice( -wlimits::FLT_EPS ) );
        }
    }

    /**
     * The nearest time slices should be calculated correctly. Boundary conditions must be
     * handled correctly.
     */
    void testGetNearestTimeSlice()
    {
        double data[] = { 1.0, 2.0, 3.0 };
        DataSetPtrVector d;
        TimesVector t;

        createData( data, 3, d, t );
        WDataSetTimeSeries ts( d, t );

        float f = ts.findNearestTimeSlice( -std::numeric_limits< float >::infinity() );
        TS_ASSERT_EQUALS( 0.0, f );
        f = ts.findNearestTimeSlice( -3346.0 );
        TS_ASSERT_EQUALS( 0.0, f );
        f = ts.findNearestTimeSlice( -1.0 );
        TS_ASSERT_EQUALS( 0.0, f );
        f = ts.findNearestTimeSlice( -0.01 );
        TS_ASSERT_EQUALS( 0.0, f );
        f = ts.findNearestTimeSlice( -wlimits::FLT_EPS );
        TS_ASSERT_EQUALS( 0.0, f );
        f = ts.findNearestTimeSlice( 0.0 );
        TS_ASSERT_EQUALS( 0.0, f );
        f = ts.findNearestTimeSlice( wlimits::FLT_EPS );
        TS_ASSERT_EQUALS( 0.0, f );
        f = ts.findNearestTimeSlice( 0.3 );
        TS_ASSERT_EQUALS( 0.0, f );
        f = ts.findNearestTimeSlice( 0.5 );
        TS_ASSERT_EQUALS( 0.0, f );
        f = ts.findNearestTimeSlice( 0.5 + wlimits::FLT_EPS );
        TS_ASSERT_EQUALS( 1.0, f );
        f = ts.findNearestTimeSlice( 1.0 - wlimits::FLT_EPS );
        TS_ASSERT_EQUALS( 1.0, f );
        f = ts.findNearestTimeSlice( 1.5 - wlimits::FLT_EPS );
        TS_ASSERT_EQUALS( 1.0, f );
        f = ts.findNearestTimeSlice( 1.5 );
        TS_ASSERT_EQUALS( 1.0, f );
        f = ts.findNearestTimeSlice( 2.0 - wlimits::FLT_EPS );
        TS_ASSERT_EQUALS( 2.0f, f );
        f = ts.findNearestTimeSlice( 2.0 );
        TS_ASSERT_EQUALS( 2.0f, f );
        f = ts.findNearestTimeSlice( std::numeric_limits< float >::infinity() );
        TS_ASSERT_EQUALS( 2.0f, f );
        TS_ASSERT_THROWS( ts.findNearestTimeSlice( 0.0 / 0.0 ), WException );
    }

    /**
     * Provided datasets should be returned for provided time slices.
     */
    void testGetDataSetPtrAtTimeSlice()
    {
        double data[] = { 1.0, 2.0, 3.0 };
        DataSetPtrVector d;
        TimesVector t;

        createData( data, 3, d, t );
        WDataSetTimeSeries ts( d, t );

        boost::shared_ptr< WDataSetScalar const > null;

        TS_ASSERT_EQUALS( ts.getDataSetPtrAtTimeSlice( 0.0f / 0.0f ), null );
        TS_ASSERT_EQUALS( ts.getDataSetPtrAtTimeSlice( -std::numeric_limits< float >::infinity() ), null );
        TS_ASSERT_EQUALS( ts.getDataSetPtrAtTimeSlice( 0.0f ), d[ 0 ] );
        TS_ASSERT_EQUALS( ts.getDataSetPtrAtTimeSlice( wlimits::FLT_EPS ), null );
        TS_ASSERT_EQUALS( ts.getDataSetPtrAtTimeSlice( 0.999f ), null );
        TS_ASSERT_EQUALS( ts.getDataSetPtrAtTimeSlice( 1.0f ), d[ 1 ] );
        TS_ASSERT_EQUALS( ts.getDataSetPtrAtTimeSlice( 2.0f ), d[ 2 ] );
        TS_ASSERT_EQUALS( ts.getDataSetPtrAtTimeSlice( 344643.0f ), null );
        TS_ASSERT_EQUALS( ts.getDataSetPtrAtTimeSlice( std::numeric_limits< float >::infinity() ), null );
    }

    /**
     * Interpolated datasets need to be correct.
     */
    void testInterpolatedDataSets()
    {
        double data[] = { 1.0, 2.0, 3.0 };
        DataSetPtrVector d;
        TimesVector t;

        createData( data, 3, d, t );
        WDataSetTimeSeries ts( d, t );

        boost::shared_ptr< WDataSetScalar const > null;
        boost::shared_ptr< WDataSetScalar const > ds;
        std::string name( "a name" );

        ds = ts.calcDataSetAtTime( -std::numeric_limits< float >::infinity(), name );
        TS_ASSERT_EQUALS( ds, null );

        ds = ts.calcDataSetAtTime( -wlimits::FLT_EPS, name );
        TS_ASSERT_EQUALS( ds, null );

        ds = ts.calcDataSetAtTime( std::numeric_limits< float >::infinity(), name );
        TS_ASSERT_EQUALS( ds, null );

        ds = ts.calcDataSetAtTime( 2.0f + 2.0f * wlimits::FLT_EPS, name );
        TS_ASSERT_EQUALS( ds, null );

        ds = ts.calcDataSetAtTime( 0.0f, name );
        TS_ASSERT_EQUALS( ds, d[ 0 ] );

        ds = ts.calcDataSetAtTime( 1.0f, name );
        TS_ASSERT_EQUALS( ds, d[ 1 ] );

        ds = ts.calcDataSetAtTime( 2.0f, name );
        TS_ASSERT_EQUALS( ds, d[ 2 ] );

        {
            std::vector< double > v( 27, 1.35 );
            ds = ts.calcDataSetAtTime( 0.35f, name );
            TS_ASSERT( ds );
            boost::shared_ptr< WValueSet< double > > vs = boost::dynamic_pointer_cast< WValueSet< double > >( ds->getValueSet() );
            TS_ASSERT( vs );
            for( std::size_t k = 0; k < v.size(); ++k )
            {
                TS_ASSERT_DELTA( v[ k ], vs->rawData()[ k ], 1.0f * wlimits::FLT_EPS );
            }
        }
        {
            std::vector< double > v( 27, 1.99 );
            ds = ts.calcDataSetAtTime( 0.99f, name );
            TS_ASSERT( ds );
            boost::shared_ptr< WValueSet< double > > vs = boost::dynamic_pointer_cast< WValueSet< double > >( ds->getValueSet() );
            TS_ASSERT( vs );
            for( std::size_t k = 0; k < v.size(); ++k )
            {
                TS_ASSERT_DELTA( v[ k ], vs->rawData()[ k ], 1.0f * wlimits::FLT_EPS );
            }
        }
        {
            std::vector< double > v( 27, 2.598 );
            ds = ts.calcDataSetAtTime( 1.598f, name );
            TS_ASSERT( ds );
            boost::shared_ptr< WValueSet< double > > vs = boost::dynamic_pointer_cast< WValueSet< double > >( ds->getValueSet() );
            TS_ASSERT( vs );
            for( std::size_t k = 0; k < v.size(); ++k )
            {
                TS_ASSERT_DELTA( v[ k ], vs->rawData()[ k ], 1.0f * wlimits::FLT_EPS );
            }
        }
    }

    /**
     * Interpolation of values should be correct.
     */
    void testInterpolate()
    {
        double data[] = { 1.0, 2.0, 3.0 };
        DataSetPtrVector d;
        TimesVector t;

        createData( data, 3, d, t );
        WDataSetTimeSeries ts( d, t );

        float inf = std::numeric_limits< float >::infinity();
        bool success;
        double h;

        // test invalid times
        WVector3d pos( 1.0, 0.5, 1.0 );

        TS_ASSERT_THROWS( h = ts.interpolate< double >( pos, -inf, &success ), WException );
        TS_ASSERT( !success );
        TS_ASSERT_THROWS( h = ts.interpolate< double >( pos, -3.0f, &success ), WException );
        TS_ASSERT( !success );
        TS_ASSERT_THROWS( h = ts.interpolate< double >( pos, -wlimits::FLT_EPS, &success ), WException );
        TS_ASSERT( !success );
        TS_ASSERT_THROWS( h = ts.interpolate< double >( pos, 2.0f + 2.0f * wlimits::FLT_EPS, &success ), WException );
        TS_ASSERT( !success );
        TS_ASSERT_THROWS( h = ts.interpolate< double >( pos, inf, &success ), WException );
        TS_ASSERT( !success );

        // test invalid position
        float time = 0.99f;
        pos[ 0 ] = -wlimits::FLT_EPS;
        h = ts.interpolate< double >( pos, time, &success );
        TS_ASSERT( !success );

        // now test some valid cases
        pos[ 0 ] = 1.0f;
        h = ts.interpolate< double >( pos, time, &success );
        TS_ASSERT( success );
        TS_ASSERT_DELTA( h, 1.99, wlimits::FLT_EPS );
    }

    /**
     * Test the lower bound time helper routine.
     * It should return the largest time slice that is smaller than or equal
     * to the input time, or -inf if there is no such time slice.
     */
    void testLBTime()
    {
        double data[] = { 1.0, 2.0, 3.0 };
        DataSetPtrVector d;
        TimesVector t;

        createData( data, 3, d, t );
        WDataSetTimeSeries ts( d, t );

        float neginf = -std::numeric_limits< float >::infinity();

        // not using TS_ASSERT_EQUALS here because of a bug
        // passing inf as a parameter leads to an endless loop
        TS_ASSERT( ts.getLBTimeSlice( neginf ) == neginf );
        TS_ASSERT( ts.getLBTimeSlice( -0.01f ) == neginf );
        TS_ASSERT( ts.getLBTimeSlice( 0.0f ) == 0.0f );
        TS_ASSERT( ts.getLBTimeSlice( -wlimits::FLT_EPS ) == neginf );
        TS_ASSERT( ts.getLBTimeSlice( wlimits::FLT_EPS ) == 0.0f );
        TS_ASSERT( ts.getLBTimeSlice( 1.0f ) == 1.0f );
        TS_ASSERT( ts.getLBTimeSlice( 1.2f ) == 1.0f );
        TS_ASSERT( ts.getLBTimeSlice( 2.0f - wlimits::FLT_EPS ) == 1.0f );
        TS_ASSERT( ts.getLBTimeSlice( 2.0f ) == 2.0f );
        TS_ASSERT( ts.getLBTimeSlice( -neginf ) == 2.0f );

        // note that there is no test for nan, as these routines are private
        // it is the callers responsibility to check for nan
    }

    /**
     * Test the upper bound time helper routine.
     * It should return the smallest time slice that is larger than the input
     * time, or inf if there is no such time slice.
     */
    void testUBTime()
    {
        double data[] = { 1.0, 2.0, 3.0 };
        DataSetPtrVector d;
        TimesVector t;

        createData( data, 3, d, t );
        WDataSetTimeSeries ts( d, t );

        float inf = std::numeric_limits< float >::infinity();

        // not using TS_ASSERT_EQUALS here because of a bug
        // passing inf as a parameter leads to an endless loop
        TS_ASSERT( ts.getUBTimeSlice( -inf ) == 0.0f );
        TS_ASSERT( ts.getUBTimeSlice( -0.01f ) == 0.0f );
        TS_ASSERT( ts.getUBTimeSlice( 0.0f ) == 1.0f );
        TS_ASSERT( ts.getUBTimeSlice( -wlimits::FLT_EPS ) == 0.0f );
        TS_ASSERT( ts.getUBTimeSlice( wlimits::FLT_EPS ) == 1.0f );
        TS_ASSERT( ts.getUBTimeSlice( 1.0f ) == 2.0f );
        TS_ASSERT( ts.getUBTimeSlice( 1.2f ) == 2.0f );
        TS_ASSERT( ts.getUBTimeSlice( 2.0f - wlimits::FLT_EPS ) == 2.0f );
        TS_ASSERT( ts.getUBTimeSlice( 2.0f ) == inf );
        TS_ASSERT( ts.getUBTimeSlice( inf ) == inf );

        // note that there is no test for nan, as these routines are private
        // it is the callers responsibility to check for nan
    }

private:
    /**
     * A helper function that creates some input data.
     *
     * \param data An array of data values, one for each time slice.
     * \param number The number of time slices.
     * \param dsets The output datasets.
     * \param times Some times for the output datasets.
     */
    void createData( double* data, int number, DataSetPtrVector& dsets, TimesVector& times ) // NOLINT
    {
        dsets.clear();
        times.clear();

        WMatrix< double > mat( 4, 4 );
        mat.makeIdentity();
        mat( 0, 0 ) = 1.0;
        mat( 1, 1 ) = 0.5;
        mat( 2, 2 ) = 2.0;

        WGridTransformOrtho transform( mat );
        boost::shared_ptr< WGridRegular3D > g( new WGridRegular3D( 3, 3, 3, transform ) );

        for( int i = 0; i < number; ++i )
        {
            boost::shared_ptr< std::vector< double > > v = boost::shared_ptr< std::vector< double > >( new std::vector< double >( 27, data[i] ) );
            boost::shared_ptr< WValueSet< double > > vs( new WValueSet< double >( 0, 1, v, W_DT_DOUBLE ) );
            dsets.push_back( boost::shared_ptr< WDataSetScalar const >( new WDataSetScalar( vs, g ) ) );
            times.push_back( static_cast< float >( i ) );
        }
    }

    /**
     * Setup logger and other stuff for each test.
     */
    void setUp()
    {
        WLogger::startup();
    }
};

#endif  // WDATASETTIMESERIES_TEST_H
