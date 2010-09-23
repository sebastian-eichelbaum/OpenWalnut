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

#ifndef WSIMPLERESAMPLER_TEST_H
#define WSIMPLERESAMPLER_TEST_H

#include <sstream>
#include <vector>

#include <cxxtest/TestSuite.h>

#include "../WSimpleResampler.h"

/**
 * Testsuite for the simple resample method.
 */
class WSimpleResamplerTest : public CxxTest::TestSuite
{
public:

    /**
     * The line integrator sum's up all segment's length''''ssss. Damn I never
     * know when to set the apostroph''ss...
     */
    void testLineIntegration( void )
    {
        WSimpleResampler r( 0 );
        TS_ASSERT_EQUALS( r.lineIntegration( m_equiTractVerts, m_startIdx, m_length ), std::sqrt( 3.0 ) * 4.0 );
    }

    /**
     * If there is the same number of sample points given then the tract should
     * be resampled equidistantly into \c n-1 segments when having \c n new
     * sample points.
     */
    void testResamplingWithGivenNumberOfNewSamplePoints( void )
    {
        WSimpleResampler r( 3 );
        boost::shared_ptr< std::vector< double > > newTractVerts( new std::vector< double >( 9*3, -1.0 ) );
        r.resample( m_equiTractVerts, m_startIdx, m_length, newTractVerts, 3 );
        double verts[] = { -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,  // NOLINT
                            0.0,  0.0,  0.0,
                            2.0,  2.0,  2.0,
                            4.0,  4.0,  4.0,
                           -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0 };  // NOLINT
        std::vector< double > expected( verts, verts + sizeof( verts ) / sizeof( double ) );
        TS_ASSERT_EQUALS( expected.size(), newTractVerts->size() );
        for( size_t i = 0; i < expected.size(); ++i )
        {
            if( std::abs( expected[i] - (*newTractVerts)[i] ) > 1.0e-10 )
            {
                std::stringstream ss;
                ss << "Positions differ in index: " << i << " with expected: " << expected[i] << " but got " << (*newTractVerts)[i];
                TS_FAIL( ss.str() );
            }
        }
    }

//    /**
//     * When a given new segment length is given, resample the fiber in segments
//     * of this length, the last segment may be not that length.
//     * TODO(math): please talk to sebastian, if that really makes sense??
//     */
//    void testResampleWithGivenSegmentLength( void )
//    {
//    }

    /**
     * If the tract is not equidistant sampled than this should work:
     * TODO(math): describe the drawbacks of the simple resampler
     */
    void testResamplingOnNonEquidistantTract( void )
    {
    }

    /**
     * The resampling should also work with equidistant sampled tracts.
     */
    void testResamplingOnEquidistantTract( void )
    {
    }

    /**
     * Prepares two single fibers, one which is equidistant sampled and one
     * which is not. Also the vertex array is simulated, as well as start index
     * and length.
     */
    void setUp( void )
    {
        double vertsE[] = { -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,  // NOLINT array init list
                             0.0,  0.0,  0.0,
                             1.0,  1.0,  1.0,
                             2.0,  2.0,  2.0,
                             3.0,  3.0,  3.0,
                             4.0,  4.0,  4.0,
                            -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0 };  // NOLINT array init list
        m_equiTractVerts = boost::shared_ptr< std::vector< double > >( new std::vector< double >( vertsE, vertsE + sizeof( vertsE ) / sizeof( double ) ) ); // NOLINT long line
        m_startIdx = 5;
        m_length = 5;
        double vertsNE[] = { -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,  // NOLINT array init list
                              0.75, 0.0,  0.0,
                              0.8,  0.9,  0.0,
                              1.0,  1.0,  0.0,
                              1.2,  0.9,  0.0,
                              1.25, 0.0,  0.0,
                             -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0 };  // NOLINT array init list
        m_nonEquiTractVerts = boost::shared_ptr< std::vector< double > >( new std::vector< double >( vertsNE, vertsNE + sizeof( vertsNE ) / sizeof( double ) ) ); // NOLINT long line
    }

    /**
     * We do not need to remove the arrays since the are not allocated via the
     * NEW operator. But we should clear the vectors and reset the indices.
     */
    void tearDown( void )
    {
        m_equiTractVerts->clear();
        m_nonEquiTractVerts->clear();
        m_startIdx = 0;
        m_length = 0;
    }

private:
    /**
     * Simulates a vertex array of equidistant sampled tracts as you will have
     * with WDataSetFibers.
     */
    boost::shared_ptr< std::vector< double > > m_equiTractVerts;

    /**
     * Simulates a vertex array of non equidistant sampled tracts as you maybe
     * will have with WDataSetFibers.
     */
    boost::shared_ptr< std::vector< double > > m_nonEquiTractVerts;

    /**
     * Simulates the start index of the a tract inside of the vertex array.
     */
    size_t m_startIdx;

    /**
     * Simulates the length of the tract inside of the vertex array.
     */
    size_t m_length;
};

#endif  // WSIMPLERESAMPLER_TEST_H
