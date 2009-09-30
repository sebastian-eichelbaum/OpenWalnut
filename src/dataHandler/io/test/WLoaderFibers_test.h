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

#ifndef WLOADERFIBERS_TEST_H
#define WLOADERFIBERS_TEST_H

#include <sstream>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <cxxtest/TestSuite.h>
#include <cxxtest/ValueTraits.h>

#include "../WLoaderFibers.h"
#include "../../WDataHandler.h"

// New value trait for std::streampos
#ifdef CXXTEST_RUNNING
namespace CxxTest
{
    CXXTEST_TEMPLATE_INSTANTIATION
    class ValueTraits< std::streampos >
    {
        char _s[256];

        public:  // NOLINT
        explicit ValueTraits( const std::streampos &pos )
        {
            std::stringstream ss;
            ss << pos;
            snprintf( _s, ss.str().size(), "%s", ss.str().c_str() );
        }
        const char *asString() const
        {
            return _s;
        }
    };
}
#endif  // CXXTEST_RUNNING

/**
 * Unit tests the WLoaderFibers class.
 */
class WLoaderFibersTest : public CxxTest::TestSuite
{
public:
    /**
     * Create test environment.
     */
    void setUp( void )
    {
        m_dataHandler = boost::shared_ptr< WDataHandler >( new WDataHandler() );
        m_vtkFile = boost::filesystem::path( "fixtures/VTK/ascii-2nd-order-tensor.vtk" );
        m_ifs_ptr = NULL;
        m_ifs_ptr = new std::ifstream( m_vtkFile.string().c_str(), std::ifstream::in | std::ifstream::binary );
        assert( m_ifs_ptr );
        m_fibLoader = new WLoaderFibers( m_vtkFile.string(), m_dataHandler );
        assert( boost::filesystem::exists( m_vtkFile ) );
    }

    /**
     * Tidy up things as e.g. open file streams.
     */
    void tearDown( void )
    {
        assert( m_ifs_ptr );
        (*m_ifs_ptr).close();
        delete( m_ifs_ptr );
        m_ifs_ptr = NULL;
        delete( m_fibLoader );
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
        m_fibLoader->readHeader( m_ifs_ptr );
        std::vector< std::string > expected;
        expected.push_back( "# vtk DataFile Version 3.0" );
        expected.push_back( "vtk output" );
        expected.push_back( "ASCII" );
        expected.push_back( "DATASET STRUCTURED_POINTS" );
        TS_ASSERT_EQUALS( m_fibLoader->m_header, expected );
    }

    /**
     * After reading the header the position in the stream should have
     * changed
     */
    void testReadHeaderReturnsCorrectStreamPosistion( void )
    {
        m_fibLoader->readHeader( m_ifs_ptr );
        TS_ASSERT_EQUALS( m_ifs_ptr->tellg(), 70 );
    }

    /**
     * If the header introduces a STRUCTURED_POINTS VTK DATASET then true
     * should be returned.
     */
    void testCheckDatasetType( void )
    {
        m_fibLoader->readHeader( m_ifs_ptr );
        TS_ASSERT_EQUALS( m_fibLoader->datasetTypeIs( "STRUCTURED_POINTS" ), true );
    }

    /**
     * A VTK file is typically either in BINARY or ASCII format.
     */
    void testBinaryOrAscii( void )
    {
        m_fibLoader->readHeader( m_ifs_ptr );
        TS_ASSERT_EQUALS( m_fibLoader->isBinary(), false );
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

    /**
     * Emulation of ifstream
     */
    std::ifstream *m_ifs_ptr;

    /**
     * Instanciating an loader
     */
    WLoaderFibers *m_fibLoader;
};

#endif  // WLOADERFIBERS_TEST_H
