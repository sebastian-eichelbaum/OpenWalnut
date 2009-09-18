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

#ifndef WLOADERVTK_TEST_H
#define WLOADERVTK_TEST_H

#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <cxxtest/TestSuite.h>

#include "../WLoaderVTK.h"
#include "../../WDataHandler.h"

/**
 * Unit tests the WLoaderVTK class.
 */
class WLoaderVTKTest : public CxxTest::TestSuite
{
public:
    /**
     * Create test environment.
     */
    void setUp( void )
    {
        m_dataHandler = boost::shared_ptr< WDataHandler >( new WDataHandler() );
        m_vtkFile = boost::filesystem::path( "fixtures/VTK/ascii-2nd-order-tensor.vtk" );
        assert( boost::filesystem::exists( m_vtkFile ) );
    }

    /**
     * A valid "header" of a VTK file consists of 4 lines of text (this is
     * arbitrary specified by me, the author of this class)
     * 1. VTK version stuff
     * 2. VTK description header stuff (max 256 chars)
     * 3. BINARY or ASCII
     * 4. DATASET type
     * 
     * Further description about the DATASET e.g. ORIGIN in case of
     * STRUCTURED_POINTS does not belong to the header.
     *
     * FYI: It makes really sense to restrict the header to 4 lines since since
     * every VTK file must have at least 4 lines describing in ASCII what comes
     * next.
     */
    void testReadHeader( void )
    {
        WLoaderVTK loader( m_vtkFile.string(), m_dataHandler );
        loader.readHeader();
        std::vector< std::string > expected;
        expected.push_back( "# vtk DataFile Version 3.0" );
        expected.push_back( "vtk output" );
        expected.push_back( "ASCII" );
        expected.push_back( "DATASET STRUCTURED_POINTS" );
        TS_ASSERT_EQUALS( loader.m_header, expected );
    }

    /**
     * If the path is not valid there should be thrown an exception
     */
    void testReadHeaderOnInvalidFile( void )
    {
        WLoaderVTK loader( "no/such/file", m_dataHandler );
        TS_ASSERT_THROWS_EQUALS( loader.readHeader(),
                                 const WDHIOFailure &e,
                                 e.what(),
                                 std::string( "Invalid file or filename: no/such/file" ) );
    }

    /**
     * If the header introduces a STRUCTURED_POINTS VTK DATASET then true
     * should be returned.
     */
    void testCheckDatasetType( void )
    {
        WLoaderVTK loader( m_vtkFile.string(), m_dataHandler );
        loader.readHeader();
        TS_ASSERT_EQUALS( loader.datasetTypeIs( "STRUCTURED_POINTS" ), true );
    }

    /**
     * A VTK file is typically either in BINARY or ASCII format.
     */
    void testBinaryOrAscii( void )
    {
        WLoaderVTK loader( m_vtkFile.string(), m_dataHandler );
        loader.readHeader();
        TS_ASSERT_EQUALS( loader.isBinary(), false );
    }

private:
    /**
     * Dummy DataHandler instance
     */
    boost::shared_ptr< WDataHandler > m_dataHandler;

    /**
     * A standard VTK file.
     */
    boost::filesystem::path m_vtkFile;
};

#endif  // WLOADERVTK_TEST_H
