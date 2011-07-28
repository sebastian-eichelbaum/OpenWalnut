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

#ifndef WDATASETFIBERVECTOR_TEST_H
#define WDATASETFIBERVECTOR_TEST_H

#include <vector>

#include <boost/shared_ptr.hpp>
#include <cxxtest/TestSuite.h>

#include "../WDataSetFiberVector.h"
#include "../../common/math/linearAlgebra/WLinearAlgebra.h"

/**
 * Unit tests our fiber dataset class
 */
class WDataSetFiberVectorTest : public CxxTest::TestSuite
{
public:
    /**
     * Generates the test evironment
     */
    void setUp( void )
    {
        using std::vector;
        m_somefibs = boost::shared_ptr< vector< WFiber > >( new vector< WFiber > );
        vector< WPosition > points;
        points.push_back( WPosition( 0., 0., 1. ) );
        m_somefibs->push_back( WFiber( points ) );
        points.push_back( WPosition( 0., 0., 2. ) );
        m_somefibs->push_back( WFiber( points ) );
        points.push_back( WPosition( 0., 0., 3. ) );
        m_somefibs->push_back( WFiber( points ) );
    }

    /**
     * With a vector of WFiber a valid DataSetFibers should be possible
     */
    void testBasicAccessAndCreation( void )
    {
        std::vector< WPosition > points;
        points.push_back( WPosition( 0., 0., 1. ) );
        points.push_back( WPosition( 0., 0., 2. ) );
        points.push_back( WPosition( 0., 0., 3. ) );
        WFiber expected( points );
        WDataSetFiberVector d( m_somefibs );
        TS_ASSERT_EQUALS( d[2], expected );
    }
private:
    boost::shared_ptr< std::vector< WFiber > > m_somefibs; //!< Default fiber dataset
};

#endif  // WDATASETFIBERVECTOR_TEST_H
