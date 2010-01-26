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

#include <map>
#include <string>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

#include <osg/StateSet>
#include <osg/Node>

#include "WGraphicsEngine.h"
#include "../common/WLogger.h"

#include "WShader2.h"

WShader2::WShader2( std::string name ):
    osg::Program(),
    m_shaderPath( WGraphicsEngine::getGraphicsEngine()->getShaderPath() ),
    m_name( name ),
    m_reload( true )
{
    // create shader
    m_vertexShader = osg::ref_ptr< osg::Shader >( new osg::Shader( osg::Shader::VERTEX ) );
    m_fragmentShader = osg::ref_ptr< osg::Shader >( new osg::Shader( osg::Shader::FRAGMENT ) );
    m_geometryShader = osg::ref_ptr< osg::Shader >( new osg::Shader( osg::Shader::GEOMETRY ) );

    // add them
    addShader( m_vertexShader );
    addShader( m_fragmentShader );
    addShader( m_geometryShader );
}

WShader2::~WShader2()
{
    // cleanup
}

void WShader2::apply( osg::ref_ptr< osg::Node > node )
{
    // set the shader attribute
    osg::StateSet* rootState = node->getOrCreateStateSet();
    rootState->setAttributeAndModes( this, osg::StateAttribute::ON );

    // add a custom callback which actually sets and updated the shader.
    node->addUpdateCallback( osg::ref_ptr< SafeUpdaterCallback >( new SafeUpdaterCallback( this ) ) );
}

void WShader2::reload()
{
    m_reload = true;
}

WShader2::SafeUpdaterCallback::SafeUpdaterCallback( WShader2* shader ):
    m_shader( shader )
{
}

void WShader2::SafeUpdaterCallback::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    // is it needed to do something here?
    if ( m_shader->m_reload )
    {
        try
        {
            // remove the shaders
            m_shader->removeShader( m_shader->m_vertexShader );
            m_shader->removeShader( m_shader->m_fragmentShader );
            m_shader->removeShader( m_shader->m_geometryShader );

            // reload the sources and set the shader
            // vertex shader
            WLogger::getLogger()->addLogMessage( "Reloading vertex shader \"" + m_shader->m_name + ".vs\"", "WShader", LL_DEBUG );
            std::string source = m_shader->readTextFile( m_shader->m_name + ".vs" );
            if ( source != "" )
            {
                m_shader->m_vertexShader->setShaderSource( source );
                m_shader->addShader( m_shader->m_vertexShader );
            }

            // fragment shader
            WLogger::getLogger()->addLogMessage( "Reloading fragment shader \"" + m_shader->m_name + ".fs\"", "WShader", LL_DEBUG );
            source = m_shader->readTextFile( m_shader->m_name + ".fs" );
            if ( source != "" )
            {
                m_shader->m_fragmentShader->setShaderSource( source );
                m_shader->addShader( m_shader->m_fragmentShader );
            }

            // Geometry Shader
            WLogger::getLogger()->addLogMessage( "Reloading geometry shader \"" + m_shader->m_name + ".gs\"", "WShader", LL_DEBUG );
            source = m_shader->readTextFile( m_shader->m_name + ".gs" );
            if ( source != "" )
            {
                m_shader->m_geometryShader->setShaderSource( source );
                m_shader->addShader( m_shader->m_geometryShader );
            }
        }
        catch( const std::exception& e )
        {
            WLogger::getLogger()->addLogMessage( "Problem loading shader.", "WShader", LL_ERROR );

            // clean up the mess
            m_shader->removeShader( m_shader->m_vertexShader );
            m_shader->removeShader( m_shader->m_fragmentShader );
            m_shader->removeShader( m_shader->m_geometryShader );
        }

        // everything done now.
        m_shader->m_reload = false;
    }

    // forward the call
    traverse( node, nv );
}

std::string WShader2::readTextFile( std::string fileName )
{
    std::string fileText;

    namespace fs = boost::filesystem;

    std::ifstream ifs( ( ( fs::path( m_shaderPath ) / fileName ).file_string() ).c_str() );
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

bool WShader2::isIncludeLine( std::string line )
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

std::string WShader2::getIncludeFileName( std::string line )
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
        WLogger::getLogger()->addLogMessage( "Missing quotes around file name in include statement of shader.", "WShader", LL_ERROR );
        // TODO(schurade): here we could throw an exception
        return "";
    }

    typedef boost::tokenizer< boost::char_separator< char > > tokenizer;

    boost::char_separator<char> sep( "\"" );
    tokenizer tok( line, sep );
    tokenizer::iterator it = tok.begin();
    ++it;
    return *it;
}

void WShader2::setDefine( std::string key, float value )
{
    if ( key.length() > 0 )
    {
        m_defines[key] = value;
    }
}

void WShader2::eraseDefine( std::string key )
{
    m_defines.erase( key );
}

