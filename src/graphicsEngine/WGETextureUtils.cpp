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

#include <GL/gl.h>

#include "WGETexture.h"

#include "WGETextureUtils.h"

void wge::unbindTexture( osg::ref_ptr< osg::Node > node, size_t unit, size_t count )
{
    for ( size_t i = unit; i < unit + count; ++i )
    {
        node->getOrCreateStateSet()->removeTextureAttribute( i, osg::StateAttribute::TEXTURE );
        node->getOrCreateStateSet()->removeTextureAttribute( i, osg::StateAttribute::TEXMAT );
    }
}

size_t wge::getMaxTexUnits()
{
    // GLint ret;
    // glGetIntegerv( GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &ret );
    // Why do we not use these glGet things here? The answer is simple: The GLSL 1.20 Standard does not define a way to access more than 8
    // texture coordinate attributes.
    return 8;
}

