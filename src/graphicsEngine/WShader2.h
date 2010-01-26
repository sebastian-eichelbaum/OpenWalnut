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

#ifndef WSHADER2_H
#define WSHADER2_H

#include <map>
#include <string>

#include <osg/Shader>
#include <osg/Program>

/**
 * Class encapsulating the OSG Program class for a more convenient way of adding and modifying shader.
 */
class WShader2: public osg::Program
{
public:

    /**
     * Default constructor. Loads the specified shader programs.
     *
     * \param name the name of the shader. It gets searched in the shader path.
     */
    explicit WShader2( std::string name );

    /**
     * Destructor.
     */
    virtual ~WShader2();

    /**
     * Apply this shader to the specified node. Use this method to ensure, that reload events can be handled properly during the
     * update cycle.
     *
     * \param node the node where the program should be registered to.
     */
    virtual void apply( osg::ref_ptr< osg::Node > node );

    /**
     * Initiate a reload of the shader during the next update cycle.
     */
    virtual void reload();

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

protected:

    /**
     * Helper routine to load a text file into a string
     *
     * \return String
     *
     * \param fileName The file name. This is used to build the final filename using the shader path defined in WGraphicsEngine.
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
     * The name of the shader. It is used to construct the actual filename to load.
     */
    std::string m_name;

    /**
     * Flag denoting whether a shader should be reloaded.
     */
    bool m_reload;

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
        explicit SafeUpdaterCallback( WShader2* shader );

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
        WShader2* m_shader;
    };

private:
};

#endif  // WSHADER2_H

