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

#include "WRulerOrtho.h"

#include "../../dataHandler/WDataSet.h"
#include "../../graphicsEngine/WGEGroupNode.h"
#include "../../kernel/WModule.h"
#include "../../kernel/WModuleConnector.h"
#include "../../kernel/WModuleInputData.h"

/**
 * class that implements the various coordinate systems as overlays within the 3D view
 * \ingroup modules
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
     * \par Description
     * Entry point after loading the module. Runs in separate thread.
     */
    virtual void moduleMain();

private:

    WPropBool m_dataSetAddedProp; //!< TODO(schurade): document this.

    WPropInt m_axialPosProp; //!< Axial slice position.
    WPropInt m_coronalPosProp; //!< Coronal slice position.
    WPropInt m_sagittalPosProp; //!< Sagittal slice position.

    WPropDouble m_fltXProp; //!< Front left top corner X comp.
    WPropDouble m_fltYProp; //!< Front left top corner Y comp.
    WPropDouble m_fltZProp; //!< Front left top corner Z comp.

    WPropDouble m_brbXProp; //!< Back right bottom corner X comp.
    WPropDouble m_brbYProp; //!< Back right bottom corner Y comp.
    WPropDouble m_brbZProp; //!< Back right bottom corner Z comp.

    WPropDouble m_zeroXProp; //!< Zero point X comp.
    WPropDouble m_zeroYProp; //!< Zero point Y comp.
    WPropDouble m_zeroZProp; //!< Zero point Z comp.

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
     * helper funktion to create the actual geometry
     */
    osg::ref_ptr<osg::Geometry> createGeometryNode();

    /**
     * helper function that finds the bounding box of the topmost dataset in the datasetbrowser
     */
    void findBoundingBox();

    /**
     * the root node for this module
     */
    osg::ref_ptr< WGEGroupNode > m_rootNode;

    /**
     * node for the bounding box
     */
    osg::ref_ptr<osg::Geode> m_boxNode;

    /**
     * node for the bounding box
     */
    osg::ref_ptr< osg::Group > m_rulerNode;


    /**
     * the shader object for this module
     */
    // boost::shared_ptr< WShader >m_shader;

    /**
     * lock to prevent concurrent threads trying to update the osg node
     */
    boost::shared_mutex m_updateLock;

    /**
     * Node callback to handle updates properly
     */
    class coordinateNodeCallback : public osg::NodeCallback
    {
    public: //NOLINT
        /**
         * operator ()
         *
         * \param node the osg node
         * \param nv the node visitor
         */
        virtual void operator()( osg::Node* node, osg::NodeVisitor* nv )
        {
            osg::ref_ptr< WMCoordinateSystem > module = static_cast< WMCoordinateSystem* > ( node->getUserData() );
            if ( module )
            {
                module->updateGeometry();
            }
            traverse( node, nv );
        }
    };
};

#endif  // WMCOORDINATESYSTEM_H
