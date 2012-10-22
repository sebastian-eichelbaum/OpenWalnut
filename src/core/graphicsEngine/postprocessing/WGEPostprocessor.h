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

#ifndef WGEPOSTPROCESSOR_H
#define WGEPOSTPROCESSOR_H

#include <vector>
#include <string>

#include <boost/shared_ptr.hpp>

#include <osg/ref_ptr>
#include <osg/Node>
#include <osg/Camera>
#include <osg/Texture>

#include "../offscreen/WGEOffscreenRenderNode.h"
#include "../offscreen/WGEOffscreenRenderPass.h"
#include "../offscreen/WGEOffscreenFinalPass.h"

#include "../shaders/WGEShaderPropertyDefineOptions.h"

#include "../../common/WProperties.h"
#include "../../common/WPrototyped.h"



/**
 * The base class for all custom post-processors. It allows building an own texture processing pipeline for special processings.
 */
class WGEPostprocessor: public WPrototyped
{
public:
    /**
     * This class encapsulates a G-Buffer. Basically, this is a collection of per-pixel geometry information.
     */
    class PostprocessorInput
    {
    public:
        /**
         * Constructs an instance from a given list of textures. The order in the list define color, normal, parameter, tangent, depth. There are
         * no restrictions to the input list. If textures are missing, the corresponding textures in the GBuffer are missing.
         *
         * \param from source list
         */
        explicit PostprocessorInput( std::vector< osg::ref_ptr< osg::Texture2D > > from );

        /**
         * Construct GBuffer with an explicit list of textures.
         *
         * \param color color texture
         * \param normal normal texture
         * \param parameter parameter texture
         * \param tangent tangent texture
         * \param depth depth texture
         */
        PostprocessorInput( osg::ref_ptr< osg::Texture2D > color,
                            osg::ref_ptr< osg::Texture2D > normal,
                            osg::ref_ptr< osg::Texture2D > parameter,
                            osg::ref_ptr< osg::Texture2D > tangent,
                            osg::ref_ptr< osg::Texture2D > depth );

        /**
         * Constructor creates empty GBuffer. All textures are un-initialized.
         */
        PostprocessorInput();

        /**
         * Attaches the needed textures to the specified render pass and returns the G-Buffer
         *
         * \param from the renderpass to attach this to
         *
         * \return the buffer.
         */
        static PostprocessorInput attach( osg::ref_ptr< WGEOffscreenRenderPass > from );

        /**
         * Attaches these textures to the specified renderpass
         *
         * \param to attach to this
         *
         * \return the ID of the NEXT free texture unit you can use
         */
        size_t bind( osg::ref_ptr< WGEOffscreenRenderPass > to ) const;

        /**
         * Color in RGBA
         */
        osg::ref_ptr< osg::Texture2D > m_colorTexture;

        /**
         * Normal in RGB
         */
        osg::ref_ptr< osg::Texture2D > m_normalTexture;

        /**
         * Some not yet defined parameter texture, LUMINANCE only
         */
        osg::ref_ptr< osg::Texture2D > m_parameterTexture;

        /**
         * Tangent in RGB
         */
        osg::ref_ptr< osg::Texture2D > m_tangentTexture;

        /**
         * Depth
         */
        osg::ref_ptr< osg::Texture2D > m_depthTexture;
    };

    /**
     * Convenience typedef for an osg::ref_ptr< WGEPostprocessor >.
     */
    typedef boost::shared_ptr< WGEPostprocessor > SPtr;

    /**
     * Convenience typedef for an osg::ref_ptr< const WGEPostprocessor >.
     */
    typedef boost::shared_ptr< const WGEPostprocessor > ConstSPtr;

    /**
     * Type used for returning lists of postprocessor prototypes.
     */
    typedef std::vector< WGEPostprocessor::SPtr > ProcessorList;

    /**
     * Returns a list of all known postprocessor prototypes
     *
     * \return the list
     */
    static ProcessorList getPostprocessors();

    /**
     * Needs to be called prior to any "getPostprocessors" call. Needed for initialization. This is done by WGraphicsEngine.
     */
    static void initPostprocessors();

    /**
     * Allows adding a postprocessor. After this call, everyone using the WGEPostprocessor can utilize your addded postproc.
     *
     * \param processor the postprocessor to add
     *
     * \return the index of the newly added postprocessor.
     */
    static size_t addPostprocessor( SPtr processor );

    /**
     * Create named prototype. You should call this in your prototype constructor.
     *
     * \param name name of processor
     * \param description description.
     */
    WGEPostprocessor( std::string name, std::string description );

    /**
     * Destructor.
     */
    virtual ~WGEPostprocessor();

    /**
     * Create instance. Uses the protected constructor. Implement it if you derive from this class! This is called whenever your postprocessor is
     * applied to the standard render-output. You can add your own constructors and creators for other cases.
     *
     * \param offscreen use this offscreen node to add your texture pass'
     * \param gbuffer the input textures you should use
     * \returns shared pointer to the created instance
     */
    virtual SPtr create( osg::ref_ptr< WGEOffscreenRenderNode > offscreen, const PostprocessorInput& gbuffer ) const = 0;

    /**
     * Returns the set of properties controlling the post-processing node. You can use them to provide them to the user for example.
     *
     * \return the properties as a group.
     */
    virtual WPropGroup getProperties() const;

    /**
     * Returns the result texture. Use this to continue processing.
     *
     * \param idx which output. Each postprocessor returns at least one texture in index 0, which also is the default value
     *
     * \return the result texture
     */
    virtual osg::ref_ptr< osg::Texture2D > getOutput( size_t idx = 0 ) const;

    /**
     * This processor can produce multiple outputs. Grab them here. This vector always contains at least the first filtered texture in unit 0.
     *
     * \return the vector as copy.
     */
    const std::vector< osg::ref_ptr< osg::Texture2D > >& getOutputList() const;

    /**
     * Returns the new depth texture. Allows you to modify the depth values. By default, this is NULL. Check this!
     *
     * \return the depth texture
     */
    virtual osg::ref_ptr< osg::Texture2D > getDepth() const;

    /**
     * Gets the name of this postprocessor.
     *
     * \return the name.
     */
    virtual const std::string getName() const;

    /**
     * Gets the description for this postprocessor
     *
     * \return the description
     */
    virtual const std::string getDescription() const;
protected:
    /**
     * The textures contain the result. Add at least one result texture
     */
    std::vector< osg::ref_ptr< osg::Texture2D > > m_resultTextures;

    /**
     * The texture contains the new depth
     */
    osg::ref_ptr< osg::Texture2D > m_depthTexture;

    /**
     * All the properties of the post-processor.
     */
    WPropGroup m_properties;

    /**
     * A flag denoting whether the effect should be combined with color or not.
     */
    WPropBool m_effectOnly;

    /**
     * For convenience, this is a shader preprocessor controlled by m_effectOnly property.
     */
    WGEShaderPreprocessor::SPtr m_effectOnlyPreprocessor;
private:
    /**
     * Name string. Set by the constructor.
     */
    std::string  m_name;

    /**
     * Description string. Set by the constructor.
     */
    std::string m_description;

    /**
     * List of all postprocessors. Handled as singleton.
     */
    static ProcessorList m_postProcessors;
};

#endif  // WGEPOSTPROCESSOR_H

