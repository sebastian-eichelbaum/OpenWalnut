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
#include <boost/shared_ptr.hpp>
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
private:
    std::string _s;

public:
    explicit ValueTraits( const std::streampos &pos )
    {
        std::stringstream ss;
        ss << pos;
        _s = ss.str();
    }
    const char *asString() const
    {
        return _s.c_str();
    }
};
}
#endif  // CXXTEST_RUNNING

// New value trait for wmath::WFiber
#ifdef CXXTEST_RUNNING
namespace CxxTest
{
CXXTEST_TEMPLATE_INSTANTIATION
class ValueTraits< wmath::WFiber >
{
private:
    std::string _s;

public:
    explicit ValueTraits( const wmath::WFiber &fib )
    {
        std::stringstream ss;
        ss << fib;
        _s = ss.str();
    }
    const char *asString() const
    {
        return _s.c_str();
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
        WLoaderFibers loader( "fixtures/Fibers/valid_small_example.fib", m_dataHandler );
        std::vector< std::string > expected;
        expected.push_back( "# vtk DataFile Version 3.0" );
        expected.push_back( "Neural Pathways aka as fibers." );
        expected.push_back( "BINARY" );
        expected.push_back( "DATASET POLYDATA" );
        loader.readHeader();
        TS_ASSERT_EQUALS( loader.m_header, expected );
    }

    /**
     * A valid .fib header starts with "# vtk DataFile Version 3.0".
     */
    void testUnsupportedVTKFileFormatString( void )
    {
        WLoaderFibers loader( "fixtures/Fibers/unsupported_format_version_string.fib", m_dataHandler );
        TS_ASSERT_THROWS_EQUALS( loader.readHeader(),
                                 const WDHException &e,
                                 e.what(),
                                 std::string( "Unsupported format version string: # vtk DataFile Version 9.0" ) );
    }

    /**
     * If the internal VTK header is too big an error is thrown
     */
    void testReadHeaderOnTooBigVTKHeader( void )
    {
        WLoaderFibers loader( "fixtures/Fibers/invalid_header_length.fib", m_dataHandler );
        TS_ASSERT_THROWS_EQUALS( loader.readHeader(),
                                 const WDHException &e,
                                 e.what(),
                                 std::string( "VTK header too big: 261" ) );
    }

    /**
     * ATM we only support BINARY VTK files.
     */
    void testBinaryOrAscii( void )
    {
        WLoaderFibers loader( "fixtures/Fibers/ascii.fib", m_dataHandler );
        TS_ASSERT_THROWS_EQUALS( loader.readHeader(),
                                 const WDHException &e,
                                 e.what(),
                                 std::string( "VTK files in 'ASCII' format are not yet supported" ) );
    }

    /**
     * If the header not introduces a POLYDATA DATASET an exception should be
     * thrown.
     */
    void testCheckDatasetType( void )
    {
        WLoaderFibers loader( "fixtures/Fibers/invalid_dataset.fib", m_dataHandler );
        TS_ASSERT_THROWS_EQUALS( loader.readHeader(),
                                 const WDHException &e,
                                 e.what(),
                                 std::string( "Invalid VTK DATASET type: STRUCTURED_POINTS" ) );
    }

    /**
     * After reading the header the position in the stream should have
     * changed
     */
    void testStreamPosIsValidAfterReadHeaderCall( void )
    {
        WLoaderFibers loader( "fixtures/Fibers/valid_small_example.fib", m_dataHandler );
        loader.readHeader();
        TS_ASSERT_EQUALS( loader.m_ifs->tellg(), 82 );
    }

    /**
     * If there is no header at all an exception should be thrown.
     */
    void testNoHeaderThere( void )
    {
        WLoaderFibers loader( "fixtures/Fibers/no_header.fib", m_dataHandler );
        TS_ASSERT_THROWS( loader.readHeader(), WDHException );
    }

    /**
     * If there is a crippled header then an exception should be thrown
     */
    void testOnAbruptHeader( void )
    {
        WLoaderFibers loader( "fixtures/Fibers/crippled_header.fib", m_dataHandler );
        TS_ASSERT_THROWS_EQUALS( loader.readHeader(),
                                 const WDHException &e,
                                 e.what(),
                                 std::string( "Unexpected end of file: fixtures/Fibers/crippled_header.fib" ) );
    }

    /**
     * After reading the points the point vector of WLoaderFibers should be
     * filled with valid numbers.
     */
    void testReadPoints( void )
    {
        WLoaderFibers loader( "fixtures/Fibers/valid_small_example.fib", m_dataHandler );
        loader.readHeader();
        loader.readPoints();
        double delta = 0.0001;
        TS_ASSERT_EQUALS( loader.m_points.size(), 1224 );
        TS_ASSERT_DELTA( loader.m_points.at( 1223 )[2], 60.4135, delta );
        TS_ASSERT_DELTA( loader.m_points.at( 0 )[0], 46.2582, delta );
        TS_ASSERT_DELTA( loader.m_points.at( 0 )[1], 36.7184, delta );
        TS_ASSERT_DELTA( loader.m_points.at( 0 )[2], 65.7245, delta );
    }

    /**
     * Every line is stored in fib files with
     * 1. Its number of points (length)
     * 2. All references to the points its using
     */
    void testReadAllLines( void )
    {
        using boost::shared_ptr;
        using std::vector;
        using wmath::WFiber;
        using wmath::WPosition;
        WLoaderFibers loader( "fixtures/Fibers/small_example_one_fiber.fib", m_dataHandler );
        loader.readHeader();
        loader.readPoints();
        shared_ptr< vector< WFiber > > actual = loader.readLines();
        TS_ASSERT_EQUALS( actual->size(), 1 );
        vector< WPosition > pointList;
        // CAUTION: we use here floats since the positions are stored with
        // floats as well. If we would use doubles here, the both vectors are
        // different then.
        pointList.push_back( WPosition( 1.2f, 3.4f, 5.6f ) );
        pointList.push_back( WPosition( 7.8f, 9.0f, -1.2f ) );
        WFiber expected( pointList );
        TS_ASSERT_EQUALS( expected, actual->back() );
    }

    /**
     * When e.g. the CopyConstructor is invoked, all internals (including)
     * all pointers will be destructed and deleted.
     */
    void testDestructor( void )
    {
        std::vector< WLoaderFibers > loaders;
        {
            WLoaderFibers loader( "fixtures/Fibers/valid_small_example.fib", m_dataHandler );
            loaders.push_back( loader );
            // destructor invoked
        }
        TS_ASSERT( loaders[0].m_ifs->is_open() );
        loaders.clear();  // second destruction
    }

    /**
     * The () operator normaly starts the thread, so we check here if every thing
     * terminates quite well.
     */
    void testBracesOperatorTerminatesWell( void )
    {
        TS_ASSERT_EQUALS( m_dataHandler->getNumberOfSubjects(), 0 );
        WLoaderFibers loader( "fixtures/Fibers/valid_small_example.fib", m_dataHandler );
        loader.load();
        TS_ASSERT_EQUALS( m_dataHandler->getNumberOfSubjects(), 1 );
    }

private:
    /**
     * Dummy DataHandler instance
     */
    boost::shared_ptr< WDataHandler > m_dataHandler;
};

#endif  // WLOADERFIBERS_TEST_H
