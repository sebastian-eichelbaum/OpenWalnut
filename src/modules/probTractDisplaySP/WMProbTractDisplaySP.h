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

#include "../../common/WBoundingBox.h"
#include "../../dataHandler/WDataSetFibers.h"
#include "../../dataHandler/WDataSetScalar.h"
#include "../../dataHandler/WSubject.h"
#include "../../graphicsEngine/WGEManagedGroupNode.h"
#include "../../kernel/WModule.h"
#include "../../kernel/WModuleInputData.h"

class WSPSliceGeodeBuilder;

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
     * Whenever the grid changes we must update the slide properties, in order to represent valid maxima and minima again.
     *
     * \param grid The grid of the probabilistic tractogram
     */
    void updateProperties( boost::shared_ptr< const WGridRegular3D > grid );

    /**
     * Updates the axial, coronal or sagittal slices.
     *
     * \param sliceNum 0 means xSlice or sagittal slice, 1 means ySlice and 2 means zSlice.
     * \param builder An instance of a slice builder, correctly initialized and ready to use.
     */
    void updateSlices( const unsigned char sliceNum, boost::shared_ptr< const WSPSliceGeodeBuilder > builder );

    /**
     * The probabilistic tractogram input connector.
     */
    boost::shared_ptr< WModuleInputData< WDataSetScalar > > m_probIC;

    /**
     * The tracts input connector.
     */
    boost::shared_ptr< WModuleInputData< WDataSetFibers > > m_tractsIC;

    /**
     * The Geode containing all the slices and the mesh
     */
    osg::ref_ptr< WGEManagedGroupNode > m_output;

    /**
     * The transformation node moving the X slice through the dataset space if the sliders are used
     */
    osg::ref_ptr< WGEManagedGroupNode > m_xSlice;

    /**
     * The transformation node moving the Y slice through the dataset space if the sliders are used
     */
    osg::ref_ptr< WGEManagedGroupNode > m_ySlice;

    /**
     * The transformation node moving the Z slice through the dataset space if the sliders are used
     */
    osg::ref_ptr< WGEManagedGroupNode > m_zSlice;

    WPropGroup    m_sliceGroup; //!< the group contains several slice properties

    WPropInt      m_xPos; //!< x position of the slice

    WPropInt      m_yPos; //!< y position of the slice

    WPropInt      m_zPos; //!< z position of the slice

    WPropBool     m_showonX; //!< indicates whether the vector should be shown on slice X

    WPropBool     m_showonY; //!< indicates whether the vector should be shown on slice Y

    WPropBool     m_showonZ; //!< indicates whether the vector should be shown on slice Z

    WPropBool     m_showIntersection; //!< Switch on or off the intersecting line stipplings

    WPropBool     m_showProjection; //!< Switch on or off the projections of the intersecting line stipplings

    WPropDouble   m_delta; //!< Environment around the slices where to cut off the tracts

    /**
     * Condition to notify about changes of the slices.
     */
    boost::shared_ptr< WCondition > m_sliceChanged;
};

#endif  // WMPROBTRACTDISPLAYSP_H
