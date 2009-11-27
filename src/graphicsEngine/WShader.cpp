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
#include <map>
#include <string>

#include <boost/algorithm/string.hpp>
#include "boost/lexical_cast.hpp"
#include <boost/tokenizer.hpp>

#include "WShader.h"
#include "../common/WLogger.h"

WShader::WShader()
{
}

WShader::WShader( std::string fileName, std::string shaderPath ):
    m_shaderPath( shaderPath )
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

//      std::cout << "\n=====OW==SHADER============\n"
//                << fileText
//                << "\n=====OW==SHADER=END========\n"
//                << std::endl;

    osg::Shader* shader = new osg::Shader( type, fileText );

    return shader;
}


std::string WShader::readTextFile( std::string fileName )
{
    std::string fileText;

    std::ifstream ifs( (  m_shaderPath + fileName ).c_str() );
    std::string line;

    std::map< std::string, float >::const_iterator mi = m_defines.begin();

    while ( mi != m_defines.end() )
    {
        fileText += "#define ";
        fileText += mi->first;
        fileText += " ";
        fileText += boost::lexical_cast< std::string, float >( mi->second );
        fileText += '\n';
    }

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
    if( line[0] == '/' && line[1] == '/' )
    {
        return false; // we encountered a comment
    }
    if ( boost::find_first( line, "#include" ) )
    {
        return true;
    }
    return false;
}


std::string WShader::getIncludeFileName( std::string line )
{
    std::string fileName;

    int count = 0;
    for ( size_t i = 0 ; i < line.length() ; ++i )
    {
        if ( line[i] == '\"' )
        {
           ++count;
        }
    }
    if ( count < 2 )
    {
        WLogger::getLogger()->addLogMessage( "Missing quotes around file name in include statement of shader.", "Marching Cubes", LL_ERROR );
        // TODO(schurade): here we could throw an exception
        return 0;
    }

    typedef boost::tokenizer< boost::char_separator< char > > tokenizer;

    boost::char_separator<char> sep( "\"" );
    tokenizer tok( line, sep );
    tokenizer::iterator it = tok.begin();
    ++it;
    return *it;
}


osg::Program* WShader::getProgramObject()
{
    return m_ProgramObject;
}

void WShader::setDefine( std::string key, float value )
{
    if ( key.length() > 0 )
    {
        m_defines[key] = value;
    }
}


void WShader::eraseDefine( std::string key )
{
    m_defines.erase( key );
}

