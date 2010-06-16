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
#include <sstream>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/regex.hpp>
#include <boost/signals2/signal.hpp>
#include <boost/function.hpp>

#include <osg/StateSet>
#include <osg/Node>

#include "WGraphicsEngine.h"
#include "../common/WLogger.h"

#include "WShader.h"

WShader::WShader( std::string name ):
    osg::Program(),
    m_shaderPath( WGraphicsEngine::getGraphicsEngine()->getShaderPath() ),
    m_name( name ),
    m_reload( true ),
    m_shaderLoaded( false ),
    m_deactivated( false )
{
    // create shader
    m_vertexShader = osg::ref_ptr< osg::Shader >( new osg::Shader( osg::Shader::VERTEX ) );
    m_fragmentShader = osg::ref_ptr< osg::Shader >( new osg::Shader( osg::Shader::FRAGMENT ) );
    m_geometryShader = osg::ref_ptr< osg::Shader >( new osg::Shader( osg::Shader::GEOMETRY ) );

    // add them
    addShader( m_vertexShader );
    addShader( m_fragmentShader );
    addShader( m_geometryShader );

    m_reloadSignalConnection = WGraphicsEngine::getGraphicsEngine()->subscribeSignal( GE_RELOADSHADERS, boost::bind( &WShader::reload, this ) );
}

WShader::~WShader()
{
    // cleanup
    m_reloadSignalConnection.disconnect();
}

void WShader::apply( osg::ref_ptr< osg::Node > node )
{
    // set the shader attribute
    // NOTE: the attribute is protected to avoid father nodes overwriting it
    osg::StateSet* rootState = node->getOrCreateStateSet();
    rootState->setAttributeAndModes( this, osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );
    m_deactivated = false;
    m_reload = !m_shaderLoaded;

    // add a custom callback which actually sets and updated the shader.
    node->addUpdateCallback( osg::ref_ptr< SafeUpdaterCallback >( new SafeUpdaterCallback( this ) ) );
}

void WShader::deactivate( osg::ref_ptr< osg::Node > node )
{
    // set the shader attribute
    // NOTE: the attribute is protected to avoid father nodes overwriting it
    osg::StateSet* rootState = node->getOrCreateStateSet();
    rootState->setAttributeAndModes( this, osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );

    m_deactivated = true;
    m_shaderLoaded = false;

    // add a custom callback which actually sets and updated the shader.
    node->addUpdateCallback( osg::ref_ptr< SafeUpdaterCallback >( new SafeUpdaterCallback( this ) ) );
}

void WShader::reload()
{
    m_reload = true;
}

WShader::SafeUpdaterCallback::SafeUpdaterCallback( WShader* shader ):
    m_shader( shader )
{
}

void WShader::SafeUpdaterCallback::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    // is it needed to do something here?
    if ( m_shader->m_deactivated )
    {
        // remove the shaders
        m_shader->removeShader( m_shader->m_vertexShader );
        m_shader->removeShader( m_shader->m_fragmentShader );
        m_shader->removeShader( m_shader->m_geometryShader );
    }

    else if ( m_shader->m_reload )
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
            std::string source = m_shader->processShader( m_shader->m_name + ".vs" );
            if ( source != "" )
            {
                m_shader->m_vertexShader->setShaderSource( source );
                m_shader->addShader( m_shader->m_vertexShader );
            }

            // fragment shader
            WLogger::getLogger()->addLogMessage( "Reloading fragment shader \"" + m_shader->m_name + ".fs\"", "WShader", LL_DEBUG );
            source = m_shader->processShader( m_shader->m_name + ".fs" );
            if ( source != "" )
            {
                m_shader->m_fragmentShader->setShaderSource( source );
                m_shader->addShader( m_shader->m_fragmentShader );
            }

            // Geometry Shader
            WLogger::getLogger()->addLogMessage( "Reloading geometry shader \"" + m_shader->m_name + ".gs\"", "WShader", LL_DEBUG );
            source = m_shader->processShader( m_shader->m_name + ".gs", true );
            if ( source != "" )
            {
                m_shader->m_geometryShader->setShaderSource( source );
                m_shader->addShader( m_shader->m_geometryShader );
            }

            m_shader->m_shaderLoaded = true;
        }
        catch( const std::exception& e )
        {
            m_shader->m_shaderLoaded = false;

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

std::string WShader::processShader( const std::string filename, bool optional, int level )
{
    std::stringstream output;    // processed output

    if ( level == 0 )
    {
        // for the shader (not the included one, for which level != 0)

        // apply defines
        for ( std::map< std::string, float >::const_iterator mi = m_defines.begin(); mi != m_defines.end(); ++mi )
        {
            output << "#define " << mi->first << " " << boost::lexical_cast< std::string, float >( mi->second ) << std::endl;
        }
    }

    // we encountered an endless loop
    if ( level > 32 )
    {
        // reached a certain level. This normally denotes a inclusion cycle.
        // We do not throw an exception here to avoid OSG to crash.
        WLogger::getLogger()->addLogMessage( "Inclusion depth is too large. Maybe there is a inclusion cycle in the shader code.",
                "WShader (" + filename + ")", LL_ERROR
        );

        // just return unprocessed source
        return "";
    }

    // this is the proper regular expression for includes. This also excludes commented includes
    static const boost::regex re( "^[ ]*#[ ]*include[ ]+[\"<](.*)[\">].*" );
    // this is an expression for the #version statement
    static const boost::regex ver( "^[ ]*#[ ]*version[ ]+[123456789][0123456789]+.*$" );

    // the input stream
    std::string fn = ( boost::filesystem::path( m_shaderPath ) / filename ).file_string();
    std::ifstream input( fn.c_str() );
    if ( !input.is_open() )
    {
        if ( optional )
        {
            return "";
        }

        // file does not exist. Do not throw an exception to avoid OSG crash
        if ( level == 0 )
        {
            WLogger::getLogger()->addLogMessage( "Can't open shader file \"" + fn + "\".",
                    "WShader (" + filename + ")", LL_ERROR
            );
        }
        else
        {
            WLogger::getLogger()->addLogMessage( "Can't open shader file for inclusion \"" + fn + "\".",
                    "WShader (" + filename + ")", LL_ERROR
            );
        }

        return "";
    }

    // go through each line and process includes
    std::string line;               // the current line
    boost::smatch matches;          // the list of matches
    std::string versionLine;        // the version
    bool foundVersion = false;

    while ( std::getline( input, line ) )
    {
        if ( boost::regex_search( line, matches, re ) )
        {
            output << processShader( matches[1], false, level + 1 );
        }
        else if( boost::regex_match( line, ver ) ) // look for the #version statement
        {
            // there already was a version statement in this file
            // this does not track multiple version statements through included files
            if( foundVersion )
            {
                WLogger::getLogger()->addLogMessage( "Multiple version statements in shader file \"" + fn + "\".",
                        "WShader (" + filename + ")", LL_ERROR
                );
                return "";
            }
            versionLine = line;
            foundVersion = true;
        }
        else
        {
            output << line;
        }

        output << std::endl;
    }

    input.close();

    // no version statement found
    if( !foundVersion )
    {
        return output.str();
    }

    // the ATI compiler needs the version statement to be the first statement in the shader
    std::stringstream vs;
    vs << versionLine.c_str() << std::endl << output.str();
    return vs.str();
}

void WShader::setDefine( std::string key, float value )
{
    if ( key.length() > 0 )
    {
        m_defines[key] = value;
        m_reload = true;
    }
}

void WShader::eraseDefine( std::string key )
{
    m_defines.erase( key );
    m_reload = true;
}

void WShader::eraseAllDefines()
{
    m_defines.clear();
    m_reload = true;
}

