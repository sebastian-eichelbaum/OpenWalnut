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

#include <vector>

#include <boost/shared_ptr.hpp>

#include <osg/Geode>

#include "../../common/WProperties.h"
#include "../../common/WPropertyTypes.h"
#include "WSPSliceBuilder.h"

class WDataSetVector;

/**
 * This Builder is used to generate views of probabilistic tractograms ala Schmahmann and Pandya.
 */
class WSPSliceBuilderVectors : public WSPSliceBuilder
{
public:
    /**
     * Creates a geode builder to build axial, coronal and sagittal slices ala Schahmann y Pandya.
     *
     * \param probTracts The list of probabilistic tractograms which should be taken into account.
     * \param sliceGroup Slice positions
     * \param colorMap For each connected probabilistic trac its color
     * \param vector Vector dataset with the largest eigen vectors.
     * \param vectorGroup Properties for some parameters of this drawing method
     */
    WSPSliceBuilderVectors( ProbTractList probTracts, WPropGroup sliceGroup, std::vector< WPropGroup > colorMap,
            boost::shared_ptr< const WDataSetVector > vector, WPropGroup vectorGroup );

    /**
     * Implements the preprocessing interface \ref WSPSliceBuilder.
     */
    void preprocess();

    /**
     * Implements the generateSlice interface \ref WSPSliceBuilder.
     * This generates for each axis a group node with some geodes.
     *
     * \param sliceNum 0 == xSlice, 1 ySlice, 2 zSlice.
     *
     * \return The group of nodes for the given slice.
     */
    osg::ref_ptr< WGEGroupNode > generateSlice( const unsigned char sliceNum ) const;

private:
//    osg::ref_ptr< osg::Vec3Array > generateQuadStubs( const wmath::WPosition& pos ) const;
    osg::ref_ptr< osg::Vec3Array > generateQuadTexCoords( std::pair< unsigned char, unsigned char > activeDims, double size ) const;

//    boost::shared_ptr< std::vector< wmath::WVector3D > > generateClockwiseDir( std::pair< unsigned char, unsigned char > activeDims, double distance ) const;

    boost::shared_ptr< const WDataSetVector > m_vectors;

    std::pair< unsigned char, unsigned char > computeSliceBase( const unsigned char sliceNum, boost::shared_ptr< wmath::WVector3D > origin,
            boost::shared_ptr< wmath::WVector3D > a, boost::shared_ptr< wmath::WVector3D > b ) const;

    boost::shared_ptr< const WPVInt > m_spacing;

    boost::shared_ptr< const WPVDouble > m_probThreshold;
};

#endif  // WSPSLICEBUILDERVECTORS_H
