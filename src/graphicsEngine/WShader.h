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

#include <osg/Geometry>

#include <boost/shared_ptr.hpp>

class WShader
{
public:
    /**
     * default constructor
     */
    WShader();

    /**
     *
     */
    explicit WShader( std::string fileName );

    /**
     * destructor
     */
    virtual ~WShader();



    /**
     *
     */
    osg::Program* getProgramObject();


private:
    /**
     *
     */
    osg::Shader* readShaderFromFile( std::string fileName, osg::Shader::Type type );

    /**
     *
     */
    std::string readTextFile( std::string fileName );

    /**
     *
     */
    bool isIncludeLine( std::string line );

    /**
     *
     */
    std::string getIncludeFileName( std::string line );

    /**
     *
     */
    osg::Shader* m_VertexObject;

    /**
     *
     */
    osg::Shader* m_FragmentObject;

    /**
     *
     */
    osg::Program* m_ProgramObject;
};

#endif  // WSHADER_H
