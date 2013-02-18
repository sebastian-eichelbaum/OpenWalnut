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

#ifndef WTHREADEDPERVOXELOPERATION_TEST_H
#define WTHREADEDPERVOXELOPERATION_TEST_H

#include <vector>

#include <cxxtest/TestSuite.h>

#include "../../common/WThreadedFunction.h"
#include "../../common/WLogger.h"
#include "../WDataHandlerEnums.h"
#include "../WDataSetSingle.h"
#include "../WThreadedPerVoxelOperation.h"

/**
 * \class WThreadedPerVoxelOperationTest
 *
 * Test the WThreadedPerVoxelOperation template.
 */
class WThreadedPerVoxelOperationTest : public CxxTest::TestSuite
{
    //! the test instance of the template
    typedef WThreadedPerVoxelOperation< int, 2, float, 3 > TPVO;

    //! the type of valueset used in the test
    typedef TPVO::ValueSetType ValueSetType;

    //! the input type of the test function
    typedef TPVO::TransmitType ArrayType;

    //! the output type of the test function
    typedef TPVO::OutTransmitType OutArrayType;

public:
    /**
     * Setup logger and other stuff for each test.
     */
    void setUp()
    {
        WLogger::startup();
    }

    /**
     * Test if everything gets instantiated correctly or if
     * incorrect values properly lead to exceptions.
     */
    void testInstantiation()
    {
        boost::shared_ptr< WDataSetSingle > ds = buildTestData();

        TS_ASSERT_THROWS( TPVO t( boost::shared_ptr< WDataSetSingle >(),
                                  boost::bind( &WThreadedPerVoxelOperationTest::func, this, _1 ) ), WException );
        TS_ASSERT_THROWS( TPVO t( ds, TPVO::FunctionType() ), WException );
        TS_ASSERT_THROWS( TPVO t( boost::shared_ptr< WDataSetSingle >( new WDataSetSingle( ds->getValueSet(),
                                                                                           boost::shared_ptr< WGrid >() ) ),
                                  boost::bind( &WThreadedPerVoxelOperationTest::func, this, _1 ) ), WException );
        TS_ASSERT_THROWS( TPVO t( boost::shared_ptr< WDataSetSingle >( new WDataSetSingle( boost::shared_ptr< WValueSetBase >(),
                                                                                           ds->getGrid() ) ),
                                  boost::bind( &WThreadedPerVoxelOperationTest::func, this, _1 ) ), WException );

        TS_ASSERT_THROWS_NOTHING( TPVO t( ds, boost::bind( &WThreadedPerVoxelOperationTest::func, this, _1 ) ) );

        TPVO t( ds, boost::bind( &WThreadedPerVoxelOperationTest::func, this, _1 ) );

        TS_ASSERT_EQUALS( ds->getGrid(), t.m_grid );
    }

    /**
     * Now test the whole class as a multithreaded function.
     */
    void testMultithreadedFunction()
    {
        boost::shared_ptr< WDataSetSingle > ds = buildTestData();
        boost::shared_ptr< TPVO > t( new TPVO( ds, boost::bind( &WThreadedPerVoxelOperationTest::func, this, _1 ) ) );

        m_exception = false;
        m_threadsDone = false;

        WThreadedFunction< TPVO > f( 5, t );
        f.getThreadsDoneCondition()->subscribeSignal( boost::bind( &WThreadedPerVoxelOperationTest::handleThreadsDone, this ) );
        f.subscribeExceptionSignal( boost::bind( &WThreadedPerVoxelOperationTest::handleException, this, _1 ) );

        TS_ASSERT_THROWS_NOTHING( f.run() );
        TS_ASSERT_THROWS_NOTHING( f.wait() );
        TS_ASSERT( !m_exception );
        TS_ASSERT( m_threadsDone );

        boost::shared_ptr< WDataSetSingle > res = t->getResult();

        float shouldBe[] = {
                              2.0f,  2.0f,   5.0f,
                             -5.0f,  4.0f,  -7.0f,
                              8.0f,  8.0f,  23.0f,
                             -4.0f,  3.0f,  -6.0f,
                            -28.0f, 13.0f, -44.0f,
                              3.0f,  4.0f,   9.0f,
                             -4.0f,  5.0f,  -4.0f,
                              2.0f, -4.0f,  -1.0f
                           };

        TS_ASSERT( res );
        TS_ASSERT( res->getValueSet() );
        TS_ASSERT_EQUALS( res->getGrid(), ds->getGrid() );
        TS_ASSERT_EQUALS( res->getValueSet()->dimension(), 3 );
        TS_ASSERT_EQUALS( res->getValueSet()->order(), 1 );
        TS_ASSERT_EQUALS( res->getValueSet()->size(), 8 );

        boost::shared_ptr< WValueSet< float > > vs = boost::dynamic_pointer_cast< WValueSet< float > >( res->getValueSet() );

        TS_ASSERT( vs );
        TS_ASSERT( vs->rawData() );
        TS_ASSERT_EQUALS( vs->rawDataVectorPointer()->size(), 24 );
        TS_ASSERT_SAME_DATA( vs->rawData(), shouldBe, 8 * 3 * sizeof( float ) );
    }

private:
    /**
     * The test operation.
     *
     * \param a The subarray of the input valueset that denotes the voxel's data.
     * \return The output data as an array.
     */
    OutArrayType const func( ArrayType const& a ) const
    {
        OutArrayType o;
        o[ 0 ] = static_cast< float >( a[ 1 ] );
        o[ 1 ] = static_cast< float >( a[ 0 ] + 1 );
        o[ 2 ] = static_cast< float >( a[ 0 ] + 2 * a[ 1 ] );
        return o;
    }

    /**
     * Build a test dataset.
     *
     * \return The test dataset.
     */
    boost::shared_ptr< WDataSetSingle > buildTestData()
    {
        int a[] = { 1, 2, 3, -5, 7, 8, 2, -4, 12, -28, 3, 3, 4, -4, -5, 2 };
        boost::shared_ptr< std::vector< int > > v = boost::shared_ptr< std::vector< int > >( new std::vector< int >( a, a + 16 ) );
        dataType r = DataType< int >::type;
        boost::shared_ptr< ValueSetType > vs( new ValueSetType( 1, 2, v, r ) );
        boost::shared_ptr< WGridRegular3D > g( new WGridRegular3D( 2, 2, 2 ) );
        return boost::shared_ptr< WDataSetSingle >( new WDataSetSingle( vs, g ) );
    }

    /**
     * Handle an exception.
     */
    void handleException( WException const& )
    {
        m_exception = true;
    }

    //! a flag that is used to check if any exceptions were thrown
    bool m_exception;

    /**
     * This function gets called when all threads are done.
     */
    void handleThreadsDone()
    {
        m_threadsDone = true;
    }

    //! a flag indicating if all threads are done
    bool m_threadsDone;
};

#endif  // WTHREADEDPERVOXELOPERATION_TEST_H
