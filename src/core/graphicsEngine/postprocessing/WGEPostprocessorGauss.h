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

#ifndef WGEPOSTPROCESSORGAUSS_H
#define WGEPOSTPROCESSORGAUSS_H

#include <boost/shared_ptr.hpp>

#include <osg/Texture2D>

#include "WGEPostprocessor.h"

/**
 * Gauss filtering of the input. It does filter all the textures you bind on it and returns a gauss filtered version. If you want to define the
 * exact list of textures to filter in one pass, use the alternative \ref create call.
 */
class WGEPostprocessorGauss: public WGEPostprocessor
{
public:
    /**
     * Convenience typedef for a boost::shared_ptr< WGEPostprocessorGauss >.
     */
    typedef boost::shared_ptr< WGEPostprocessorGauss > SPtr;

    /**
     * Convenience typedef for a boost::shared_ptr< const WGEPostprocessorGauss >.
     */
    typedef boost::shared_ptr< const WGEPostprocessorGauss > ConstSPtr;

    /**
     * Default constructor.
     */
    WGEPostprocessorGauss();

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
    WGEPostprocessorGauss( osg::ref_ptr< WGEOffscreenRenderNode > offscreen,
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
    virtual ~WGEPostprocessorGauss();

    /**
     * Create instance. Uses the protected constructor. Implement it if you derive from this class!
     *
     * \param offscreen use this offscreen node to add your texture pass'
     * \param gbuffer the input textures you should use
     * \returns shared pointer to the created instance
     */
    virtual WGEPostprocessor::SPtr create( osg::ref_ptr< WGEOffscreenRenderNode > offscreen,
                                           const PostprocessorInput& gbuffer ) const;
protected:
private:
};

#endif  // WGEPOSTPROCESSORGAUSS_H

