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

#ifndef WMFIBERDISPLAY_H
#define WMFIBERDISPLAY_H

#include <string>

#include <osg/Geode>

#include "../../dataHandler/WDataSetFibers.h"
#include "../../graphicsEngine/WGEGroupNode.h"
#include "../../kernel/WModule.h"
#include "../../kernel/WModuleInputData.h"
#include "../../math/WFiber.h"

/**
 * Test module for drawing fibers
 * \ingroup modules
 */
class WMFiberDisplay : public WModule
{
friend class WMFiberDisplayTest;
public:
    /**
     * Constructs new FiberTestModule
     */
    WMFiberDisplay();

    /**
     * Destructs this FiberTestModule
     */
    virtual ~WMFiberDisplay();

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
     * Due to the prototype design pattern used to build modules, this method
     * returns a new instance of this method. NOTE: it should never be
     * initialized or modified in some other way. A simple new instance is
     * required.
     *
     * \return the prototype used to create every module in OpenWalnut.
     */
    virtual boost::shared_ptr< WModule > factory() const;

    /**
     * Determine what to do if a property was changed.
     * \param propertyName Name of the property.
     */
    void slotPropertyChanged( std::string propertyName );

protected:
    /**
     * Entry point after loading the module. Runs in separate thread.
     */
    virtual void moduleMain();

    /**
     * Generates an OSG geometry for the given fiber dataset.
     *
     * \param fibers pointer to fiber data set.
     * \param globalColoring determines whether the whole fiber has
     * the same color (true) or separate segements can have different colors.
     * \return OSG geometry representing the fiber.
     */
    osg::ref_ptr< osg::Geode > genFiberGeode(
            boost::shared_ptr< const WDataSetFibers > fibers,
            bool globalColoring = true ) const;

    /**
     * Initialize the connectors this module is using.
     */
    virtual void connectors();

    /**
     * Initialize the properties for this module.
     */
    virtual void properties();

    /**
     * Redraws the scene.
     *
     * \problem This might take a while with e.g. 70,000 fibers approx 4 sec
     */
    void update();

    /**
     * If the fibers have to be drawn in global coloring mode this is true, otherwise false.
     */
    bool m_globalColoring;

private:
    /**
     * Input connector for a fiber dataset.
     */
    boost::shared_ptr< WModuleInputData< const WDataSetFibers > > m_fiberInput;

    /**
     * Pointer to the fiber data set
     */
    boost::shared_ptr< const WDataSetFibers > m_dataset;

    /**
     * OSG node for this module. All other OSG nodes of this module should be
     * placed as child to this node.
     */
    osg::ref_ptr< WGEGroupNode > m_osgNode;
};

/**
 * Some helper functions for displaying fibers
 */
namespace display_utils
{
    /**
     * Transforms a direction given via two points into a RGB color.
     *
     * \param pos1 First point
     * \param pos2 Second point
     */
     WColor getRGBAColorFromDirection( const wmath::WPosition &pos1, const wmath::WPosition &pos2 );
}

inline const std::string WMFiberDisplay::getName() const
{
    return std::string( "Fiber Display Module" );
}

inline const std::string WMFiberDisplay::getDescription() const
{
    return std::string( "Draws fibers out of a WDataSetFibers" );
}

inline WColor display_utils::getRGBAColorFromDirection( const wmath::WPosition &pos1, const wmath::WPosition &pos2 )
{
    wmath::WPosition direction( ( pos2 - pos1 ) );
    direction.normalize();
    return WColor( std::abs( direction[0] ), std::abs( direction[1] ), std::abs( direction[2] ) );
}

#endif  // WMFIBERDISPLAY_H
