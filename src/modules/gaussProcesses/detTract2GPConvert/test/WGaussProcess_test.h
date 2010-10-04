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

#ifndef WGAUSSPROCESS_TEST_H
#define WGAUSSPROCESS_TEST_H

#include <vector>

#include <cxxtest/TestSuite.h>

#include "../../../../common/datastructures/WFiber.h"
#include "../../../../common/WLogger.h"
#include "../../../../dataHandler/WDataSetDTI.h"
#include "../WGaussProcess.h"

static WLogger logger; // In case someone uses the logger in one of the classes above

/**
 * Testsuite for the gaussian process class.
 */
class WGaussProcessTest : public CxxTest::TestSuite
{
public:
    /**
     * If the point for the mean function is outside of the environment with distance R the mean
     * should equals to zero.
     */
    void testMeanFunctionOutsideOf_R_Environment( void )
    {
        WGaussProcess p( m_tract, m_emptyDTIDataSet );
        TS_ASSERT_DELTA( p.mean( wmath::WPosition( -( p.m_R + wlimits::DBL_EPS ), 0.0, 0.0 ) ), 0.0, wlimits::DBL_EPS );
    }

    /**
     * Inside of the R environment there shall be values unequal to zero.
     */
    void testMeanFunctionInsideOf_R_Environment( void )
    {
        WGaussProcess p( m_tract, m_emptyDTIDataSet );
        TS_ASSERT( std::abs( p.mean( wmath::WPosition( -p.m_R + 2 * wlimits::DBL_EPS, 0.0, 0.0 ) ) ) > wlimits::DBL_EPS );
    }

    /**
     * Inside of the R environment the values should be monoton increasing in direction to the tract
     * segements.
     */
    void testMeanFunctionMonotonyIn_R_Environment( void )
    {
        WGaussProcess p( m_tract, m_emptyDTIDataSet );
        TS_ASSERT( std::abs( p.mean( wmath::WPosition( -p.m_R + 3 * wlimits::DBL_EPS, 0.0, 0.0 ) ) ) >
                             p.mean( wmath::WPosition( -p.m_R + 2 * wlimits::DBL_EPS, 0.0, 0.0 ) ) );
    }

    /**
     * The mean value on the sample point is the maximum level set.
     */
    void testMeanFunctionOnSamplePoint( void )
    {
        WGaussProcess p( m_tract, m_emptyDTIDataSet );
        TS_ASSERT_DELTA( p.mean( wmath::WPosition( 0.0, 0.0, 0.0 ) ), p.m_maxLevel, wlimits::DBL_EPS );
    }

//    void testMeanFunctionOnSegmentButNotOnSamplePoint( void )
//    {
//        WGaussProcess p( m_tract, m_emptyDTIDataSet );
//        TS_ASSERT_DELTA( p.mean( wmath::WPosition( 0.4, 0.4, 0.0 ) ), p.m_maxLevel, wlimits::DBL_EPS );
//    }

protected:
    /**
     * SetUp test environment.
     */
    void setUp( void )
    {
        float dataArray[6] = { 0.0, 1.0, 2.0, 3.0, 4.0, 5.0 }; // NOLINT array init list
        std::vector< float > data( &dataArray[0], &dataArray[0] + sizeof( dataArray ) / sizeof( float ) );
        boost::shared_ptr< WValueSetBase > newValueSet( new WValueSet< float >( 1, 6, data, W_DT_FLOAT ) );
        boost::shared_ptr< WGrid > newGrid( new WGridRegular3D( 1, 1, 1, 1, 1, 1 ) );
        m_emptyDTIDataSet = boost::shared_ptr< WDataSetDTI >(  new WDataSetDTI( newValueSet, newGrid ) );

        m_tract.push_back( wmath::WPosition( 0.0, 0.0, 0.0 ) );
        m_tract.push_back( wmath::WPosition( 1.0, 1.0, 0.0 ) );
        m_tract.push_back( wmath::WPosition( 1.0, 2.0, 0.0 ) );
        m_tract.push_back( wmath::WPosition( 2.0, 2.0, 0.0 ) );
    }

    /**
     * Clean up everything.
     */
    void tearDown( void )
    {
        m_tract.clear();
        m_emptyDTIDataSet.reset();
    }

private:
    /**
     * Dummy DTI dataset.
     */
    boost::shared_ptr< WDataSetDTI > m_emptyDTIDataSet;

    /**
     * A single tract to check the gauss process
     */
    wmath::WFiber m_tract;
};

#endif  // WGAUSSPROCESS_TEST_H
