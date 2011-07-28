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

#include <string>

#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

#include <osg/Camera>

#include "../common/WLogger.h"
#include "WGraphicsEngine.h"
#include "WGEViewer.h"

#include "WGEProjectFileIO.h"

WGEProjectFileIO::WGEProjectFileIO():
    WProjectFileIO()
{
    // initialize members
}

WGEProjectFileIO::~WGEProjectFileIO()
{
    // cleanup
}

/**
 * Interpret a string as a ";"- separated sequence of doubles.
 *
 * \param seq   the sequence
 * \param size  the number of doubles needed
 *
 * \return the values
 */
double* parseDoubleSequence( std::string seq, unsigned int size )
{
    // parse the string
    // -> tokenize it and fill pointer appropriately
    typedef boost::tokenizer<boost::char_separator< char > > tokenizer;
    boost::char_separator< char > sep( ";" );   // separate by ;
    tokenizer tok( seq, sep );

    // each value must be stored at the proper position
    double* values = new double[ size ];
    unsigned int i = 0;
    for( tokenizer::iterator it = tok.begin(); ( it != tok.end() ) && ( i < size ); ++it )
    {
        values[ i ] = boost::lexical_cast< double >( ( *it ) );
        ++i;
    }

    // finally, set the values
    return values;
}

/**
 * Parse a string and interpret it as matrix. It creates a array of 16 values representing a 4x4 matrix in OSG notation.
 *
 * \param matrix the matrix as string.
 *
 * \return the parsed values.
 */
double* parseMatrix( std::string matrix )
{
    return parseDoubleSequence( matrix, 16 );
}

/**
 * Parse a string and interpret it as vector. It creates an array of 3 values representing the vector in OSG notation-
 *
 * \param vec the string to parse
 *
 * \return the values.
 */
double* parseVector( std::string vec )
{
    return parseDoubleSequence( vec, 3 );
}

bool WGEProjectFileIO::parse( std::string line, unsigned int lineNumber )
{
    // regular expressions to parse the line
    static const boost::regex camRe( "^ *CAMERA:([0-9]*):(.*)$" );
    static const boost::regex matrixRe(     "^ *MANIPULATOR:\\(([0-9]*),Matrix\\)=(.*)$" );
    static const boost::regex homeEyeRe(    "^ *MANIPULATOR:\\(([0-9]*),HomeEye\\)=(.*)$" );
    static const boost::regex homeCenterRe( "^ *MANIPULATOR:\\(([0-9]*),HomeCenter\\)=(.*)$" );
    static const boost::regex homeUpRe(     "^ *MANIPULATOR:\\(([0-9]*),HomeUp\\)=(.*)$" );

    // use regex to parse it
    boost::smatch matches;  // the list of matches
    if( boost::regex_match( line, matches, camRe ) )
    {
        // it is a camera line
        // matches[1] is the ID
        // matches[2] is the name of the view/camera
        wlog::debug( "Project Loader [Parser]" ) << "Line " << lineNumber << ": Camera \"" << matches[2] << "\" with ID " << matches[1];

        // store it
        m_cameras[ boost::lexical_cast< unsigned int >( matches[1] ) ] = matches[2];

        return true;
    }
    else if( boost::regex_match( line, matches, matrixRe ) )
    {
        // it is a camera modelview matrix line
        // matches[1] is the ID of the camera
        // matches[2] is the matrix line
        wlog::debug( "Project Loader [Parser]" ) << "Line " << lineNumber << ": Camera Manipulator Matrix with ID " << matches[1];

        // is there already a matrix for this camera? -> we do not care :-). Overwrite it.
        m_manipulatorMatrices[ boost::lexical_cast< unsigned int >( matches[1] ) ] =
            osg::Matrixd( parseMatrix( boost::lexical_cast< std::string >( matches[2] ) ) );

        return true;
    }
    else if( boost::regex_match( line, matches, homeEyeRe ) )
    {
        // it is a eye point of the manipulator

        // matches[1] is the ID of the camera
        // matches[2] is the eye vector
        wlog::debug( "Project Loader [Parser]" ) << "Line " << lineNumber << ": Camera Manipulator Home Eye Point with ID " << matches[1];

        // is there already a vector set? -> ignore.
        double* vals = parseVector( boost::lexical_cast< std::string >( matches[2] ) );
        m_homeEyeVectors[ boost::lexical_cast< unsigned int >( matches[1] ) ] = osg::Vec3d( vals[0], vals[1], vals[2] );

        return true;
    }
    else if( boost::regex_match( line, matches, homeCenterRe ) )
    {
        // it is a center point of the manipulator

        // matches[1] is the ID of the camera
        // matches[2] is the eye vector
        wlog::debug( "Project Loader [Parser]" ) << "Line " << lineNumber << ": Camera Manipulator Home Center Point with ID " << matches[1];

        // is there already a vector set? -> ignore.
        double* vals = parseVector( boost::lexical_cast< std::string >( matches[2] ) );
        m_homeCenterVectors[ boost::lexical_cast< unsigned int >( matches[1] ) ] = osg::Vec3d( vals[0], vals[1], vals[2] );

        return true;
    }
    else if( boost::regex_match( line, matches, homeUpRe ) )
    {
        // it is a up vector of the manipulator

        // matches[1] is the ID of the camera
        // matches[2] is the eye vector
        wlog::debug( "Project Loader [Parser]" ) << "Line " << lineNumber << ": Camera Manipulator Home Up Point with ID " << matches[1];

        // is there already a vector set? -> ignore.
        double* vals = parseVector( boost::lexical_cast< std::string >( matches[2] ) );
        m_homeUpVectors[ boost::lexical_cast< unsigned int >( matches[1] ) ] = osg::Vec3d( vals[0], vals[1], vals[2] );

        return true;
    }

    return false;
}

void WGEProjectFileIO::done()
{
    // apply camera here
    for( CameraList::const_iterator iter = m_cameras.begin(); iter != m_cameras.end(); ++iter )
    {
        // does the corresponding view exists?
        boost::shared_ptr< WGEViewer > view = WGraphicsEngine::getGraphicsEngine()->getViewerByName( ( *iter ).second );
        if( !view )
        {
            wlog::warn( "Project Loader" ) << "Project file contained a camera \"" << ( *iter ).second << "\" but the corresponding view does " <<
                                              "not exist. Ignoring.";
        }

        // did we have a matrix?
        if( !m_manipulatorMatrices.count( ( *iter ).first ) )
        {
            wlog::warn( "Project Loader" ) << "Project file contained a camera \"" << ( *iter ).second << "\" but no proper manipulator matrix. " <<
                                              "Leaving current matrix untouched.";
        }
        else
        {
            view->getCameraManipulator()->setByMatrix( m_manipulatorMatrices[ ( *iter ).first ] );
        }

        // home position found?
        if( ( !m_homeEyeVectors.count( ( *iter ).first ) ) ||
             ( !m_homeCenterVectors.count( ( *iter ).first ) ) ||
             ( !m_homeUpVectors.count( ( *iter ).first ) )
           )
        {
            wlog::warn( "Project Loader" ) << "Project file contained a camera \"" << ( *iter ).second << "\" but no proper manipulator home " <<
                                              "position. Leaving current home untouched.";
        }
        else
        {
            view->getCameraManipulator()->setHomePosition( m_homeEyeVectors[ ( *iter ).first ],
                                                           m_homeCenterVectors[ ( *iter ).first ],
                                                           m_homeUpVectors[ ( *iter ).first ]
            );
        }
    }
}

void WGEProjectFileIO::save( std::ostream& output ) // NOLINT
{
    output << "//////////////////////////////////////////////////////////////////////////////////////////////////////////////////" << std::endl <<
              "// Camera definitions" << std::endl <<
              "//////////////////////////////////////////////////////////////////////////////////////////////////////////////////" << std::endl <<
              std::endl;

    // grab the main camera
    osg::ref_ptr< osg::Camera > cam = WGraphicsEngine::getGraphicsEngine()->getViewer()->getCamera();
    osg::ref_ptr< osgGA::MatrixManipulator > mani = WGraphicsEngine::getGraphicsEngine()->getViewer()->getCameraManipulator();
    std::string name = WGraphicsEngine::getGraphicsEngine()->getViewer()->getName();
    output << "CAMERA:0:" <<  name << std::endl;

    // NOTE: OSG uses manipulators to modify the camera's view and projection matrix every frame. -> it is useless to store view and projection
    // matrix as these get overwritten every frame -> we need to store the manipulator settings here.

    // get the matrices
    osg::Matrixd view = mani->getMatrix();

    output << "//Camera Matrices: \"" << name << "\"" << std::endl;
    output << "    MANIPULATOR:(0,Matrix)=";
    for( unsigned int i = 0; i < 16; ++i )
    {
        output << view.ptr()[i];
        if( i < 15 )
        {
            output << ";";
        }
    }
    output << std::endl;

    // get home position
    osg::Vec3d eye;
    osg::Vec3d center;
    osg::Vec3d up;
    mani->getHomePosition( eye, center, up );

    // write them
    // NOTE: vec3d already provides a << operator but it writes the vector in a space separated style -> this is not conform to our common style
    // -> we manually print them
    output << "    MANIPULATOR:(0,HomeEye)=";
    output << eye.x() << ";" << eye.y() << ";" << eye.z() << std::endl;
    output << "    MANIPULATOR:(0,HomeCenter)=";
    output << center.x() << ";" << center.y() << ";" << center.z() << std::endl;
    output << "    MANIPULATOR:(0,HomeUp)=";
    output << up.x() << ";" << up.y() << ";" << up.z() << std::endl;

    output << "//Camera Matrices END: \"" << name << "\"" << std::endl;
}

