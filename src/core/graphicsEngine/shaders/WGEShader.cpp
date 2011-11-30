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
#include <ostream>
#include <sstream>
#include <string>
#include <fstream>

#include <boost/algorithm/string.hpp>
// Use filesystem version 2 for compatibility with newer boost versions.
#ifndef BOOST_FILESYSTEM_VERSION
    #define BOOST_FILESYSTEM_VERSION 2
#endif
#include <boost/filesystem.hpp>
#include <boost/function.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <boost/signals2/signal.hpp>
#include <boost/tokenizer.hpp>

#include <osg/Node>
#include <osg/StateSet>

#include "../../common/WLogger.h"
#include "../../common/WPathHelper.h"
#include "../../common/WPredicateHelper.h"
#include "../WGraphicsEngine.h"
#include "WGEShader.h"
#include "WGEShaderPreprocessor.h"
#include "WGEShaderVersionPreprocessor.h"

WGEShader::WGEShader( std::string name, boost::filesystem::path search ):
    osg::Program(),
    m_shaderPath( search ),
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

    // this preprocessor is always needed. It removes the #version statement from the code and puts it to the beginning.
    m_versionPreprocessor = WGEShaderPreprocessor::SPtr( new WGEShaderVersionPreprocessor() );

    m_reloadSignalConnection = WGraphicsEngine::getGraphicsEngine()->subscribeSignal( GE_RELOADSHADERS, boost::bind( &WGEShader::reload, this ) );
}

WGEShader::~WGEShader()
{
    // cleanup
    m_reloadSignalConnection.disconnect();
}

void WGEShader::apply( osg::ref_ptr< osg::Node > node )
{
    // set the shader attribute
    // NOTE: the attribute is protected to avoid father nodes overwriting it
    osg::StateSet* rootState = node->getOrCreateStateSet();
    rootState->setAttributeAndModes( this, osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );
    m_deactivated = false;
    m_reload = m_reload || !m_shaderLoaded;

    // add a custom callback which actually sets and updated the shader.
    node->addUpdateCallback( osg::ref_ptr< SafeUpdaterCallback >( new SafeUpdaterCallback( this ) ) );
}

void WGEShader::applyDirect( osg::State& state ) // NOLINT <- ensure this matches the official OSG API by using a non-const ref
{
    updatePrograms();
    osg::Program::apply( state );
}

void WGEShader::deactivate( osg::ref_ptr< osg::Node > node )
{
    if( !node )
    {
        return;
    }

    // set the shader attribute
    // NOTE: the attribute is protected to avoid father nodes overwriting it
    osg::StateSet* rootState = node->getOrCreateStateSet();
    rootState->setAttributeAndModes( this, osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );

    m_deactivated = true;
    m_shaderLoaded = false;

    // add a custom callback which actually sets and updated the shader.
    node->addUpdateCallback( osg::ref_ptr< SafeUpdaterCallback >( new SafeUpdaterCallback( this ) ) );
}

void WGEShader::reload()
{
    m_reload = true;
}

void WGEShader::reloadShader()
{
    try
    {
        // remove the shaders
        removeShader( m_vertexShader );
        removeShader( m_fragmentShader );
        removeShader( m_geometryShader );

        // reload the sources and set the shader
        // vertex shader
        WLogger::getLogger()->addLogMessage( "Reloading vertex shader \"" + m_name + "-vertex.glsl\"", "WGEShader", LL_DEBUG );
        std::string source = processShader( m_name + "-vertex.glsl" );
        if( source != "" )
        {
            m_vertexShader->setShaderSource( source );
            addShader( m_vertexShader );
        }

        // fragment shader
        WLogger::getLogger()->addLogMessage( "Reloading fragment shader \"" + m_name + "-fragment.glsl\"", "WGEShader", LL_DEBUG );
        source = processShader( m_name + "-fragment.glsl" );
        if( source != "" )
        {
            m_fragmentShader->setShaderSource( source );
            addShader( m_fragmentShader );
        }

        // Geometry Shader
        WLogger::getLogger()->addLogMessage( "Reloading geometry shader \"" + m_name + "-geometry.glsl\"", "WGEShader", LL_DEBUG );
        source = processShader( m_name + "-geometry.glsl", true );
        if( source != "" )
        {
            m_geometryShader->setShaderSource( source );
            addShader( m_geometryShader );
        }

        m_shaderLoaded = true;
    }
    catch( const std::exception& e )
    {
        m_shaderLoaded = false;

        WLogger::getLogger()->addLogMessage( "Problem loading shader.", "WGEShader", LL_ERROR );

        // clean up the mess
        removeShader( m_vertexShader );
        removeShader( m_fragmentShader );
        removeShader( m_geometryShader );
    }

    // everything done now.
    m_reload = false;
}

void WGEShader::updatePrograms()
{
    // is it needed to do something here?
    if( m_deactivated )
    {
        // remove the shaders
        removeShader( m_vertexShader );
        removeShader( m_fragmentShader );
        removeShader( m_geometryShader );
    }
    else if( m_reload )
    {
        reloadShader();
    }
}

WGEShader::SafeUpdaterCallback::SafeUpdaterCallback( WGEShader* shader ):
    m_shader( shader )
{
}

void WGEShader::SafeUpdaterCallback::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    m_shader->updatePrograms();

    // forward the call
    traverse( node, nv );
}

std::string WGEShader::processShaderRecursive( const std::string filename, bool optional, int level )
{
    std::stringstream output;    // processed output

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Before the preprocessors get applied, the following code build the complete shader code from many parts (includes) and handles the version
    // statement automatically. This is important since the GLSL compiler (especially ATI's) relies on it. After completely loading the whole
    // code, the preprocessors get applied.
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // we encountered an endless loop
    if( level > 32 )
    {
        // reached a certain level. This normally denotes a inclusion cycle.
        // We do not throw an exception here to avoid OSG to crash.
        WLogger::getLogger()->addLogMessage( "Inclusion depth is too large. Maybe there is a inclusion cycle in the shader code.",
                "WGEShader (" + filename + ")", LL_ERROR
        );

        // just return unprocessed source
        return "";
    }

    // this is the proper regular expression for includes. This also excludes commented includes
    static const boost::regex includeRegexp( "^[ ]*#[ ]*include[ ]+[\"<](.*)[\">].*" );

    // the input stream, first check existence of shader
    // search these places in this order:
    // 1. m_shaderPath
    // 2. m_shaderPath / shaders
    // 3. WPathHelper::getShaderPath()

    // use one of the following paths
    std::string fn = filename;
    std::string fnLocal = ( m_shaderPath / filename ).file_string();
    std::string fnLocalShaders = ( m_shaderPath / "shaders" / filename ).file_string();
    std::string fnGlobal = ( WPathHelper::getShaderPath() / filename ).file_string();

    if( boost::filesystem::exists( m_shaderPath / filename ) )
    {
        fn = fnLocal;
    }
    else if( boost::filesystem::exists( m_shaderPath / "shaders" / filename ) )
    {
        fn = fnLocalShaders;
    }
    else if( boost::filesystem::exists( WPathHelper::getShaderPath() / filename ) )
    {
        fn = fnGlobal;
    }
    else if( !optional )
    {
        WLogger::getLogger()->addLogMessage( "The requested shader \"" + filename + "\" does not exist in \"" +
                                             m_shaderPath.file_string() + "\", \"" + ( m_shaderPath / "shaders" ).file_string() + "\" or \"" +
                                             WPathHelper::getShaderPath().file_string() + "\".", "WGEShader (" + filename + ")", LL_ERROR
        );

        return "";
    }
    else
    {
        return "";
    }

    std::ifstream input( fn.c_str() );
    if( !input.is_open() )
    {
        if( optional )
        {
            return "";
        }

        // file does not exist. Do not throw an exception to avoid OSG crash
        if( level == 0 )
        {
            WLogger::getLogger()->addLogMessage( "Can't open shader file \"" + filename + "\".",
                    "WGEShader (" + filename + ")", LL_ERROR
            );
        }
        else
        {
            WLogger::getLogger()->addLogMessage( "Can't open shader file for inclusion \"" + filename + "\".",
                    "WGEShader (" + filename + ")", LL_ERROR
            );
        }

        return "";
    }

    // go through each line and process includes
    std::string line;               // the current line
    boost::smatch matches;          // the list of matches

    while( std::getline( input, line ) )
    {
        if( boost::regex_search( line, matches, includeRegexp ) )
        {
            output << processShaderRecursive( matches[1], false, level + 1 );
        }
        else
        {
            output << line;
        }

        // NOTE: we do not apply the m_processors here since the recursive processShaders may have produced many lines. We would need to loop
        // through each one of them. This is done later on for the whole code.

        output << std::endl;
    }

    input.close();

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Done. Return code.
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // this string contains the processed shader code
    return output.str();
}

std::string WGEShader::processShader( const std::string filename, bool optional )
{
    // load all the code
    std::string code = processShaderRecursive( filename, optional );
    if( code.empty() )
    {
        return "";
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // The whole code is loaded now. Apply preprocessors.
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // apply all preprocessors
    PreprocessorsList::ReadTicket r = m_preprocessors.getReadTicket();
    for( PreprocessorsList::ConstIterator pp = r->get().begin(); pp != r->get().end(); ++pp )
    {
        code = ( *pp ).first->process( filename, code );
    }
    r.reset();

    // finally ensure ONE #version at the beginning.
    return m_versionPreprocessor->process( filename, code );
}

void WGEShader::addPreprocessor( WGEShaderPreprocessor::SPtr preproc )
{
    PreprocessorsList::WriteTicket w = m_preprocessors.getWriteTicket();
    if( !w->get().count( preproc ) )   // if already exists, no connection needed
    {
        // subscribe the preprocessors update condition
        boost::signals2::connection con = preproc->getChangeCondition()->subscribeSignal( boost::bind( &WGEShader::reload, this ) );
        w->get().insert( std::make_pair( preproc, con ) );
    }
    w.reset();
    reload();
}

void WGEShader::removePreprocessor( WGEShaderPreprocessor::SPtr preproc )
{
    PreprocessorsList::WriteTicket w = m_preprocessors.getWriteTicket();
    if( w->get().count( preproc ) )   // is it in our list?
    {
        w->get().operator[]( preproc ).disconnect();
        w->get().erase( preproc );
    }
    w.reset();
    reload();
}

void WGEShader::clearPreprocessors()
{
    PreprocessorsList::WriteTicket w = m_preprocessors.getWriteTicket();

    // we need to disconnect each signal subscription
    for( PreprocessorsList::Iterator pp = w->get().begin(); pp != w->get().end(); ++pp )
    {
        ( *pp ).second.disconnect();
    }
    w->get().clear();
    w.reset();
    reload();
}

WGEShaderDefineSwitch::SPtr WGEShader::setDefine( std::string key )
{
    return this->setDefine< bool >( key, true );
}

