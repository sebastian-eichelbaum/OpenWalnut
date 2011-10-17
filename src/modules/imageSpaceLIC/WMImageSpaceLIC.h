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

#ifndef WMIMAGESPACELIC_H
#define WMIMAGESPACELIC_H

#include <string>
#include <vector>

#include "core/dataHandler/WDataSetVector.h"
#include "core/dataHandler/WDataSetScalar.h"
#include "core/dataHandler/WSubject.h"
#include "core/graphicsEngine/WTriangleMesh.h"
#include "core/graphicsEngine/WGEManagedGroupNode.h"

#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"

/**
 * This module takes an vector dataset and used it to apply an image space based (fast) LIC to an arbitrary surface. The surface can be specified
 * as tri mesh or, if not specified, slices.
 *
 * \ingroup modules
 */
class WMImageSpaceLIC: public WModule
{
public:

    /**
     * Default constructor.
     */
    WMImageSpaceLIC();

    /**
     * Destructor.
     */
    virtual ~WMImageSpaceLIC();

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
     * \return the icon.
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
     * Initializes the needed geodes, transformations and vertex arrays. This needs to be done once for each new dataset.
     *
     * \param grid the grid to places the slices in
     * \param mesh the mesh to use if not NULL and m_useSlices is false
     */
    void initOSG( boost::shared_ptr< WGridRegular3D > grid, boost::shared_ptr< WTriangleMesh > mesh );

    /**
     * The input connector containing the vector field.
     */
    boost::shared_ptr< WModuleInputData< WDataSetVector > > m_vectorsIn;

    /**
     * The input connector containing the scalar field whose derived field is used for LIC.
     */
    boost::shared_ptr< WModuleInputData< WDataSetScalar > > m_scalarIn;

    /**
     * The input containing the surface on which the LIC should be applied on
     */
    boost::shared_ptr< WModuleInputData< WTriangleMesh > > m_meshIn;

    /**
     * A property allowing the user to select whether the slices or the mesh should be used
     */
    WPropSelection m_geometrySelection;

    /**
     * A list of items that can be selected using m_geometrySelection.
     */
    boost::shared_ptr< WItemSelection > m_geometrySelections;

    /**
     * A condition used to notify about changes in several properties.
     */
    boost::shared_ptr< WCondition > m_propCondition;

    /**
     * The Geode containing all the slices and the mesh
     */
    osg::ref_ptr< WGEManagedGroupNode > m_output;

    WPropGroup    m_sliceGroup; //!< the group contains several slice properties

    WPropGroup    m_geometryGroup; //!< the group contains several input geometry parameters

    WPropGroup    m_licGroup; //!< the group contains several LIC properties

    WPropBool     m_useSlices; //!< indicates whether the vector should be shown on slices or input geometry

    WPropInt      m_xPos; //!< x position of the slice

    WPropInt      m_yPos; //!< y position of the slice

    WPropInt      m_zPos; //!< z position of the slice

    WPropBool     m_showonX; //!< indicates whether the vector should be shown on slice X

    WPropBool     m_showonY; //!< indicates whether the vector should be shown on slice Y

    WPropBool     m_showonZ; //!< indicates whether the vector should be shown on slice Z

    WPropBool     m_showHUD; //!< indicates whether to show the texture HUD

    WPropBool     m_useEdges; //!< indicates whether to show the edges

    WPropBool     m_useLight; //!< indicates whether to use Phong

    WPropBool     m_useDepthCueing; //!< indicates whether to use depth cueing in the shader

    WPropBool     m_useHighContrast; //!< use the high contrast version?

    WPropInt      m_numIters; //!< the number of iterations done per frame

    WPropDouble   m_cmapRatio; //!< the ratio between colormap and LIC

    /**
     * The group for more advanced LIC features
     */
    WPropGroup    m_advancedLicGroup;

    /**
     * If true, a 3d noise texture is used for advection
     */
    WPropBool     m_3dNoise;

    /**
     * The resolution scaling for the 3d noise
     */
    WPropDouble   m_3dNoiseRes;

    /**
     * Automatically adapt resolution of 3d texture according to zoom level
     */
    WPropBool     m_3dNoiseAutoRes;
};

#endif  // WMIMAGESPACELIC_H

