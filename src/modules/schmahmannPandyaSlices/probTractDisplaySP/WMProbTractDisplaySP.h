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

#ifndef WMPROBTRACTDISPLAYSP_H
#define WMPROBTRACTDISPLAYSP_H

#include <string>
#include <vector>

#include "core/kernel/WModule.h"

// forward declarations to reduce compile dependencies
template< class T > class WModuleInputData;
class WDataSetFibers;
class WDataSetScalar;
class WDataSetVector;
class WGEManagedGroupNode;
class WSPSliceBuilder;

/**
 * This module computes for axial, coronal and sagittal views so called Schmahmann and Pandya slices in order to visualize
 * probabilistic tractograms.
 *
 * \ingroup modules
 */
class WMProbTractDisplaySP: public WModule
{
public:
    /**
     * Default constructor.
     */
    WMProbTractDisplaySP();

    /**
     * Destructor.
     */
    virtual ~WMProbTractDisplaySP();

    /**
     * Gives back the name of this module.
     * \return the module's name.
     */
    virtual const std::string getName() const;

    /**
     * Gives back a description of this module.
     * \return description to module.
     */
    virtual const std::string getDescription() const;

    /**
     * Due to the prototype design pattern used to build modules, this method returns a new instance of this method. NOTE: it
     * should never be initialized or modified in some other way. A simple new instance is required.
     *
     * \return the prototype used to create every module in OpenWalnut.
     */
    virtual boost::shared_ptr< WModule > factory() const;

    /**
     * Get the icon for this module in XPM format.
     * \return The icon.
     */
    virtual const char** getXPMIcon() const;

protected:
    /**
     * Entry point after loading the module. Runs in separate thread.
     */
    virtual void moduleMain();

    /**
     * Initialize the connectors this module is using.
     */
    virtual void connectors();

    /**
     * Initialize the properties for this module.
     */
    virtual void properties();

private:
    /**
     * Creates for each slice (axial, sagittal and coronal) a root node, to put there projections and intersection geometries.
     */
    void initOSG();

    /**
     * Resets the max value for the slice position sliders and sets it afterwards the middle position. Whenever the grid changes
     * we must update the slice position properties to its min max dimensions.
     *
     * \param grid The grid of the probabilistic tractogram
     */
    void resetSlicePos( boost::shared_ptr< const WGridRegular3D > grid );

    /**
     * Updates all or just one slice depending on which property or situation has changed. E.g. color changes influence all slices
     * but slice position changes just a single slice.
     *
     * \param builder An instance of a slice builder, correctly initialized and ready to use.
     */
    void updateSlices( boost::shared_ptr< WSPSliceBuilder > builder );

    /**
     * If there is a change on the probTract ICs then this update callback is called and ensures that for every connected
     * probabilistic tractogram input connector there is a visible (unhidden) property group to specify the color.
     *
     * \param receiver unused here
     * \param sender unused here
     */
    void updateProperitesForTheInputConnectors( boost::shared_ptr< WModuleConnector > receiver, boost::shared_ptr< WModuleConnector > sender );

    /**
     * Checks if every probabilistic tract is in range 0..1, if not e.g. max > 10 range 0..255 is assumed and a warning is printed.
     *
     * \param probTracts All probabilistic tracts to check.
     */
    void checkProbabilityRanges( std::vector< boost::shared_ptr< const WDataSetScalar > > probTracts ) const;

    /**
     * The probabilistic tractogram input connector.
     */
    std::vector< boost::shared_ptr< WModuleInputData< WDataSetScalar > > > m_probICs;

    /**
     * Input connector for the largest eigen vector dataset.
     */
    boost::shared_ptr< WModuleInputData< WDataSetVector > > m_vectorIC;

    /**
     * The tracts input connector.
     */
    boost::shared_ptr< WModuleInputData< WDataSetFibers > > m_tractsIC;

    /**
     * The Geode containing all the slices and the mesh
     */
    osg::ref_ptr< WGEManagedGroupNode > m_output;

    /**
     * Axial, coronal and sagittal slices as array.
     * 0 : xSlice, 1 : ySlice, 2 : zSlice
     */
    boost::array< osg::ref_ptr< WGEManagedGroupNode >, 3 > m_slices;

    /**
     * The group contains several slice properties
     */
    WPropGroup    m_sliceGroup;

    /**
     * Position of the axial, sagittal and coronal slices.
     * 0 : xSlice, 1 : ySlice, 2 : zSlice
     */
    boost::array< WPropDouble, 3 > m_slicePos;

    /**
     * Indicates if the corresponding slice is shown or not.
     * 0 : xSlice, 1 : ySlice, 2 : zSlice
     */
    boost::array< WPropBool, 3 > m_showSlice;

    /**
     * The group contains several properties for tract based drawing
     */
    WPropGroup    m_tractGroup;

    /**
     * Switch on or off the intersecting line stipplings
     */
    WPropBool     m_showIntersection;

    /**
     * Switch on or off the projections of the intersecting line stipplings
     */
    WPropBool     m_showProjection;

    /**
     * Environment around the slices where to cut off the tracts
     */
    WPropDouble   m_delta;

    /**
     * Probabilities a position below this threshold does not contribute to the vertex coloring
     */
    WPropDouble   m_probThreshold;

    /**
     * The group contains several properties for eigen vector based drawing
     */
    WPropGroup    m_vectorGroup;

    /**
     * There is one group for each pobTract input connector holding a string property and a color property.
     */
    std::vector< WPropGroup > m_colorMap;

    /**
     * Fires whenever a color group has changed its color value.
     */
    boost::shared_ptr< WCondition > m_colorChanged;

    /**
     * Condition to notify about changes of the slices.
     */
    boost::shared_ptr< WCondition > m_sliceChanged;


    /**
     * Property accessor for the Algorithm list.
     */
    WPropSelection m_drawAlgorithm;
};

#endif  // WMPROBTRACTDISPLAYSP_H
