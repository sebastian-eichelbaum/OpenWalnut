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

#ifndef WMCOORDINATESYSTEM_H
#define WMCOORDINATESYSTEM_H

#include <string>

#include <osg/Geode>
#include <osg/Node>

#include "core/dataHandler/WDataSet.h"
#include "core/graphicsEngine/WGEGroupNode.h"
#include "core/kernel/WModule.h"
#include "core/kernel/WModuleConnector.h"
#include "core/kernel/WModuleInputData.h"

#include "WCoordConverter.h"
#include "WRulerOrtho.h"

/**
 * class that implements the various coordinate systems as overlays within the 3D view
 * \ingroup modules
 */
class WMCoordinateSystem : public WModule
{
public:
    /**
     * standard constructor
     */
    WMCoordinateSystem();

    /**
     * destructor
     */
    virtual ~WMCoordinateSystem();

    /**
     * Initialize the connectors this module is using.
     */
    virtual void connectors();

    /**
     * callback for updating the geometry
     */
    void updateGeometry();

    /**
     * \par Description
     * Gives back the name of this module.
     * \return the module's name.
     */
    virtual const std::string getName() const;

    /**
     * \par Description
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
     * \par Description
     * Entry point after loading the module. Runs in separate thread.
     */
    virtual void moduleMain();

private:
    WPropPosition m_crosshair; //!< position of the navigation slices
    WPropPosition m_flt; //!< lower boundary of the dataset
    WPropPosition m_brb; //!< upper boundary of the dataset

    WPropPosition m_ac; //!< anterior commissure
    WPropPosition m_pc; //!< posterior commissure
    WPropPosition m_ihp; //!< inter hemispherical point

    WPropBool m_showNumbersOnRulers; //!< show/hide numbers on rulers

    WPropBool m_showAxial; //!< show rulers on the axial slice
    WPropBool m_showCoronal; //!< show rulers on the coronal slice
    WPropBool m_showSagittal; //!< show rulers on the sagittal slice

    WPropBool m_showGridAxial; //!< show grid on the axial slice
    WPropBool m_showGridCoronal;  //!< show grid on the coronal slice
    WPropBool m_showGridSagittal;  //!< show grid on the sagittal slice

    WPropTrigger m_acTrigger; //!< button to reset the ac point
    WPropTrigger m_pcTrigger; //!< button to reset the pc point
    WPropTrigger m_ihpTrigger; //!< button to reset the ihp point;

    WPropSelection m_csSelection; //!< selection for coordinate system mode
    boost::shared_ptr< WItemSelection > m_possibleSelections;  //!< selection for coordinate system mode

    /**
     * A condition used to notify about changes in several properties.
     */
    boost::shared_ptr< WCondition > m_propCondition;

    boost::shared_ptr< WModuleInputData< WDataSetScalar > > m_input;  //!< Input connector required by this module.
    boost::shared_ptr< const WDataSetScalar > m_dataSet; //!< pointer to dataSet to be able to access it throughout the whole module.

    /**
     * the root node for this module
     */
    osg::ref_ptr< WGEGroupNode > m_rootNode;

    /**
     * node for the bounding box
     */
    osg::ref_ptr< osg::Group > m_rulerNode;

    bool m_dirty; //!< flag true if something happened that requires redrawing of gfx

    float m_drawOffset; //!< offset from slices to draw geometry on

    boost::shared_ptr< WCoordConverter > m_coordConverter; //!< stores pointer

    int m_viewAngle; //!< stores the last view angle

    /**
     * initialize the properties for this module
     */
    void properties();

    /**
     * Callback for m_active. Overwrite this in your modules to handle m_active changes separately.
     */
    virtual void activate();

    /**
     * initial creation function for the slice geometry
     */
    void createGeometry();

    /**
     * helper function that computes the innerBoundingBox for the actual data
     */
    void findBoundingBox();


    void initTalairachConverter(); //!< creates and initializes a talairach converter object

    void addRulersSagittal(); //!< helper function to create rulers
    void addRulersAxial(); //!< helper function to create rulers
    void addRulersCoronal(); //!< helper function to create rulers

    /**
     * helper function to create a grid overlay on the sagittal slice
     * \param position
     */
    void addSagittalGrid( float position );

    /**
     * helper function to create a grid overlay on the coronal slice
     * \param position
     */
    void addCoronalGrid( float position );

    /**
     * helper function to create a grid overlay on the axial slice
     * \param position
     */
    void addAxialGrid( float position );

    /**
     * Callback to listen for property changes
     */
    void propertyChanged();

    /**
     * the shader object for this module
     */
    // boost::shared_ptr< WGEShader >m_shader;

    /**
     * update function, called with each update pass of the osg render loop
     */
    void updateCallback();
};

#endif  // WMCOORDINATESYSTEM_H
