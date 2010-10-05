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

#ifndef WMLIC_H
#define WMLIC_H

#include <string>

#include <osg/Geode>

#include "../../dataHandler/WDataSetVector.h"
#include "../../graphicsEngine/WTriangleMesh.h"
#include "../../kernel/WModule.h"
#include "../../kernel/WModuleInputData.h"
#include "../../kernel/WModuleOutputData.h"

/**
 * Computes a Line Intregrated Convolution from a vector field onto a triangle mesh.
 * \warning Most of the code is taken from fantom and the fibernavigator. DO NOT scatter those sources in OpenWalnut!
 * \ingroup modules
 */
class WMLIC: public WModule
{
public:
    /**
     * Constructs a new LIC module
     */
    WMLIC();

    /**
     * Destructs and clean up this module
     */
    virtual ~WMLIC();

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
     * Generates an OSG geode for the mesh
     *
     * \param mesh the mesh which represents the LIC
     */
    void renderMesh( boost::shared_ptr< WTriangleMesh > mesh );

    void activate();

private:
    /**
     * Searches for the first dataset in subject 0 containing the phrase "evec" in its filename and check if its a vector dataset.
     *
     * \return reference to the vector dataset
     */
    boost::shared_ptr< WDataSetVector > searchVectorDS() const;

    boost::shared_ptr< WModuleInputData< WTriangleMesh > > m_meshIC; //!< The InputConnector for the mesh on which to paint
// boost::shared_ptr< WModuleInputData< WDataSetVector > > m_vectorIC; //!< The InputConnector for the vectors that are used to build the streamlines

    boost::shared_ptr< WTriangleMesh > m_inMesh; //!< The mesh given from the input connector
    boost::shared_ptr< WDataSetVector > m_inVector; //!< The vector field used to compute the LIC given from the input connector
// boost::shared_ptr< WModuleOutputData< WTriangleMesh > > m_meshOC; //!< OutputConnector for the LIC'ed mesh

    osg::ref_ptr< WGEGroupNode > m_moduleNode; //!< Pointer to the modules group node.
    osg::ref_ptr< osg::Geode > m_surfaceGeode; //!< Pointer to geode containing the surface.
    osg::ref_ptr< WShader > m_shader; //!< The shader used for the iso surface in m_geode
};

#endif  // WMLIC_H
