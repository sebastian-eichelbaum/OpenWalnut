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

#ifndef WSHADER_H
#define WSHADER_H

#include <string>
#include <map>

#include <osg/Geometry>

#include <boost/shared_ptr.hpp>

/**
 * Class the encapsulates the loading and compiling of shader objects
 * \ingroup ge
 */
class WShader
{
public:
    /**
     * default constructor
     */
    WShader();

    /**
     * constructor with initialization
     *
     * \param fileName the file name
     * \param shaderPath path to the directory where all shader file are stored
     */
    WShader( std::string fileName, std::string shaderPath );

    /**
     * destructor
     */
    virtual ~WShader();



    /**
     * Returns the OSG program object
     *
     * \return the program object
     */
    osg::Program* getProgramObject();

    /**
     * Sets a define which is include into the shader source code.
     * Not yet fully operational
     *
     * \param key The name of the define
     * \param value The value of the define.
     */
    void setDefine( std::string key, float value );

    /**
     * Deletes a define from the internal list
     *
     * \param key The name of the define
     */
    void eraseDefine( std::string key );

private:
    /**
     * Helper routine to load a shader program from a text file
     *
     * \return An osg shader
     *
     * \param fileName the file name
     * \param type The type of the shader, eg. Vertex or fragment shader
     */
    osg::Shader* readShaderFromFile( std::string fileName, osg::Shader::Type type );

    /**
     * Helper routine to load a text file into a string
     *
     * \return String
     *
     * \param fileName The file name
     */
    std::string readTextFile( std::string fileName );

    /**
     * Helper routine to test if a line contains an include statement
     *
     * \return true if line contains an include statement
     *
     * \param line a string with the line
     */
    bool isIncludeLine( std::string line );

    /**
     * Parses a line with an include statement and returns the file name
     *
     * \return string with the file name
     *
     * \param line a string with the line
     */
    std::string getIncludeFileName( std::string line );


    /**
     * String that stores the location of all shader files
     */
    std::string m_shaderPath;
    /**
     * the vertex shader object
     */
    osg::Shader* m_VertexObject;

    /**
     * the fragment shader object
     */
    osg::Shader* m_FragmentObject;

    /**
     * the shader program
     */
    osg::Program* m_ProgramObject;

    /**
     * a map of all set defines
     */
    std::map< std::string, float>m_defines;
};

#endif  // WSHADER_H
