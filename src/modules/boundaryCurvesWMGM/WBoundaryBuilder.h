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

#ifndef WBOUNDARYBUILDER_H
#define WBOUNDARYBUILDER_H

#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>

#include <osg/ref_ptr>

#include "core/common/WProperties.h"
#include "core/graphicsEngine/shaders/WGEShader.h"
#include "core/graphicsEngine/WGEManagedGroupNode.h"

// forward declarations
class WGEManagedGroupNode;
class WDataSetScalar;

/**
 * Abstract base class for a boundary builder which needs serveral input paramertes \ref WBoundaryBuilder constructor
 * and provides an interface: \ref run for generating the graphics.
 */
class WBoundaryBuilder
{
public:
    /**
     * Constructs an builder instance.
     *
     * \param texture The underlying scalar dataset to compute those boundaries for
     * \param properties Properties, like slice positions, thresholds etc.
     * \param slices Slice geodes which are controlled (hide/unhide) by the module.
     */
    WBoundaryBuilder( boost::shared_ptr< const WDataSetScalar > texture,
                      boost::shared_ptr< const WProperties > properties,
                      boost::array< osg::ref_ptr< WGEManagedGroupNode >, 3 > *slices );

    /**
     * Destructs this.
     */
    virtual ~WBoundaryBuilder();

    /**
     * Starts rendering and finally insert result in output.
     *
     * \param output Where to put/insert the results.
     * \param sliceNum If given -1 all slice will perform an update, otherwise only the slice with the given slice number.
     */
    virtual void run( osg::ref_ptr< WGEManagedGroupNode > output, const char sliceNum = -1 ) = 0;

protected:
    /**
     * The underlying scalar dataset to compute those boundaries for.
     */
    boost::shared_ptr< const WDataSetScalar > m_texture;

    /**
     * The three slice positions.
     * 0 : xSlice, 1 : ySlice, 2 : zSlice
     */
    boost::array< WPropDouble, 3 > m_slicePos;

    /**
     * Pointer to the three slices.
     * 0 : xSlice, 1 : ySlice, 2 : zSlice
     */
    boost::array< osg::ref_ptr< WGEManagedGroupNode >, 3 > m_slices;

    /**
     * Gray matter threshold.
     */
    WPropDouble m_grayMatter;

    /**
     * White matter threshold.
     */
    WPropDouble m_whiteMatter;

    /**
     * Shader generating the curves of gray and white matter.
     */
    osg::ref_ptr< WGEShader > m_shader;

    /**
     * Gray matter color.
     */
    WPropColor m_gmColor;

    /**
     * White matter color.
     */
    WPropColor m_wmColor;

private:
};

#endif  // WBOUNDARYBUILDER_H
