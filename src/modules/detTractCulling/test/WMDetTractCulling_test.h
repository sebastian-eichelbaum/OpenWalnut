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

#ifndef WMDETTRACTCULLING_TEST_H
#define WMDETTRACTCULLING_TEST_H

#include <string>

#include <cxxtest/TestSuite.h>

#include "../WMDetTractCulling.h"

/**
 * Unit test some functions in the deterministic tract culling module.
 */
class WMDetTractCullingTest : public CxxTest::TestSuite
{
public:
    /**
     * The correct save path is ascertained by replacing the extension with a
     * new suffix containing also the culling parameters.
     */
    void testSaveFileNameGeneration( void )
    {
        WMDetTractCulling module;
        module.properties();
        module.m_proximity_t->setMax( 200 );
        module.m_proximity_t->set( 123.454 );
        std::cout << "XXX" << module.m_proximity_t->get() << std::endl;
        module.m_dSt_culling_t->set( 78.9 );
        std::string expected( "/no/such/file.pt-123.45.dst-78.90.fib" );
        TS_ASSERT_EQUALS( module.saveFileName( "/no/such/file.fib" ).string(), expected );
    }

    /**
     * If no extension is present then the new file name should be ascertained by just
     * concatenating the new extension with parameters.
     */
    void testSaveFileNameGenerationWithoutExtension( void )
    {
        WMDetTractCulling module;
        module.properties();
        std::string expected( "/no/such/file.pt-1.00.dst-6.50.fib" );
        TS_ASSERT_EQUALS( module.saveFileName( "/no/such/file" ).string(), expected );
    }

    /**
     * If the path is gained from Windows every thing should work too.
     */
    void testSaveFileNameGenerationAlsoWorkingOnWindowsPaths( void )
    {
        WMDetTractCulling module;
        module.properties();
        std::string expected( "C:\\no\\such\\file.pt-1.00.dst-6.50.fib" );
        TS_ASSERT_EQUALS( module.saveFileName( "C:\\no\\such\\file" ).string(), expected );
    }
};

#endif  // WMDETTRACTCULLING_TEST_H
