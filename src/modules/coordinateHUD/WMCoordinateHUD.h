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

#include "../../common/WItemSelection.h"
#include "../../common/WItemSelector.h"

#include "../../graphicsEngine/WGEManagedGroupNode.h"
#include "../../graphicsEngine/WShader.h"

#include "../../kernel/WModule.h"
#include "../../kernel/WModuleInputData.h"
#include "../../kernel/WModuleOutputData.h"

/**
 * This module is intended to be a simple template and example module. It can be used for fast creation of new modules by copying and refactoring
 * the files. It shows the basic usage of properties, update callbacks and how to wait for data.
 *
 * \ingroup modules
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
     * The root node used for this modules graphics. For OSG nodes, always use osg::ref_ptr to ensure proper resource management.
     */
    //osg::ref_ptr< WGEManagedGroupNode > m_rootNode;
    osg::ref_ptr< osg::Geode > m_rootNode;

    /**
     * The geometry rendered by this module.
     */
    osg::ref_ptr< osg::Geode > m_geode;

    /**
     * Callback for m_active. Overwrite this in your modules to handle m_active changes separately.
     */
    virtual void activate();

private:

    /**
     * the shader
     */
    osg::ref_ptr< WShader > m_shader;
};

#endif //WMCOORDINATEHUD_H

