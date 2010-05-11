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

#ifndef WMMARCHINGCUBES_TEST_H
#define WMMARCHINGCUBES_TEST_H

#include <string>
#include <vector>

#include <cxxtest/TestSuite.h>

#include "../../../common/WLogger.h"
#include "../../../common/math/WPosition.h"
#include "../../../common/WIOTools.h"
#include "../../../graphicsEngine/WTriangleMesh2.h"
#include "../WMMarchingCubes.h"

static WLogger logger;
static bool loggerInitialized = false;

/**
 * Test for WMMarchingCubes
 */
class WMMarchingCubesTest : public CxxTest::TestSuite
{
public:

    /**
     * Setup method called before every test case.
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
     * Ensure instatiation does not throw and does initialization right.
     */
    void testInstatiation()
    {
        TS_ASSERT_THROWS_NOTHING( WMMarchingCubes() );
    }

    /**
     * Test rejection of surfaces with zero triangles or vertices for save
     */
    void testSaveZero()
    {
        WMMarchingCubes mc;
        boost::shared_ptr< WTriangleMesh2 > triMesh( new WTriangleMesh2( 0, 0 ) );
        mc.m_triMesh = triMesh;
        std::string fileName = wiotools::tempFileName();
        mc.m_properties = boost::shared_ptr< WProperties >( new WProperties( "Properties", "Module's properties" ) );
        mc.m_savePropGroup = mc.m_properties->addPropertyGroup( "Save Surface",  "" );
        mc.m_meshFile = mc.m_savePropGroup->addProperty( "Mesh File", "", boost::filesystem::path( fileName.c_str() ) );
        mc.m_saveTriggerProp = mc.m_savePropGroup->addProperty( "Do Save",  "Press!", WPVBaseTypes::PV_TRIGGER_READY );

        bool result = mc.save();
        TS_ASSERT_EQUALS( result, false ); // should return false as we did not have any vertices or triangles.
        TS_ASSERT( !wiotools::fileExists( fileName ) );
    }

    /**
     * Test rejection of surfaces with nan or inf in coordinates for save
     */
    void testSaveInfinteNan()
    {
        WMMarchingCubes mc;
        const unsigned int nbPos = 10;
        boost::shared_ptr< WTriangleMesh2 > triMesh( new WTriangleMesh2( nbPos, 3 ) );
        mc.m_triMesh = triMesh;

        std::vector< wmath::WPosition > vertices( 0 );
        for( unsigned int posId = 0; posId < nbPos; ++posId )
        {
            double x = posId * posId + 3.4;
            double y = posId + 1;
            double z = 3. /  static_cast< double >( posId ); // provide nan values by dividing with zero
            triMesh->addVertex( x, y, z );
        }

        std::string fileName = wiotools::tempFileName();
        mc.m_properties = boost::shared_ptr< WProperties >( new WProperties( "Properties", "Module's properties" ) );
        mc.m_savePropGroup = mc.m_properties->addPropertyGroup( "Save Surface",  "" );
        mc.m_meshFile = mc.m_savePropGroup->addProperty( "Mesh File", "", boost::filesystem::path( fileName.c_str() ) );
        mc.m_saveTriggerProp = mc.m_savePropGroup->addProperty( "Do Save",  "Press!", WPVBaseTypes::PV_TRIGGER_READY );

        mc.save();

        bool result = mc.save();
        TS_ASSERT_EQUALS( result, false ); // should return false as we did not have all coordinates values finite.
        TS_ASSERT( !wiotools::fileExists( fileName ) );
    }
};

#endif  // WMMARCHINGCUBES_TEST_H
