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

#ifndef WSPSLICEBUILDER_H
#define WSPSLICEBUILDER_H

// #include <vector>
//
// #include <boost/shared_ptr.hpp>
//
// #include <osg/ref_ptr>
//
// #include "core/common/WBoundingBox.h"
// #include "core/common/WProperties.h"
// #include "core/common/WPropertyTypes.h"
//
// // forward declarations
// class WDataSetScalar;
// class WGEGroupNode;
// template< typename T >
// class WGridRegular3DTemplate;
//
// /**
//  * This Builder is used to generate views of probabilistic tractograms ala Schmahmann and Pandya.
//  */
// class WSPSliceBuilder
// {
// public:
//     /**
//      * Vector of scalar datasets e.g. probabilistic tractograms.
//      */
//     typedef std::vector< boost::shared_ptr< const WDataSetScalar > > ProbTractList;
//
//     /**
//      * Creates a geode builder to build axial, coronal and sagittal slices ala Schahmann y Pandya.
//      *
//      * \param probTracts The vector of probabilistic tractograms which should be taken into account.
//      * \param sliceGroup Slice positions
//      * \param colorMap For each connected probabilistic trac its color
//      */
//     WSPSliceBuilder( ProbTractList probTracts, WPropGroup sliceGroup, std::vector< WPropGroup > colorMap );
//
//     /**
//      * Destructs this. We need a virtual desturctor as long as we have virtual member functions...
//      */
//     virtual ~WSPSliceBuilder();
//
//     /**
//      * Do some preprocessing which is needed for every slice redraw, e.g. when the slice has moved.
//      */
//     virtual void preprocess() = 0;
//
//     /**
//      * This generates for each axis a group node with some geodes.
//      *
//      * \param sliceNum 0 == xSlice, 1 ySlice, 2 zSlice.
//      *
//      * \return The group of nodes for the given slice.
//      */
//     virtual osg::ref_ptr< WGEGroupNode > generateSlice( const unsigned char sliceNum ) const = 0;
//
// protected:
//     /**
//      * Very simple color mapping which is used temporarily, later we want to substitute this with a color chooser as properties.
//      *
//      * \param probTractNum Index of the probabilisitc tractrogram to get the color for.
//      *
//      * \return The color for the given prob tract index.
//      */
//     WColor colorMap( size_t probTractNum ) const;
//
//     /**
//      * Compares the color's alpha value and the threshold.
//      *
//      * \param c The given color
//      * \param threshold The given Threshold
//      *
//      * \return ture if the color has an alpha value below the given threshold.
//      */
//     bool alphaBelowThreshold( const WColor& c, const double threshold ) const;
//
//     /**
//      * For a certain tractogram the color is looked up in the color map and the alpha value is set to the interpolated
//      * probability value.
//      *
//      * \param pos The position to look up the color and probability.
//      * \param tractID For which tract the color should be looked up
//      *
//      * \return The color of the tract, where the alpha value is either the interpolated tract probability or -1.0 if interpolation has failed.
//      */
//     WColor lookUpColor( const WPosition& pos, size_t tractID ) const;
//
//     /**
//      * Compute for each probabilistic tractogram the color at the given pos and sets the alpha value to the interpolated probability.
//      *
//      * \param pos The position to look up colors and probabilities for.
//      *
//      * \return The vector of colors.
//      */
//     osg::ref_ptr< osg::Vec4Array > computeColorsFor( const osg::Vec3& pos ) const;
//
//     /**
//      * Computes the bouding boxes for the slices.
//      *
//      * \note Whenever the xPos, yPos or zPos of the slice change those have to be recomputed!
//      */
//     void computeSliceBB();
//
//     /**
//      * The grid of the first tractogram. It is assumed that all given probablilisitc tractograms operate on the same grid.
//      */
//     boost::shared_ptr< const WGridRegular3DTemplate< double > > m_grid;
//
//     /**
//      * Hold the current position of each slice given from the properties
//      */
//     std::vector< boost::shared_ptr< const WPVDouble > >  m_slicePos;
//
//     /**
//      * List of probabilisitc tractograms.
//      */
//     ProbTractList m_probTracts;
//
//     /**
//      * Axis aligned bounding box for each slice.
//      */
//     std::vector< WBoundingBox > m_sliceBB;
//
// private:
//     /**
//      * Ensures that every grid of each probabilistic tractogram is of type WGridRegular3D and that they are all the same.
//      *
//      * \todo Check that each grid is the same as soon as operator== for WGridRegular3D is available
//      * \throw WTypeMismatched when a grid was found which is not castable to WGridRegular3D
//      */
//     void checkAndExtractGrids();
//
//     /**
//      * Reference to the color properites.
//      */
//     std::vector< WPropGroup > m_colorMap;
// };
//
#endif  // WSPSLICEBUILDER_H
