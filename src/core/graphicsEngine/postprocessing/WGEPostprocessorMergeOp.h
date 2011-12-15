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

#ifndef WGEPOSTPROCESSORMERGEOP_H
#define WGEPOSTPROCESSORMERGEOP_H

#include <string>

#include <boost/shared_ptr.hpp>

#include <osg/Texture2D>

#include "../shaders/WGEShader.h"
#include "../shaders/WGEShaderCodeInjector.h"

#include "WGEPostprocessor.h"

/**
 * MergeOp is a operator to merge multiple input texture. The merge operation can be defined by the user.
 */
class WGEPostprocessorMergeOp: public WGEPostprocessor
{
public:
    /**
     * Convenience typedef for a boost::shared_ptr< WGEPostprocessorMergeOp >.
     */
    typedef boost::shared_ptr< WGEPostprocessorMergeOp > SPtr;

    /**
     * Convenience typedef for a boost::shared_ptr< const WGEPostprocessorMergeOp >.
     */
    typedef boost::shared_ptr< const WGEPostprocessorMergeOp > ConstSPtr;

    /**
     * Default constructor.
     */
    WGEPostprocessorMergeOp();

    /**
     * Constructor. We implement a public constructor which can take more textures as input
     *
     * \param offscreen use this offscreen node to add your texture pass'
     * \param tex0 texture to filter
     * \param tex1 texture to filter
     * \param tex2 texture to filter
     * \param tex3 texture to filter
     * \param tex4 texture to filter
     * \param tex5 texture to filter
     * \param tex6 texture to filter
     * \param tex7 texture to filter
     */
    WGEPostprocessorMergeOp( osg::ref_ptr< WGEOffscreenRenderNode > offscreen,
                           osg::ref_ptr< osg::Texture2D > tex0,
                           osg::ref_ptr< osg::Texture2D > tex1 = osg::ref_ptr< osg::Texture2D >(),
                           osg::ref_ptr< osg::Texture2D > tex2 = osg::ref_ptr< osg::Texture2D >(),
                           osg::ref_ptr< osg::Texture2D > tex3 = osg::ref_ptr< osg::Texture2D >(),
                           osg::ref_ptr< osg::Texture2D > tex4 = osg::ref_ptr< osg::Texture2D >(),
                           osg::ref_ptr< osg::Texture2D > tex5 = osg::ref_ptr< osg::Texture2D >(),
                           osg::ref_ptr< osg::Texture2D > tex6 = osg::ref_ptr< osg::Texture2D >(),
                           osg::ref_ptr< osg::Texture2D > tex7 = osg::ref_ptr< osg::Texture2D >() );

    /**
     * Destructor.
     */
    virtual ~WGEPostprocessorMergeOp();

    /**
     * Create instance. Uses the protected constructor. Implement it if you derive from this class!
     *
     * \param offscreen use this offscreen node to add your texture pass'
     * \param gbuffer the input textures you should use
     */
    virtual WGEPostprocessor::SPtr create( osg::ref_ptr< WGEOffscreenRenderNode > offscreen,
                                           const PostprocessorInput& gbuffer ) const;

    /**
     * Set the GLSL code inserted into the shader as merging operation. If none was set, the mix command is used for each incoming texture. The
     * code you write here should not make any assumptions to the environment it is inserted. What you have is a vec4 named color, initialized
     * with vec4( 1.0 ). Set your final color to this vec4. It will be the result.
     *
     * \param code the code as string
     */
    void setGLSLMergeCode( std::string code );
protected:
private:
    /**
     * The shader used for merging
     */
    WGEShader::RefPtr m_mergeOpShader;

    /**
     * This preprocessor handles insertion of the custom merge code.
     */
    WGEShaderCodeInjector::SPtr m_codeInjector;
};

#endif  // WGEPOSTPROCESSORMERGEOP_H

