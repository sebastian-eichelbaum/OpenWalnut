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

#include "../WExportWGE.h"

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
     * Create instance. Uses the protected constructor. Implement it if you derive from this class!
     *
     * \param offscreen use this offscreen node to add your texture pass'
     * \param gbuffer the input textures you should use
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
     * \return the result texture
     */
    virtual osg::ref_ptr< osg::Texture2D > getOutput() const;

    /**
     * Returns the new depth texture. Allows you to modify the depth values. By default, this is the input depth texture.
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
     * Constructor. Call this constructor from your derived class.
     *
     * \param offscreen use this offscreen node to add your texture pass'
     * \param gbuffer the input textures you should use
     * \param name a name for this postprocessor
     * \param description a useful description
     */
    WGEPostprocessor( osg::ref_ptr< WGEOffscreenRenderNode > offscreen, const PostprocessorInput& gbuffer, std::string name, std::string description );

    /**
     * The texture contains the result
     */
    osg::ref_ptr< osg::Texture2D > m_resultTexture;

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
};

#endif  // WGEPOSTPROCESSOR_H

