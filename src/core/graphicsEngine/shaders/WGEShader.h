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

#ifndef WGESHADER_H
#define WGESHADER_H

#include <map>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/signals2/signal.hpp>

#include <osg/NodeCallback>
#include <osg/Program>
#include <osg/Shader>

#include "../../common/WPathHelper.h"
#include "../../common/WSharedAssociativeContainer.h"

#include "WGEShaderDefine.h"
#include "WGEShaderPreprocessor.h"

/**
 * Class encapsulating the OSG Program class for a more convenient way of adding and modifying shader.
 */
class WGEShader: public osg::Program
{
public:
    /**
     * Convenience typedef for an osg::ref_ptr
     */
    typedef osg::ref_ptr< WGEShader > RefPtr;

    /**
     * Convenience typedef for an osg::ref_ptr; const
     */
    typedef osg::ref_ptr< const WGEShader > ConstRefPtr;

    /**
     * Default constructor. Loads the specified shader programs. The path that can be specified is optional but allows modules to load their own
     * local shaders. The search order for shader files is as follows: 1. search, 2. search/shaders, 3. WPathHelper::getShaderPath()
     *
     * \param name the name of the shader. It gets searched in the shader path.
     * \param search the local search path. If not specified, the global shader path is used.
     */
    WGEShader( std::string name, boost::filesystem::path search = WPathHelper::getShaderPath() );

    /**
     * Destructor.
     */
    virtual ~WGEShader();

    /**
     * Apply this shader to the specified node. Use this method to ensure, that reload events can be handled properly during the
     * update cycle.
     *
     * \param node the node where the program should be registered to.
     */
    virtual void apply( osg::ref_ptr< osg::Node > node );

    /**
     * If enabled, activate our program in the GL pipeline, performing any rebuild operations that might be pending. In addition to the standard
     * OSG functionality, it also loads/reloads the shader source from file.
     *
     * \param state the state to apply the shader program to.
     */
    virtual void applyDirect( osg::State& state );  // NOLINT <- ensure this matches the official OSG API by using a non-const ref

    /**
     * Removes the shader from the specified node.
     *
     * \param node the node where the program is registered to.
     */
    virtual void deactivate( osg::ref_ptr< osg::Node > node );

    /**
     * Initiate a reload of the shader during the next update cycle.
     */
    virtual void reload();

    /**
     * Sets a define which is include into the shader source code. This allows the preprocessor to turn on/off several parts of your code. In GLSL
     * defines are a better choice when compared with a lot of branches (if-statements).
     *
     * \param key The name of the define
     * \param value The value of the define. If this is not specified, the define can be used as simple ifdef switch.
     *
     * \return the define object allowing later control
     */
    template < typename T >
    typename WGEShaderDefine< T >::SPtr setDefine( std::string key, T value );

    /**
     * Sets a define which is include into the shader source code. This allows the preprocessor to turn on/off several parts of your code. In GLSL
     * defines are a better choice when compared with a lot of branches (if-statements).
     *
     * \param key The name of the define
     *
     * \return the switch allowing to control the define
     */
    WGEShaderDefineSwitch::SPtr setDefine( std::string key );

    /**
     * Adds the specified preprocessor to this shader. The preprocessor is able to force shader reloads.
     *
     * \param preproc the preprocessor to add.
     */
    void addPreprocessor( WGEShaderPreprocessor::SPtr preproc );

    /**
     * Removes the specified preprocessor. Changes inside the preprocessor won't cause any updates anymore.
     *
     * \param preproc the preprocessor to remove. If not exists: nothing is done.
     */
    void removePreprocessor( WGEShaderPreprocessor::SPtr preproc );

    /**
     * Removes all preprocessors. Be careful with this one since it removes the WGESHaderVersionPreprocessor too, which is mandatory.
     */
    void clearPreprocessors();

protected:
    /**
     * This method searches and processes all includes in the shader source. The filenames in the include statement are assumed to
     * be relative to this shader's path. It simply unrolls the code.
     *
     * \param filename the filename of the shader to process.
     * \param optional denotes whether a "file not found" is critical or not
     * \param level the inclusion level. This is used to avoid cycles.
     *
     * \return the processed source.
     */
    std::string processShaderRecursive( const std::string filename, bool optional = false, int level = 0 );

    /**
     * This method searches and processes all includes in the shader source. The filenames in the include statement are assumed to
     * be relative to this shader's path. It additionally applies preprocessors.
     *
     * \see processShaderRecursive
     *
     * \param filename the filename of the shader to process.
     * \param optional denotes whether a "file not found" is critical or not
     *
     * \return the processed source.
     */
    std::string processShader( const std::string filename, bool optional = false );

    /**
     * This completely reloads the shader file and processes it. It also resets m_reload to false.
     */
    void reloadShader();

    /**
     * Handles all state changes in m_reload and m_deactivated. It ensure that the shader programs are bound properly or deactivated.
     */
    void updatePrograms();

    /**
     * String that stores the location of all shader files
     */
    boost::filesystem::path m_shaderPath;

    /**
     * The name of the shader. It is used to construct the actual filename to load.
     */
    std::string m_name;

    /**
     * Flag denoting whether a shader should be reloaded.
     */
    bool m_reload;

    /**
     * True if the shaders have been loaded successfully previously.
     */
    bool m_shaderLoaded;

    /**
     * Flag denoting whether a shader should be deactivated.
     */
    bool m_deactivated;

    /**
     * Connection object to the reload signal from WGraphbicsEngine.
     */
    boost::signals2::connection m_reloadSignalConnection;

    /**
     * The list of preprocessors - Type
     */
    typedef WSharedAssociativeContainer< std::map< WGEShaderPreprocessor::SPtr, boost::signals2::connection > > PreprocessorsList;

    /**
     * List of all pre-processing that need to be applied to this shader instance
     */
    PreprocessorsList m_preprocessors;

    /**
     * This preprocessor needs to be run LAST. It handles version-statements in GLSL.
     */
    WGEShaderPreprocessor::SPtr m_versionPreprocessor;

    /**
     * the vertex shader object
     */
    osg::ref_ptr< osg::Shader > m_vertexShader;

    /**
     * the fragment shader object
     */
    osg::ref_ptr< osg::Shader > m_fragmentShader;

    /**
     * the geometry shader object
     */
    osg::ref_ptr< osg::Shader > m_geometryShader;

    /**
     * Update callback which handles the shader reloading.
     * This ensures thread safe modification of the osg node.
     */
    class SafeUpdaterCallback : public osg::NodeCallback
    {
    public:
        /**
         * Constructor. Creates a new callback.
         *
         * \param shader the shader which needs to be updated.
         */
        explicit SafeUpdaterCallback( WGEShader* shader );

        /**
         * Callback method called by the NodeVisitor when visiting a node.
         * This inserts and removes enqueued nodes from this group node instance.
         *
         * \param node the node calling this update
         * \param nv The node visitor which performs the traversal. Should be an
         * update visitor.
         */
        virtual void operator()( osg::Node* node, osg::NodeVisitor* nv );

    protected:
        /**
         * The shader belonging to the node currently getting updated.
         */
        WGEShader* m_shader;
    };

private:
};

template < typename T >
typename WGEShaderDefine< T >::SPtr WGEShader::setDefine( std::string key, T value )
{
    typename WGEShaderDefine< T >::SPtr def;

    // try to find the define. If it exists, set it. If not, add it.
    PreprocessorsList::ReadTicket r = m_preprocessors.getReadTicket();
    for( PreprocessorsList::ConstIterator pp = r->get().begin(); pp != r->get().end(); ++pp )
    {
        typename WGEShaderDefine< T >::SPtr define = boost::dynamic_pointer_cast< WGEShaderDefine< T > >( ( *pp ).first );
        if( define && ( define->getName() == key ) )
        {
            define->setValue( value );
            def = define;
            break;
        }
    }
    r.reset();

    // did not find it. Add.
    if( !def )
    {
        def = typename WGEShaderDefine< T >::SPtr( new WGEShaderDefine< T >( key, value ) );
        addPreprocessor( def );
    }
    return def;
}

#endif  // WGESHADER_H

