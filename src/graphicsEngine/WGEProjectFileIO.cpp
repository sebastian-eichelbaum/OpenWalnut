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

bool WGEProjectFileIO::parse( std::string line, unsigned int lineNumber )
{
    // regular expressions to parse the line
    static const boost::regex camRe( "^ *CAMERA:([0-9]*):(.*)$" );
    static const boost::regex pmatrixRe( "^ *PROJECTIONMATRIX:\\(([0-9]*),(.*)\\)=(.*)$" );
    static const boost::regex mmatrixRe( "^ *MODELVIEWMATRIX:\\(([0-9]*),(.*)\\)=(.*)$" );

    // use regex to parse it
    boost::smatch matches;  // the list of matches
    if ( boost::regex_match( line, matches, camRe ) )
    {
        // it is a camera line
        // matches[1] is the ID
        // matches[2] is the name of the view/camera
        wlog::debug( "Project Loader [Parser]" ) << "Line " << lineNumber << ": Camera \"" << matches[2] << "\" with ID " << matches[1];

        // store it
        m_cameras[ boost::lexical_cast< unsigned int >( matches[1] ) ] = matches[2];

        return true;
    }
    else if ( boost::regex_match( line, matches, mmatrixRe ) )
    {
        // it is a camera modelview matrix line
        // matches[1] is the ID of the camera
        // matches[2] is the type of matrix definition (currently only "Matrix", later there might be "Eye", "Center", "Up".
        // matches[3] is the matrix line
        wlog::debug( "Project Loader [Parser]" ) << "Line " << lineNumber << ": Camera Modelview Matrix \"" << matches[2]
                                                 << "\" with ID " << matches[1];

        // valid?
        if ( ( matches[2] != "Matrix" ) )
        {
            wlog::error( "Project Loader [Parser]" ) << "Line " << lineNumber << ": Camera Matrix definition type unknown: \"" << matches[2]
                                                     << "\" for camera with ID " << matches[1];
            return true;
        }

        // is there already a matrix for this camera? -> we do not care :-). Overwrite it.
        m_viewMatrices[ boost::lexical_cast< unsigned int >( matches[1] ) ] = osg::Matrixd::identity();

        // parse the matrix -> tokenize it and fill pointer appropriately
        typedef boost::tokenizer<boost::char_separator< char > > tokenizer;
        boost::char_separator< char > sep( ";" );   // separate by ;
        tokenizer tok( boost::lexical_cast< std::string >( matches[3] ), sep );

        // each value must be stored at the proper position
        //double* values = new
        for ( tokenizer::iterator it = tok.begin(); it != tok.end(); ++it )
        {
        }

        return true;
    }

    return false;
}

void WGEProjectFileIO::done()
{
    // apply camera here
}

void WGEProjectFileIO::save( std::ostream& output ) // NOLINT
{
    output << "//////////////////////////////////////////////////////////////////////////////////////////////////////////////////" << std::endl <<
              "// Camera definitions" << std::endl <<
              "//////////////////////////////////////////////////////////////////////////////////////////////////////////////////" << std::endl <<
              std::endl;

    // grab the main camera
    osg::ref_ptr<osg::Camera> cam = WGraphicsEngine::getGraphicsEngine()->getViewer()->getCamera();
    std::string name = WGraphicsEngine::getGraphicsEngine()->getViewer()->getName();
    output << "CAMERA:0:" <<  name << std::endl;

    // get the matrices
    osg::Matrixd view = cam->getViewMatrix();
    osg::Matrixd proj = cam->getProjectionMatrix();

    output << "//Camera Matrices: \"" << name << "\"" << std::endl;
    output << "    MODELVIEWMATRIX:(0,Matrix)=";
    output << view( 0, 0 ) << ";" << view( 0, 1 ) << ";" << view( 0, 2 ) << ";" << view( 0, 3 ) << ";"
           << view( 1, 0 ) << ";" << view( 1, 1 ) << ";" << view( 1, 2 ) << ";" << view( 1, 3 ) << ";"
           << view( 2, 0 ) << ";" << view( 2, 1 ) << ";" << view( 2, 2 ) << ";" << view( 2, 3 ) << ";"
           << view( 3, 0 ) << ";" << view( 3, 1 ) << ";" << view( 3, 2 ) << ";" << view( 3, 3 ) << std::endl;
    output << "    PROJECTIONMATRIX:(0,Matrix)=";
    output << proj( 0, 0 ) << ";" << proj( 0, 1 ) << ";" << proj( 0, 2 ) << ";" << proj( 0, 3 ) << ";"
           << proj( 1, 0 ) << ";" << proj( 1, 1 ) << ";" << proj( 1, 2 ) << ";" << proj( 1, 3 ) << ";"
           << proj( 2, 0 ) << ";" << proj( 2, 1 ) << ";" << proj( 2, 2 ) << ";" << proj( 2, 3 ) << ";"
           << proj( 3, 0 ) << ";" << proj( 3, 1 ) << ";" << proj( 3, 2 ) << ";" << proj( 3, 3 ) << std::endl;
    output << "//Camera Matrices END: \"" << name << "\"" << std::endl;
}

