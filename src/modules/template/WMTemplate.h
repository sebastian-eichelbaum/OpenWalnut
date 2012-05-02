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

#ifndef WMTEMPLATE_H
#define WMTEMPLATE_H

#include <string>

#include <osg/Geode>

#include "core/common/WItemSelection.h"
#include "core/common/WItemSelector.h"

#include "core/graphicsEngine/WGEManagedGroupNode.h"

#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleOutputData.h"

/**
 * This module is intended to be a simple template and example module. It can be used for fast creation of new modules by copying and refactoring
 * the files. It shows the basic usage of properties, update callbacks and how to wait for data.
 *
 * \ingroup modules
 */
class WMTemplate: public WModule
{
public:
    /**
     * Default constructor.
     */
    WMTemplate();

    /**
     * Destructor.
     */
    virtual ~WMTemplate();

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
     * The root node used for this modules graphics. For OSG nodes, always use osg::ref_ptr to ensure proper resource management.
     */
    osg::ref_ptr< WGEManagedGroupNode > m_rootNode;

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
     * An input connector used to get datasets from other modules. The connection management between connectors must not be handled by the module.
     */
    boost::shared_ptr< WModuleInputData< WDataSetSingle > > m_input;

    /**
     * The output connector used to provide the calculated data to other modules.
     */
    boost::shared_ptr< WModuleOutputData< WDataSetSingle > > m_output;

    /**
     * A condition used to notify about changes in several properties.
     */
    boost::shared_ptr< WCondition > m_propCondition;

    /**
     * To group properties, use WPropGroup.
     */
    WPropGroup    m_group1;

    /**
     * To group properties, use WPropGroup.
     */
    WPropGroup    m_group1a;

    /**
     * To group properties, use WPropGroup.
     */
    WPropGroup    m_group2;

    /**
     * En/Disables an feature.
     */
    WPropBool     m_enableFeature;

    /**
     * A nice feature trigger inside m_group1
     */
    WPropBool     m_group1Bool;

    /**
     * An integer value.
     */
    WPropInt      m_anInteger;

    /**
     * An integer value. It is synchronized with m_anInteger.
     */
    WPropInt      m_anIntegerClone;

    /**
     * A double value.
     */
    WPropDouble   m_aDouble;

    /**
     * A string.
     */
    WPropString   m_aString;

    /**
     * A filename.
     */
    WPropFilename m_aFile;

    /**
     * A color.
     */
    WPropColor    m_aColor;

    /**
     * A int property used to show hide/un-hide feature.
     */
    WPropInt      m_aHiddenInt;

    /**
     * A group used to show that even hiding whole groups is possible.
     */
    WPropGroup    m_aHiddenGroup;

    /**
     * A trigger which can be used to trigger some kind of operation.
     */
    WPropTrigger  m_aTrigger;

    /**
     * A property allowing the user to select ONE item of some list
     */
    WPropSelection m_aSingleSelection;

    /**
     * A property allowing the user to select multiple elements of a list.
     */
    WPropSelection m_aMultiSelection;

    // Outputs -> demonstrate the use of information properties:

    /**
     * A property simply providing a integer value to the outside world.
     */
    WPropInt      m_aIntegerOutput;

    /**
     * A property simply providing a double value to the outside world.
     */
    WPropDouble   m_aDoubleOutput;

    /**
     * A property simply providing a int value to the outside world.
     */
    WPropInt      m_aIntOutput;

    /**
     * A property simply providing some text to the outside world.
     */
    WPropString   m_aStringOutput;

    /**
     * A Property used to show some color to the user.
     */
    WPropColor    m_aColorOutput;

    /**
     * A Property used to store some position.
     */
    WPropPosition   m_aPosition;

    /**
     * A Property used to show some filename to the user.
     */
    WPropFilename m_aFilenameOutput;

    /**
     * A Property used to show some trigger to the user.
     */
    WPropTrigger m_aTriggerOutput;

    /**
     * A Property used to show some selection to the user.
     */
    WPropSelection m_aSelectionOutput;

    /**
     * A list of items that can be selected using m_aSingleSelection or m_aMultiSelection.
     */
    boost::shared_ptr< WItemSelection > m_possibleSelections;

    /**
     * A Property used to show the callback mechanism avoiding the thread wake up on change.
     */
    WPropTrigger m_hideButton;

    /**
     * This causes an exception to be thrown on press to demonstrate how the GUI handles crashing modules.
     */
    WPropTrigger m_exceptionTrigger;

    /**
     * The callback triggering the hide flag of m_aColor for demonstration.
     */
    void hideButtonPressed();

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
        explicit SafeUpdateCallback( WMTemplate* module ): m_module( module ), m_initialUpdate( true )
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
        WMTemplate* m_module;

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
        explicit TranslateCallback( WMTemplate* module ): m_module( module ), m_initialUpdate( true )
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
        WMTemplate* m_module;

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
        virtual bool accept( boost::shared_ptr< WPropertyVariable< WPVBaseTypes::PV_STRING > >  property, const WPVBaseTypes::PV_STRING& value );

        /**
         * Method to clone the constraint and create a new one with the correct dynamic type.
         *
         * \return the constraint.
         */
        virtual boost::shared_ptr< WPropertyVariable< WPVBaseTypes::PV_STRING >::PropertyConstraint > clone();
    };
};

#endif  // WMTEMPLATE_H
