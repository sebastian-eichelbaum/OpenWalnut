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

#ifndef WMGEOMETRYGLYPHS_H
#define WMGEOMETRYGLYPHS_H

#include <string>

#include <osg/Node>
#include <osg/Geode>
#include <osg/Uniform>

#include "../../kernel/WModule.h"
#include "../../kernel/WModuleInputData.h"
#include "../../kernel/WModuleOutputData.h"

#include "../../dataHandler/WDataSetSingle.h"
#include "../../dataHandler/WDataSetVector.h"

#include "../../graphicsEngine/WGEGroupNode.h"
#include "../../graphicsEngine/WTriangleMesh2.h"

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
class WMGeometryGlyphs: public WModule
{
public:

    /**
     *
     */
    WMGeometryGlyphs();

    /**
     *
     */
    virtual ~WMGeometryGlyphs();

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


private:
    void prepareGlyphes(); //!< initial creation of glyphs

    /**
     * render function
     * \param mesh
     */
    void renderMesh( boost::shared_ptr< WTriangleMesh2 > mesh );

    WPropBool m_useTextureProp; //!< Property indicating whether to use texturing with scalar data sets.

    osg::ref_ptr< WGEGroupNode > m_moduleNode; //!< Pointer to the modules group node. We need it to be able to update it when callback is invoked.

    osg::ref_ptr< osg::Geode > m_outputGeode; //!< Pointer to geode containing the glpyhs

    boost::shared_ptr< WModuleInputData< WDataSetSingle > > m_input;  //!< Input connector required by this module.

    boost::shared_ptr< const WDataSetSingle > m_dataSet; //!< pointer to dataSet to be able to access it throughout the whole module.

    boost::shared_ptr< WTriangleMesh2 > m_triMesh; //!< This triangle mesh is provided as output through the connector.

    WPropInt      m_xPos; //!< x posistion of the slice

    WPropInt      m_yPos; //!< y posistion of the slice

    WPropInt      m_zPos; //!< z posistion of the slice

    WPropBool     m_showonX; //!< in dicates whether the vector should be shown on slice X

    WPropBool     m_showonY; //!< in dicates whether the vector should be shown on slice Y

    WPropBool     m_showonZ; //!< in dicates whether the vector should be shown on slice Z
};

#endif  // WMGEOMETRYGLYPHS_H
