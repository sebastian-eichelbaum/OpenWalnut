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

#ifndef WITKIMAGECONVERSION_TEST_H
#define WITKIMAGECONVERSION_TEST_H

#include <vector>

#include <boost/shared_ptr.hpp>

#include <cxxtest/TestSuite.h>

#include "../../common/WLogger.h"
#include "../WITKImageConversion.h"

/**
 * The logger instance used by some tests
 */
static WLogger logger;

/**
 * True if the logger has been initialized in the past.
 */
static bool loggerInitialized = false;

/**
 * Test functionality of WITKConversion class.
 */
class WITKImageConversionTest : public CxxTest::TestSuite
{
public:
    /**
     * Setup method called before every test case. This initializes the logger if needed.
     */
    void setUp()
    {
        if ( !loggerInitialized )
        {
            std::cout << "Initialize logger." << std::endl;
            logger.setColored( false );

            // NOTE: the logger does not need to be run, since the logger main thread just prints the messages. If compiled in
            // debug mode, the messages will be printed directly, without the logger thread.
            //logger.run();
            loggerInitialized = true;
        }
    }

    /**
     * Converting a dataset into an itk image and then converting it back into a
     * dataset should yield the same dataset.
     */
    void testConversion()
    {
        // build a dataset
        std::vector< int > values( 27, 0 );
        for( int k = 0; k < 27; ++k )
        {
            values[ k ] = 27 - k;
        }
        boost::shared_ptr< WGridRegular3D > g( new WGridRegular3D( 3, 3, 3, 1, 1, 1 ) );
        boost::shared_ptr< WValueSet< int > > v( new WValueSet< int >( 0, 1, values, W_DT_SIGNED_INT ) );
        boost::shared_ptr< WDataSetScalar > ds( new WDataSetScalar( v, g ) );

        itk::Image< int, 3 >::Pointer i = makeImageFromDataSet< int >( ds );
        boost::shared_ptr< WDataSetScalar > newds = makeDataSetFromImage< int >( i );

        TS_ASSERT_SAME_DATA( boost::shared_dynamic_cast< WValueSet< int > >( ds->getValueSet() )->rawData(),
                             boost::shared_dynamic_cast< WValueSet< int > >( newds->getValueSet() )->rawData(), sizeof( int ) * 27 );
    }
};

#endif  // WITKIMAGECONVERSION_TEST_H
