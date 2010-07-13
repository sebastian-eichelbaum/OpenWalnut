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
     * Projection node for defining view frustrum for HUD
     */
    osg::ref_ptr<osg::Projection> m_rootNode;

    /**
     * Geometry group for all HUD realted things
     */
    osg::ref_ptr< WGEGroupNode > m_HUDs;

    /**
     * Node callback to change the color of the shapes inside the root node. For more details on this class, refer to the documentation in
     * moduleMain().
     */
    class SafeUpdateCallback : public osg::NodeCallback
    {
    public: // NOLINT

        /**
         * Constructor.
         *
         * \param module just set the creating module as pointer for later reference.
         */
        explicit SafeUpdateCallback( WMCoordinateHUD* module ): m_module( module ), m_initialUpdate( true )
        {
        };

        /**
         * operator () - called during the update traversal.
         *
         * \param node the osg node
         * \param nv the node visitor
         */
        virtual void operator()( osg::Node* node, osg::NodeVisitor* nv );

        /**
         * Pointer used to access members of the module to modify the node.
         * Please do not use shared_ptr here as this would prevent deletion of the module as the callback contains
         * a reference to it. It is safe to use a simple pointer here as callback get deleted before the module.
         */
        WMCoordinateHUD* m_module;

        /**
         * Denotes whether the update callback is called the first time. It is especially useful
         * to set some initial value even if the property has not yet changed.
         */
        bool m_initialUpdate;
    };


    /**
     * Node callback to change the position of the shapes in the coordinate system of the scene.
     * For more details on this class, refer to the documentation in moduleMain().
     */
    class TranslateCallback : public osg::NodeCallback
    {
    public: // NOLINT

        /**
         * Constructor.
         *
         * \param module just set the creating module as pointer for later reference.
         */
        explicit TranslateCallback( WMCoordinateHUD* module ): m_module( module ), m_initialUpdate( true )
        {
        };

        /**
         * operator () - called during the update traversal.
         *
         * \param node the osg node
         * \param nv the node visitor
         */
        virtual void operator()( osg::Node* node, osg::NodeVisitor* nv );

        /**
         * Pointer used to access members of the module to modify the node.
         * Please do not use shared_ptr here as this would prevent deletion of the module as the callback contains
         * a reference to it. It is safe to use a simple pointer here as callback get deleted before the module.
         */
        WMCoordinateHUD* m_module;

        /**
         * Denotes whether the update callback is called the first time. It is especially useful
         * to set some initial value even if the property has not yet changed.
         */
        bool m_initialUpdate;
    };

    /**
     * This shows how to write custom constraints for your modules. Please refer to the documentation in properties() for more details.
     *
     * \note: always use WPVBaseTypes to specialize the PropertyVariable template.
     */
    class StringLength: public WPropertyVariable< WPVBaseTypes::PV_STRING >::PropertyConstraint
    {
        /**
         * You need to overwrite this method. It decides whether the specified new value should be accepted or not.
         *
         * \param property the property thats going to be changed.
         * \param value the new value
         *
         * \return true if the new value is OK.
         */
        virtual bool accept( boost::shared_ptr< WPropertyVariable< WPVBaseTypes::PV_STRING > >  property, WPVBaseTypes::PV_STRING value );

        /**
         * Method to clone the constraint and create a new one with the correct dynamic type.
         *
         * \return the constraint.
         */
        virtual boost::shared_ptr< WPropertyVariable< WPVBaseTypes::PV_STRING >::PropertyConstraint > clone();
    };
};

#endif  // WMTEMPLATE_H

