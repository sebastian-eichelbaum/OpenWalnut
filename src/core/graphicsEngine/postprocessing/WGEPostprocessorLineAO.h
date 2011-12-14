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

#ifndef WGEPOSTPROCESSORLINEAO_H
#define WGEPOSTPROCESSORLINEAO_H

#include <boost/shared_ptr.hpp>

#include "WGEPostprocessor.h"

/**
 * LineAO implementation.
 */
class WGEPostprocessorLineAO: public WGEPostprocessor
{
public:

    /**
     * Convenience typedef for a boost::shared_ptr< WGEPostprocessorLineAO >.
     */
    typedef boost::shared_ptr< WGEPostprocessorLineAO > SPtr;

    /**
     * Convenience typedef for a boost::shared_ptr< const WGEPostprocessorLineAO >.
     */
    typedef boost::shared_ptr< const WGEPostprocessorLineAO > ConstSPtr;

    /**
     * Default constructor.
     */
    WGEPostprocessorLineAO();

    /**
     * Destructor.
     */
    virtual ~WGEPostprocessorLineAO();

    /**
     * Create instance. Uses the protected constructor. Implement it if you derive from this class!
     *
     * \param offscreen use this offscreen node to add your texture pass'
     * \param gbuffer the input textures you should use
     */
    virtual WGEPostprocessor::SPtr create( osg::ref_ptr< WGEOffscreenRenderNode > offscreen, const PostprocessorInput& gbuffer ) const;
protected:
    /**
     * Constructor. Implement this constructor and build your processing pipeline in here
     *
     * \param offscreen use this offscreen node to add your texture pass'
     * \param gbuffer the input textures you should use
     */
    WGEPostprocessorLineAO( osg::ref_ptr< WGEOffscreenRenderNode > offscreen, const PostprocessorInput& gbuffer );
private:
};

#endif  // WGEPOSTPROCESSORLINEAO_H

