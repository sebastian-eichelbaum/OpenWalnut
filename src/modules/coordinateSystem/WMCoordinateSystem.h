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

#include "../../dataHandler/WDataSet.h"
#include "../../kernel/WModule.h"
#include "../../kernel/WModuleConnector.h"
#include "../../kernel/WModuleInputData.hpp"

/**
 * class that implements the various coordinate systems as overlays within the 3D view
 */
class WMCoordinateSystem : public WModule, public osg::Referenced
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
     * Connect the listener function of the module to the gui signals
     * this has to be called after full initialization fo the gui
     */
    void connectToGui();

    /**
     * Due to the prototype design pattern used to build modules, this method returns a new instance of this method. NOTE: it
     * should never be initialized or modified in some other way. A simple new instance is required.
     * 
     * \return the prototype used to create every module in OpenWalnut.
     */
    virtual boost::shared_ptr< WModule > factory() const;

protected:
    /**
     * \par Description
     * Entry point after loading the module. Runs in separate thread.
     */
    virtual void threadMain();

private:
    /**
     * initialize the properties for this module
     */
    void properties();

    /**
     * initial creation function for the slice geometry
     */
    void createGeometry();

    /**
     * the root node for this module
     */
    osg::ref_ptr<osg::Geode> m_rootNode;

    /**
     * the shader object for this module
     */
    // boost::shared_ptr< WShader >m_shader;

    /**
     * lock to prevent concurrent threads trying to update the osg node
     */
    boost::shared_mutex m_updateLock;
};


class coordinateNodeCallback : public osg::NodeCallback
{
public:
    explicit coordinateNodeCallback( boost::shared_ptr< WMCoordinateSystem > module )
    {
        m_module = module;
    }

    virtual void operator()( osg::Node* node, osg::NodeVisitor* nv )
    {
        if ( m_module )
        {
            m_module->updateGeometry();
        }
        traverse( node, nv );
    }
private:
    boost::shared_ptr< WMCoordinateSystem > m_module;
};




#endif  // WMCOORDINATESYSTEM_H
