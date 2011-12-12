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

#ifndef WMCOORDINATEHUD_H
#define WMCOORDINATEHUD_H

#include <string>

#include <osg/Geode>

#include "core/common/WItemSelection.h"
#include "core/common/WItemSelector.h"

#include "core/graphicsEngine/WGEManagedGroupNode.h"
#include "core/graphicsEngine/WGEZoomTrackballManipulator.h"
#include "core/graphicsEngine/shaders/WGEShader.h"
#include "core/graphicsEngine/shaders/WGEShaderDefineOptions.h"

#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleOutputData.h"

/**
 * This module shows a coordinatesystem as HUD.
 * The attenuation can be customized to users needs.
 */
class WMCoordinateHUD: public WModule, public osg::Referenced
{
public:
    /**
     * Default constructor.
     */
    WMCoordinateHUD();

    /**
     * Destructor.
     */
    virtual ~WMCoordinateHUD();

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
     * Build the geode for colorfull coordinate axis
     */
    virtual void buildColorAxis();

    /**
     * build the geode for black & white coordinate axis
     */
    virtual void buildBWAxis();

    /**
     * build the geode for black & white coordinate cube
     *
     * \param withFaceLabels if true, a face texture is used
     * \param labelPrefix label files prefix
     */
    virtual void buildColorCube( bool withFaceLabels = true, std::string labelPrefix = "medical" );

    /**
     * build the geometry of the cube
     *
     * \param texcoords the right tex coord array. Can be NULL if not needed.
     *
     * \return the cube vertices
     */
    virtual osg::Vec3Array* buildCubeVertices( osg::Vec2Array* texcoords = NULL );

    /**
     * build the geometry of the axis
     *
     * \return the axis vertices
     */
    virtual osg::Vec3Array* buildAxisVertices();

    /**
     * The root node used for this modules graphics.
     */
    osg::ref_ptr< WGEManagedGroupNode > m_rootNode;

    /**
     * The geometry rendered by this module.
     */
    osg::ref_ptr< osg::Geode > m_geode;

    /**
     * The caption rendered by this module.
     */
    osg::ref_ptr< osg::Geode > m_txtGeode;

private:
    /**
     * the shader
     */
    osg::ref_ptr< WGEShader > m_shader;

    /**
     * Tells whether to enable the face label texture.
     */
    WGEShaderDefineOptions::SPtr m_enableFaceTexture;

    /**
     * A condition used to notify about changes in several properties.
     */
    boost::shared_ptr< WCondition > m_propCondition;

    /**
     * A property allowing the user to select ONE item of some list
     */
    WPropSelection m_aSingleSelection;

    /**
     * A list of items that can be selected using m_aSingleSelection or m_aMultiSelection.
     */
    boost::shared_ptr< WItemSelection > m_possibleSelections;
};

#endif  // WMCOORDINATEHUD_H
