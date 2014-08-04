//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2013 OpenWalnut Community, BSV-Leipzig and CNCF-CBS
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

#ifndef WMProject_H
#define WMProject_H

#include <string>

#include "core/kernel/WModule.h"

#include "core/common/WItemSelection.h"
#include "core/common/WItemSelectionItem.h"
#include "core/common/WItemSelectionItemTyped.h"
#include "core/common/WItemSelector.h"
#include "core/common/WPropertyHelper.h"

#include <osg/Geode>

#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleOutputData.h"

enum PlaneType
{
  NONE, MIP, COMPOSITING_F2B, COMPOSITING_B2F
};

// forward declarations to reduce compile dependencies
template< class T > class WModuleInputData;
class WDataSetScalar;
class WGEManagedGroupNode;

/**
 * Computes contour lines (aka isolines) for the given data and render them on a 2D plane.
 * \ingroup modules
 */
class WMProject: public WModule
{
public:
    /**
     * Creates the module for drawing contour lines.
     */
    WMProject();

    /**
     * Destroys this module.
     */
    virtual ~WMProject();

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
     *
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

    osg::ref_ptr< osg::Geode > m_geode;

private:
    /**
     * Input connector for scalar data.
     */
    boost::shared_ptr< WModuleInputData< WDataSetScalar > > m_scalarIC;
    
    /**
     * The transfer function as an input data set
     */
	boost::shared_ptr< WModuleInputData< WDataSetSingle > > m_transferFunction;

    /**
     * The OSG root node for this module. All other geodes or OSG nodes will be attached on this single node.
     */
	osg::ref_ptr< WGEManagedGroupNode > m_rootNode;
    /**
     * Needed for recreating the geometry, incase when resolution changes.
     */
	boost::shared_ptr< WCondition > m_propCondition;
    
	typedef WItemSelectionItemTyped< PlaneType > MyItemType;
    
	boost::shared_ptr< WItemSelection > m_possibleSelectionsUsingTypes;
    
	WPropSelection		m_singleSelectionAxialTop, m_singleSelectionAxialBottom,
		m_singleSelectionCoronalTop, m_singleSelectionCoronalBottom,
		m_singleSelectionSagittalTop, m_singleSelectionSagittalBottom;
		
	WPropDouble   m_earlyRayTerminationAlpha;

    // Output
	WPropPosition				m_maxCoordsPosition;
	WPropPosition				m_normalOut;
	WPropInt					m_planeTypes[6];

	osg::ref_ptr< WGEShader >	m_shader;

	boost::shared_ptr< WDataSetScalar > scalarData;
};

#endif  // WMProject_H
