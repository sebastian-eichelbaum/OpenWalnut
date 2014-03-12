//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2009 OpenWalnut Community, BSV-Leipzig and CNCF-CBS
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

#ifndef WMDATASETPROFILE_H
#define WMDATASETPROFILE_H

#include <osgWidget/WindowManager>

#include <string>
#include <vector>

#include <osg/Geode>

#include "core/dataHandler/WDataSetScalar.h"
#include "core/dataHandler/WDataSetSingle.h"

#include "core/graphicsEngine/WGEManagedGroupNode.h"
#include "core/graphicsEngine/WROISphere.h"
#include "core/graphicsEngine/WGECamera.h"

#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleOutputData.h"

const unsigned int MASK_2D = 0xF0000000; //!< used for osgWidget stuff
const unsigned int MASK_3D = 0x0F000000; //!< used for osgWidget stuff

/**
 * Someone should add some documentation here.
 * Probably the best person would be the module's
 * creator, i.e. "schurade".
 *
 * This is only an empty template for a new module. For
 * an example module containing many interesting concepts
 * and extensive documentation have a look at "src/modules/template"
 *
 * \ingroup modules
 */
class WMDatasetProfile: public WModule
{
public:
    /**
     *
     */
    WMDatasetProfile();

    /**
     *
     */
    virtual ~WMDatasetProfile();

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

    /**
     * Initialize requirements for this module.
     */
    virtual void requirements();

    /**
     * update function, called with each update pass of the osg render loop
     */
    void updateCallback();


private:
    /**
     * initializes the controls and osg nodes
     */
    void init();

    /**
     * updates the osg nodes
     */
    void update();

    /**
     * sets the dirty flag, used as callback for control knob changes
     */
    void setDirty();

    /**
     * dirty flag to indicate that redrawing is needed
     */
    bool m_dirty;

    /**
     * creates an osg node that contains the underlying grid
     * \return osg::ref_ptr< osg::Geode >
     */
    osg::ref_ptr< osg::Geode > createGraphGridGeode();

    /**
     * creates an osg node that contains the graph
     * \return osg::ref_ptr< osg::Geode >
     */
    osg::ref_ptr< osg::Geode > createGraphGeode();

    /**
     * adds a control knob
     * \param pos Position of the new knob
     */
    void addKnob( WPosition pos );

    /**
     * An input connector that accepts order 1 datasets.
     */
    boost::shared_ptr< WModuleInputData< WDataSetScalar > > m_input;

    /**
     * This is a pointer to the dataset the module is currently working on.
     */
    boost::shared_ptr< WDataSetScalar > m_dataSet;

    /**
     * stores a pointer to the grid we use;
     */
    boost::shared_ptr< WGridRegular3D > m_grid;



    /**
     * A condition used to notify about changes in several properties.
     */
    boost::shared_ptr< WCondition > m_propCondition;

    /**
     * A list of cluster selection methods
     */
    boost::shared_ptr< WItemSelection > m_snapSelectionsList;

    /**
     * Selection property for clusters
     */
    WPropSelection m_snapSelection;

    WPropTrigger  m_propAddKnobTrigger; //!< Adds a knob


    WPropColor m_graphColor; //!< color of the graph

    WPropDouble m_propLength; //!< length between knobs

    WPropBool m_propUseLength; //!< enforce lengths

    WPropBool m_propInterpolate; //!< interpolate the graph

    WPropInt m_propNumSamples; //!< number of sample points on the graph

    /**
     * The root node used for this modules graphics.
     */
    osg::ref_ptr< WGEManagedGroupNode > m_rootNode;

    osg::ref_ptr< WGEManagedGroupNode > m_graphNode; //!< Pointer to the graph group node.

    /**
     * The geometry rendered by this module.
     */
    osg::ref_ptr< osg::Geode > m_lineGeode;

    /**
     * osg node for the grid
     */
    osg::ref_ptr< osg::Geode > m_graphGridGeode;

    /**
     * osg node for the graph
     */
    osg::ref_ptr< osg::Geode > m_graphGeode;

    /**
     * stores pointers to the control knobs
     */
    std::vector< osg::ref_ptr<WROISphere> >knobs;

    boost::shared_ptr< boost::function< void() > > m_changeRoiSignal; //!< Signal that can be used to update the graph

    int m_oldViewHeight; //!< stores the old viewport resolution to check whether a resize happened
    int m_oldViewWidth; //!< stores the old viewport resolution to check whether a resize happened

    osgWidget::WindowManager* m_wm; //!< stores a pointer to the window manager used for osg wdgets and overlay stuff

    WGECamera* m_camera; //!< stores the camera object
};

#endif  // WMDATASETPROFILE_H
