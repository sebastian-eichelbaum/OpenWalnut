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

#include "../../kernel/WModule.h"
#include "../../kernel/WModuleInputData.h"
#include "../../kernel/WModuleOutputData.h"

/**
 * This module is intended to be a simple template and example module. It can be used for fast creation of new modules by copying and refactoring
 * the files. It shows the basic usage of properties, update callbacks and how to wait for data.
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
    osg::ref_ptr<osg::Geode> m_rootNode;

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
     * This is a pointer to the dataset the module is currently working on.
     */
    boost::shared_ptr< WDataSetSingle > m_dataSet;

    /**
     * A condition used to notify about changes in several properties.
     */
    boost::shared_ptr< WCondition > m_propCondition;

    /**
     * En/Disables an feature.
     */
    WPropBool     m_enableFeature;

    /**
     * An integer value.
     */
    WPropInt      m_anInteger;

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
        explicit SafeUpdateCallback( WMTemplate* module ): m_module( module )
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
         */
        WMTemplate* m_module;
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
    };
};

#endif  // WMTEMPLATE_H

