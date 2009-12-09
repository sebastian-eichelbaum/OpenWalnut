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

#ifndef WMHUD_H
#define WMHUD_H

#include <string>

#include <osg/Group>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Node>

#include "../../kernel/WModule.h"

/**
 * This module implements several onscreen status displays
 */
class WMHud : public WModule, public osg::Referenced
{
public:
    /**
     * standard constructor
     */
    WMHud();

    /**
     * destructor
     */
    virtual ~WMHud();

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
     *
     */
    void updatePickText( std::string text );

protected:

    /**
     * \par Description
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
     * Receive DATA_CHANGE notifications.
     *
     * \param input the input connector that got the change signal. Typically it is one of the input connectors from this module.
     * \param output the output connector that sent the signal. Not part of this module instance.
     */
    virtual void notifyDataChange( boost::shared_ptr<WModuleConnector> input,
                                   boost::shared_ptr<WModuleConnector> output );

    /**
     * Due to the prototype design pattern used to build modules, this method returns a new instance of this method. NOTE: it
     * should never be initialized or modified in some other way. A simple new instance is required.
     *
     * \return the prototype used to create every module in OpenWalnut.
     */
    virtual boost::shared_ptr< WModule > factory() const;



private:
    // Projection node for defining view frustrum for HUD
    osg::ref_ptr<osg::Projection> m_rootNode;

    osg::ref_ptr<osg::Group>m_HUDs;

    // Text instance that wil show up in the HUD:
    osg::ref_ptr<osgText::Text> m_osgPickText;


    std::string m_pickText;

    void init();

    void update();

    /**
     * Gets signaled from the properties object when something was changed
     */
    void slotPropertyChanged( std::string propertyName );


    class HUDNodeCallback : public osg::NodeCallback
    {
    public: // NOLINT
        virtual void operator()( osg::Node* node, osg::NodeVisitor* nv )
        {
            osg::ref_ptr< WMHud > module = static_cast< WMHud* > ( node->getUserData() );

            if ( module )
            {
                module->update();
            }
            traverse( node, nv );
        }
    };
};

#endif  // WMHUD_H
