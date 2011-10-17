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

#include "core/common/WStringUtils.h"
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
//        WSimpleResampler r;
//        TS_ASSERT_DELTA( r.lineIntegration( m_equiTractVerts, m_startIdx, m_length ), std::sqrt( 3.0 ) * 4.0, 1.0e-6 );
//        TS_ASSERT_DELTA( r.lineIntegration( m_nonEquiTractVerts, m_startIdx, m_length ),
//                         2 * std::sqrt( 0.05 * 0.05 + 0.9 * 0.9 ) + 2 * std::sqrt( 0.2 * 0.2 + 0.1 * 0.1 ),
//                         1.0e-6 );
    }
//
//    /**
//     * If there is the same number of sample points given then the tract should
//     * be resampled equidistantly into \c n-1 segments when having \c n new
//     * sample points.
//     */
//    void testResamplingWithGivenNumberOfNewSamplePointsOnEquidistantSampledTract( void )
//    {
//        WSimpleResampler r( 3 );
//        boost::shared_ptr< std::vector< float > > newTractVerts( new std::vector< float >( 9*3, -1.0 ) );
//        r.resample( m_equiTractVerts, m_startIdx, m_length, newTractVerts, 3 );
//        float verts[] = { -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,  // NOLINT
//                            0.0,  0.0,  0.0,
//                            2.0,  2.0,  2.0,
//                            4.0,  4.0,  4.0,
//                           -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0 };  // NOLINT
//        std::vector< float > expected( verts, verts + sizeof( verts ) / sizeof( float ) );
//        TS_ASSERT_EQUALS( expected.size(), newTractVerts->size() );
//        for( size_t i = 0; i < expected.size(); ++i )
//        {
//            if( std::abs( expected[i] - (*newTractVerts)[i] ) > 1.0e-6 )
//            {
//                std::stringstream ss;
//                ss << "Positions differ in index: " << i << " with expected: " << expected[i] << " but got " << (*newTractVerts)[i];
//                TS_FAIL( ss.str() );
//            }
//        }
//    }
//
//    /**
//     * If the tract is not equidistant sampled than this should work too.
//     */
//    void testResamplingOnNonEquidistantTract( void )
//    {
//        WSimpleResampler r( 3 );
//        boost::shared_ptr< std::vector< float > > newTractVerts( new std::vector< float >( 9*3, -1.0 ) );
//        r.resample( m_nonEquiTractVerts, m_startIdx, m_length, newTractVerts, 3 );
//        float verts[] = { -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,  // NOLINT
//                            0.75,  0.0,  0.0,
//                            1.0,  1.0,  0.0,
//                            1.25,  0.0,  0.0,
//                           -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0 };  // NOLINT
//        std::vector< float > expected( verts, verts + sizeof( verts ) / sizeof( float ) );
//        TS_ASSERT_EQUALS( expected.size(), newTractVerts->size() );
//        for( size_t i = 0; i < expected.size(); ++i )
//        {
//            if( std::abs( expected[i] - (*newTractVerts)[i] ) > 1.0e-6 )
//            {
//                using string_utils::operator<<;
//                std::stringstream ss;
//                ss << "Positions differ in index: " << i << " with expected: " << expected[i] << " but got " << (*newTractVerts)[i];
//                ss << std::endl << expected << std::endl;
//                ss << std::endl << *newTractVerts << std::endl;
//                TS_FAIL( ss.str() );
//            }
//        }
//    }
//
//    /**
//     * The resampling should also work with equidistant sampled tracts.
//     */
//    void testResamplingOnEquidistantTract( void )
//    {
//    }
//
//    /**
//     * Prepares two single fibers, one which is equidistant sampled and one
//     * which is not. Also the vertex array is simulated, as well as start index
//     * and length.
//     */
//    void setUp( void )
//    {
//        float vertsE[] = { -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,  // NOLINT array init list
//                             0.0,  0.0,  0.0,
//                             1.0,  1.0,  1.0,
//                             2.0,  2.0,  2.0,
//                             3.0,  3.0,  3.0,
//                             4.0,  4.0,  4.0,
//                            -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0 };  // NOLINT array init list
//        m_equiTractVerts = boost::shared_ptr< std::vector< float > >( new std::vector< float >( vertsE, vertsE + sizeof( vertsE ) / sizeof( float ) ) ); // NOLINT long line
//        m_startIdx = 5;
//        m_length = 5;
//        float vertsNE[] = { -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,  // NOLINT array init list
//                              0.75, 0.0,  0.0,
//                              0.8,  0.9,  0.0,
//                              1.0,  1.0,  0.0,
//                              1.2,  0.9,  0.0,
//                              1.25, 0.0,  0.0,
//                             -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0 };  // NOLINT array init list
//        m_nonEquiTractVerts = boost::shared_ptr< std::vector< float > >( new std::vector< float >( vertsNE, vertsNE + sizeof( vertsNE ) / sizeof( float ) ) ); // NOLINT long line
//    }
//
//    /**
//     * We do not need to remove the arrays since the are not allocated via the
//     * NEW operator. But we should clear the vectors and reset the indices.
//     */
//    void tearDown( void )
//    {
//        m_equiTractVerts->clear();
//        m_nonEquiTractVerts->clear();
//        m_startIdx = 0;
//        m_length = 0;
//    }
//
//private:
//    /**
//     * Simulates a vertex array of equidistant sampled tracts as you will have
//     * with WDataSetFibers.
//     */
//    boost::shared_ptr< std::vector< float > > m_equiTractVerts;
//
//    /**
//     * Simulates a vertex array of non equidistant sampled tracts as you maybe
//     * will have with WDataSetFibers.
//     */
//    boost::shared_ptr< std::vector< float > > m_nonEquiTractVerts;
//
//    /**
//     * Simulates the start index of the a tract inside of the vertex array.
//     */
//    size_t m_startIdx;
//
//    /**
//     * Simulates the length of the tract inside of the vertex array.
//     */
//    size_t m_length;
};

#endif  // WSIMPLERESAMPLER_TEST_H
