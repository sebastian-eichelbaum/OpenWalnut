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
#include <osgText/Text>

#include "../../kernel/WModule.h"
#include "../../graphicsEngine/WGEGroupNode.h"
#include "../../graphicsEngine/WPickInfo.h"

/**
 * This module implements several onscreen status displays
 * \ingroup modules
 */
class WMHud : public WModule
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
     * Sets pick text member vairable
     *
     * \param pickInfo information about the pick
     */
    void updatePickText( WPickInfo pickInfo );

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

private:


    boost::shared_mutex m_updateLock; //!< Lock to prevent concurrent threads trying to update the osg node

    /**
     * Projection node for defining view frustrum for HUD
     */
    osg::ref_ptr<osg::Projection> m_rootNode;

    /**
     * Geometry group for all hud related things
     */
    osg::ref_ptr< WGEGroupNode > m_HUDs;

    /**
     * Text instance that will show up in the HUD
     */
    osg::ref_ptr< osgText::Text > m_osgPickText;

    /**
     * string to store the pick result from the picking method
     */
    std::string m_pickText;

    /**
     * init method
     */
    void init();

    /**
     * update method
     */
    void update();

    /**
     * Gets signaled from the properties object when something was changed. Now, only m_active is used. This method therefore simply
     * activates/deactivates the HUD.
     */
    void activate();

    /**
    * Wrapper class for userData to prevent cyclic destructor calls
    */
    class userData: public osg::Referenced
    {
        friend class WMHud;
    public:
        /**
        * userData Constructur with shared pointer to module
        * \param _parent pointer to the module 
        */
        explicit userData( boost::shared_ptr< WMHud > _parent )
        {
            m_parent = _parent;
        }

        /**
        * update wrapper Function
        */
        void update();

    private:
        /**
        * shared pointer to the module
        */
        boost::shared_ptr< WMHud > m_parent;
    };

    /**
     * Node callback to handle updates properly
     */
    class HUDNodeCallback : public osg::NodeCallback
    {
    public: // NOLINT
        /**
         * operator ()
         *
         * \param node the osg node
         * \param nv the node visitor
         */
        virtual void operator()( osg::Node* node, osg::NodeVisitor* nv )
        {
            osg::ref_ptr< userData > module = static_cast< userData* > ( node->getUserData() );

            if( module )
            {
                module->update();
            }
            traverse( node, nv );
        }
    };
};

#endif  // WMHUD_H
