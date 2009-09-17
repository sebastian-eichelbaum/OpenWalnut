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

#include <iostream>
#include <fstream>
#include <string>

#include "../kernel/WKernel.h"

#include "WShader.h"

WShader::WShader()
{
}

WShader::WShader( std::string fileName )
{
    m_VertexObject = readShaderFromFile( fileName + ".vs", osg::Shader::VERTEX );
    m_FragmentObject = readShaderFromFile( fileName + ".fs", osg::Shader::FRAGMENT );

    m_ProgramObject = new osg::Program;

    if ( m_FragmentObject )
    {
        m_ProgramObject->addShader( m_FragmentObject );
    }

    if ( m_VertexObject )
    {
        m_ProgramObject->addShader( m_VertexObject );
    }
}

WShader::~WShader()
{
}

osg::Shader* WShader::readShaderFromFile( std::string fileName, osg::Shader::Type type )
{
    std::string fileText = readTextFile( fileName );

    std::cout << fileText << std::endl;

    osg::Shader* shader = new osg::Shader( type, fileText );

    return shader;
}

std::string WShader::readTextFile( std::string fileName )
{
    std::string fileText;

    std::ifstream ifs( (  WKernel::getRunningKernel()->getShaderPath() + fileName ).c_str() );
    std::string line;

    while ( getline( ifs, line ) )
    {
        if ( isIncludeLine( line ) )
        {
            fileText += readTextFile( getIncludeFileName( line ) );
        }
        else
        {
            fileText += line;
            fileText += '\n';
        }
    }

    return fileText;
}

bool WShader::isIncludeLine( std::string line )
{
    if ( line.substr( 0, 8 ) == "#include" )
    {
        return true;
    }
    return false;
}

std::string WShader::getIncludeFileName( std::string line )
{
    size_t pos = 0;
    std::string fileName;

    while ( line[pos++] != '\"' )
    {
    }

    while ( ( pos < line.length() ) &&  ( line[pos] != '\"' ) )
    {
        fileName += line[pos];
        ++pos;
    }

    return fileName;
}

osg::Program* WShader::getProgramObject()
{
    return m_ProgramObject;
}
