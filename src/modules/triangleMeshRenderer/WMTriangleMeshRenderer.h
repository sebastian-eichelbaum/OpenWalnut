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

#ifndef WMTRIANGLEMESHRENDERER_H
#define WMTRIANGLEMESHRENDERER_H

#include <string>

#include <osg/Geode>

#include "../../graphicsEngine/WGEGroupNode.h"
#include "../../kernel/WModule.h"
#include "../../kernel/WModuleInputData.h"
#include "../../kernel/WModuleOutputData.h"

class WTriangleMesh;

/**
 * Someone should add some documentation here.
 * Probably the best person would be the module's
 * creator, i.e. "wiebel".
 *
 * This is only an empty template for a new module. For
 * an example module containing many interesting concepts
 * and extensive documentation have a look at "src/modules/template"
 *
 * \ingroup modules
 */
class WMTriangleMeshRenderer: public WModule
{
public:

    /**
     *
     */
    WMTriangleMeshRenderer();

    /**
     *
     */
    virtual ~WMTriangleMeshRenderer();

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
     * Callback for m_active. Overwrite this in your modules to handle m_active changes separately.
     */
    virtual void activate();

private:

    /**
     * An input connector used to get mehses from other modules. The connection management between connectors must not be handled by the module.
     */
    boost::shared_ptr< WModuleInputData< WTriangleMesh > > m_input;

    osg::ref_ptr< WGEGroupNode > m_moduleNode; //!< Pointer to the modules group node.

    osg::ref_ptr< osg::Geode > m_surfaceGeode; //!< Pointer to geode containing the surface.

    /**
     * This function generates the osg geometry from the WTriangleMesh.
     * \param mesh The triangle mesh that will be rendered.
     */
    void renderMesh( boost::shared_ptr< WTriangleMesh > mesh );
};

#endif  // WMTRIANGLEMESHRENDERER_H