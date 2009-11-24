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

#ifndef WZHANGMETRIC_TEST_H
#define WZHANGMETRIC_TEST_H

#include <utility>
#include <vector>

#include <cxxtest/TestSuite.h>

#include "../WZhangMetric.h"

/**
 * Dummy class since WZhangMetric is abstract
 */
class Dummy : public WZhangMetric
{
public:
    /**
     * Creates a dummy subclass from WZhangMetric instance to test the
     * dXt_optimized member function since WZhangMetric is abstract.
     */
    explicit Dummy( double thresholdSquare )
        : WZhangMetric( thresholdSquare )
    {
    }

    /**
     * Computes the d_t(Q,R) if m_dtqr is true via inherited dXt_optimized.
     */
    virtual double dist( const wmath::WFiber &q, const wmath::WFiber &r ) const
    {
        std::pair< double, double > result = dXt_optimized( q, r );
        if( m_dtqr )
        {
            return result.first;
        }
        return result.second;
    }

    /**
     * Flag for computation of metric d_t. If true dist will compute d_t(Q,R)
     * otherwise d_t(R,Q)
     */
    bool m_dtqr;
};

/**
 * Test the optimized measure dXt
 */
class WZhangMetricTest : public CxxTest::TestSuite
{
public:
    /**
     * The dt(Q,R) measure (mean closest point distance) is not symmetric.
     * distances below a certain threshold will be omitted.
     */
    void testDTMeasure( void )
    {
        Dummy dt( 1.0 );
        dt.m_dtqr = true;

        // generates two fibers just in 2D
        using wmath::WPosition;
        std::vector< WPosition > lineData1;
        lineData1.push_back( WPosition( 0, 1, 0 ) );
        lineData1.push_back( WPosition( 0, 0, 0 ) );
        std::vector< WPosition > lineData2;
        lineData2.push_back( WPosition( 1, 1, 0 ) );
        lineData2.push_back( WPosition( 2, 2, 0 ) );
        wmath::WFiber q( lineData1 );
        wmath::WFiber r( lineData2 );

        TS_ASSERT_EQUALS( dt.dist( q, r ), std::sqrt( 2.0 ) / 2.0 );
        dt.m_dtqr = false;
        TS_ASSERT_EQUALS( dt.dist( q, r ), std::sqrt( 5.0 ) / 2.0 );
    }
};

#endif  // WZHANGMETRIC_TEST_H
