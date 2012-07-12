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

#ifndef WSPSLICEBUILDERVECTORS_H
#define WSPSLICEBUILDERVECTORS_H

// #include <utility>
// #include <vector>
//
// #include <boost/filesystem/path.hpp>
// #include <boost/shared_ptr.hpp>
//
// #include <osg/Geode>
//
// #include "core/common/WProperties.h"
// #include "core/common/WPropertyTypes.h"
// #include "core/graphicsEngine/shaders/WGEShader.h"
// #include "WSPSliceBuilder.h"
//
// // forward declarations
// class WDataSetVector;
//
// /**
//  * This Builder is used to generate views of probabilistic tractograms ala Schmahmann and Pandya.
//  */
// class WSPSliceBuilderVectors : public WSPSliceBuilder
// {
// public:
//     /**
//      * Creates a geode builder to build axial, coronal and sagittal slices ala Schahmann y Pandya.
//      *
//      * \param probTracts The list of probabilistic tractograms which should be taken into account.
//      * \param sliceGroup Slice positions
//      * \param colorMap For each connected probabilistic trac its color
//      * \param vector Vector dataset with the largest eigen vectors.
//      * \param vectorGroup Properties for some parameters of this drawing method
//      * \param shaderPath The filesystem path where the shader file is located.
//      */
//     WSPSliceBuilderVectors( ProbTractList probTracts, WPropGroup sliceGroup, std::vector< WPropGroup > colorMap,
//             boost::shared_ptr< const WDataSetVector > vector, WPropGroup vectorGroup, boost::filesystem::path shaderPath  );
//
//     /**
//      * Implements the preprocessing interface \ref WSPSliceBuilder.
//      */
//     void preprocess();
//
//     /**
//      * Implements the generateSlice interface \ref WSPSliceBuilder.
//      * This generates for each axis a group node with some geodes.
//      *
//      * \param sliceNum 0 == xSlice, 1 ySlice, 2 zSlice.
//      *
//      * \return The group of nodes for the given slice.
//      */
//     osg::ref_ptr< WGEGroupNode > generateSlice( const unsigned char sliceNum ) const;
//
// private:
// //    osg::ref_ptr< osg::Vec3Array > generateQuadStubs( const WPosition& pos ) const;
//
//     /**
//      * Computes four vertices so each describe the translation of the middle point to one of the four quad corners.
//      *
//      * \param activeDims This are the opposite indices of the current slice.
//      *
//      * \return All four coordinate transformations, for each vertex in a slice.
//      */
//     osg::ref_ptr< osg::Vec3Array > generateQuadSpanning( std::pair< unsigned char, unsigned char > activeDims ) const;
//
//     /**
//      * Generates directions clockwise around a center where the other quads will be placed later on.
//      *
//      * \param activeDims This are the opposite indices of the current slice.
//      * \param distance How far the other quads are placed around the center point
//      *
//      * \return A array of direction to add to the center point to get the new centerpoints around in clockwise manner.
//      */
// boost::shared_ptr< std::vector< WVector3d > > generateClockwiseDir( std::pair< unsigned char, unsigned char > activeDims, double distance ) const;
//
//     /**
//      * Compute the origin and the base vectors of each slice, and returns the other opposite indices.
//      *
//      * \param sliceNum The current slice index: 0 sagittal (xSlice), 1 coronal (ySlice), 2 axial (zSlice)
//      * \param origin There will be put the origin of the given slice
//      * \param a The first base vector of the given slice
//      * \param b The second base vector of the given slice
//      *
//      * \return If \c sliceNum=0 the oppsite indices are \c 1 and \c 2. For \c sliceNum=1, they are \c 0,2, and for \c sliceNum=2 they are \c 1,2.
//      */
//     std::pair< unsigned char, unsigned char > computeSliceBase( const unsigned char sliceNum, boost::shared_ptr< WVector3d > origin,
//             boost::shared_ptr< WVector3d > a, boost::shared_ptr< WVector3d > b ) const;
//
//     /**
//      * Compute the focal points for ellipsoid generation within the fragment shader. Basically this are just the start and end points of
//      * the principal diffusion direction, e.g. the interpolated vector at that given point projected onto the current slice.
//      *
//      * \param pos Where the interpolation should start
//      * \param sliceNum Which slice to project on
//      *
//      * \return The foci describing the ellipsoid.
//      */
//     std::pair< WPosition, WPosition > computeFocalPoints( const WPosition& pos, size_t sliceNum ) const;
//
//     /**
//      * The eigenvectors.
//      */
//     boost::shared_ptr< const WDataSetVector > m_vectors;
//
//     /**
//      * A reference to the property which denotes the space between the primary quads (aka subdivision) as well as the size of the cell borders.
//      */
//     WPropDouble m_spacing; // we cannot make const here since we need the WPropType for the WGEPropertyUniform creation
//
//     /**
//      * Upto which threshold the quads should be discarded.
//      */
//     boost::shared_ptr< const WPVDouble > m_probThreshold;
//
//     /**
//      * Spacing between the glyphs around the grid points.
//      */
//     boost::shared_ptr< const WPVDouble > m_glyphSpacing;
//
//     /**
//      * Thickness of the line used to draw the glyph.
//      */
//     WPropDouble m_glyphThickness;
//
//     /**
//      * Flag to show or hide the slice grid of current resolution.
//      */
//     WPropBool m_showGrid;
//
//     /**
//      * Shading the quads and transform them to the glyphs (line stipples).
//      */
//     osg::ref_ptr< WGEShader > m_shader;
// };

#endif  // WSPSLICEBUILDERVECTORS_H
