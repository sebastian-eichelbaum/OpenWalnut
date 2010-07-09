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

#include <map>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/signals2/signal.hpp>

#include <osg/Shader>
#include <osg/Program>

/**
 * Class encapsulating the OSG Program class for a more convenient way of adding and modifying shader.
 */
class WShader: public osg::Program
{
public:

    /**
     * Default constructor. Loads the specified shader programs.
     *
     * \param name the name of the shader. It gets searched in the shader path.
     */
    explicit WShader( std::string name );

    /**
     * Destructor.
     */
    virtual ~WShader();

    /**
     * Apply this shader to the specified node. Use this method to ensure, that reload events can be handled properly during the
     * update cycle.
     *
     * \param node the node where the program should be registered to.
     */
    virtual void apply( osg::ref_ptr< osg::Node > node );

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
     */
    void setDefine( std::string key, float value = 1.0 );

    /**
     * Deletes a define from the internal list
     *
     * \param key The name of the define
     */
    void eraseDefine( std::string key );

    /**
     * Removes all existing defines.
     */
    void eraseAllDefines();

protected:

    /**
     * This method searches and processes all includes in the shader source. The filenames in the include statement are assumed to
     * be relative to this shader's path.
     *
     * \param filename the filename of the shader to process.
     * \param optional denotes whether a "file not found" is critical or not
     * \param level the inclusion level. This is used to avoid cycles.
     *
     * \return the processed source.
     */
    std::string processShader( const std::string filename, bool optional = false, int level = 0 );

    /**
     * String that stores the location of all shader files
     */
    std::string m_shaderPath;

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
     * a map of all set defines
     */
    std::map< std::string, float > m_defines;

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
        explicit SafeUpdaterCallback( WShader* shader );

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
        WShader* m_shader;
    };

private:
};

#endif  // WSHADER_H

