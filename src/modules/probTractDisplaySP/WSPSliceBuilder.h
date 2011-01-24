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

#include <list>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "../../common/WProperties.h"
#include "../../common/WPropertyTypes.h"

// forward declarations
namespace osg
{
    template< class T > class ref_ptr;
}
class WDataSetScalar;
class WGEGroupNode;
class WGridRegular3D;

/**
 * This Builder is used to generate views of probabilistic tractograms ala Schmahmann and Pandya.
 */
class WSPSliceBuilder
{
public:
    /**
     * List of scalar datasets e.g. probabilistic tractograms.
     */
    typedef std::list< boost::shared_ptr< const WDataSetScalar > > ProbTractList;

    /**
     * Creates a geode builder to build axial, coronal and sagittal slices ala Schahmann y Pandya.
     *
     * \param probTracts The list of probabilistic tractograms which should be taken into account.
     * \param sliceGroup Slice positions
     * \param colorMap For each connected probabilistic trac its color
     */
    WSPSliceBuilder( ProbTractList probTracts, WPropGroup sliceGroup, std::vector< WPropGroup > colorMap );

    /**
     * Destructs this. We need a virtual desturctor as long as we have virtual member functions...
     */
    virtual ~WSPSliceBuilder();

    /**
     * Do some preprocessing which is needed for every slice redraw, e.g. when the slice has moved.
     */
    virtual void preprocess() = 0;

    /**
     * This generates for each axis a group node with some geodes.
     *
     * \param sliceNum 0 == xSlice, 1 ySlice, 2 zSlice.
     *
     * \return The group of nodes for the given slice.
     */
    virtual osg::ref_ptr< WGEGroupNode > generateSlice( const unsigned char sliceNum ) const = 0;

protected:
    /**
     * Very simple color mapping which is used temporarily, later we want to substitute this with a color chooser as properties.
     *
     * \param probTractNum Index of the probabilisitc tractrogram to get the color for.
     *
     * \return The color for the given prob tract index.
     */
    WColor colorMap( size_t probTractNum ) const;

    /**
     * The grid of the first tractogram. It is assumed that all given probablilisitc tractograms operate on the same grid.
     */
    boost::shared_ptr< const WGridRegular3D > m_grid;

    /**
     * Hold the current position of each slice given from the properties
     */
    std::vector< boost::shared_ptr< const WPVInt > >  m_slicePos;

    /**
     * List of probabilisitc tractograms.
     */
    ProbTractList m_probTracts;

private:
    /**
     * Ensures that every grid of each probabilistic tractogram is of type WGridRegular3D and that they are all the same.
     *
     * \todo Check that each grid is the same as soon as operator== for WGridRegular3D is available
     * \throw WTypeMismatched when a grid was found which is not castable to WGridRegular3D
     */
    void checkAndExtractGrids();

    /**
     * Reference to the color properites.
     */
    std::vector< WPropGroup > m_colorMap;
};

#endif  // WSPSLICEBUILDER_H
