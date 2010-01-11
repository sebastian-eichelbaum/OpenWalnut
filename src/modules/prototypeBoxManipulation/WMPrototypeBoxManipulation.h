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

#ifndef WMPROTOTYPEBOXMANIPULATION_H
#define WMPROTOTYPEBOXMANIPULATION_H

#include <map>
#include <string>
#include <vector>

#include <osg/Node>
#include <osg/Geode>
#include <osg/Uniform>

#include "../../kernel/WModule.h"
#include "../../kernel/WModuleInputData.h"

#include "../../math/WVector3D.h"

class WPickHandler;

/**
 * Prototype module
 */
class WMPrototypeBoxManipulation : public WModule
{
public:
    /**
     * Standard constructor.
     */
    WMPrototypeBoxManipulation();

    /**
     * Destructor.
     */
    ~WMPrototypeBoxManipulation();

    /**
     * Gives back the name of this module.
     * \return the module's name.
     */
    virtual const std::string getName() const;

    /**
     * Gives back a description of this module.
     * \return description of module.
     */
    virtual const std::string getDescription() const;

    /**
     * Determine what to do if a property was changed.
     * \param propertyName Name of the property.
     */
    void slotPropertyChanged( std::string propertyName );

    /**
     * Due to the prototype design pattern used to build modules, this method returns a new instance of this method. NOTE: it
     * should never be initialized or modified in some other way. A simple new instance is required.
     *
     * \return the prototype used to create every module in OpenWalnut.
     */
    virtual boost::shared_ptr< WModule > factory() const;

    /**
     *  updates the graphics
     * \param text text info from pick
     */
    void updateGFX( std::string text );

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

    /**
     * draw initial graphics
     * \param minPos the lower left front corener of the box
     * \param maxPos the upper right back corner of the box
     */
    void draw( wmath::WPosition minPos, wmath::WPosition maxPos );

    boost::shared_mutex m_updateLock; //!< Lock to prevent concurrent threads trying to update the osg node

    osg::Geode* m_geode; //!< Pointer to geode. We need it to be able to update it when callback is invoked.

    boost::shared_ptr< WModuleInputData< WDataSetSingle > > m_input;  //!< Input connector required by this module.

    bool m_isPicked; //!< Indicates whether the box is currently picked or not.
    wmath::WPosition m_pickedPosition; //!< Caches the old picked position to a allow for cmoparison
    wmath::WPosition m_minPos; //!< The minimum position of the box
    wmath::WPosition m_maxPos; //!< The maximum position of the box

    boost::shared_ptr< WPickHandler > m_pickHandler; //!< A pointer to the pick handler used to get gui events for moving the box.
};

/**
 * Adapter object for realizing callbacks of the node representing the box in the osg
 */
class BoxNodeCallback : public osg::NodeCallback
{
public:
    /**
     * Constructor of the callback adapter.
     * \param module A function of this module will be called
     */
    explicit BoxNodeCallback( boost::shared_ptr< WMPrototypeBoxManipulation > module );

    /**
     * Function that is called by the osg and that call the function in the module.
     * \param node The node we are called.
     * \param nv the visitor calling us.
     */
    virtual void operator()( osg::Node* node, osg::NodeVisitor* nv );

private:
    boost::shared_ptr< WMPrototypeBoxManipulation > m_module; //!< Pointer to the module to which the function that is called belongs to.
};

inline BoxNodeCallback::BoxNodeCallback( boost::shared_ptr< WMPrototypeBoxManipulation > module )
    : m_module( module )
{
}

inline void BoxNodeCallback::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    if ( m_module )
    {
        m_module->updateGFX( "..." );
    }
    traverse( node, nv );
}

#endif  // WMPROTOTYPEBOXMANIPULATION_H
