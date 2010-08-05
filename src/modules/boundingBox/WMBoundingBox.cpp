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

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>

#include <cmath>

#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osgText/Text>

#include "../../common/WAssert.h"
#include "../../common/WStringUtils.h"
#include "../../dataHandler/WGridRegular3D.h"
#include "../../graphicsEngine/WGEGeodeUtils.h"
#include "../../kernel/WKernel.h"
#include "../../common/math/WPosition.h"
#include "../../common/math/WVector3D.h"
#include "../../kernel/modules/data/WMData.h"
#include "WMBoundingBox.h"
#include "boundingbox.xpm"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMBoundingBox )

WMBoundingBox::WMBoundingBox():
    WModule()
{
    // WARNING: initializing connectors inside the constructor will lead to an exception.
    // Implement WModule::initializeConnectors instead.
}

WMBoundingBox::~WMBoundingBox()
{
    // cleanup
    removeConnectors();
}

boost::shared_ptr< WModule > WMBoundingBox::factory() const
{
    return boost::shared_ptr< WModule >( new WMBoundingBox() );
}

const char** WMBoundingBox::getXPMIcon() const
{
    return boundingbox_xpm;
}

const std::string WMBoundingBox::getName() const
{
    return "Bounding Box";
}

const std::string WMBoundingBox::getDescription() const
{
    return "Shows the bounding box of a data set.";
}

void WMBoundingBox::moduleMain()
{
    // use the m_input "data changed" flag
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );

    // signal ready state
    ready();

    // loop until the module container requests the module to quit
    while ( !m_shutdownFlag() )
    {
        bool dataUpdated = m_input->updated();
        boost::shared_ptr< WDataSetSingle > dataSet = m_input->getData();
        bool dataValid = ( dataSet );

        if ( !dataValid )
        {
            // OK, the output has not yet sent data
            // NOTE: see comment at the end of this while loop for m_moduleState
            debugLog() << "Waiting for data ...";
            m_moduleState.wait();
            continue;
        }

        if ( dataUpdated )
        {
            createGFX();
        }

        // this waits for m_moduleState to fire. By default, this is only the m_shutdownFlag condition.
        // NOTE: you can add your own conditions to m_moduleState using m_moduleState.add( ... )
        m_moduleState.wait();
    }

    WGraphicsEngine::getGraphicsEngine()->getScene()->remove( m_bBoxNode );
}

void WMBoundingBox::createGFX()
{
    boost::shared_ptr< WDataSetSingle > dataSet = m_input->getData();

    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( dataSet->getGrid() );

    WAssert( grid, "Seems that grid is of wrong type." );

    WGraphicsEngine::getGraphicsEngine()->getScene()->remove( m_bBoxNode );

    std::pair< wmath::WPosition, wmath::WPosition > bb = grid->getBoundingBox();

    m_bBoxNode = osg::ref_ptr< WGEGroupNode >( new WGEGroupNode );
    m_bBoxNode->setNodeMask( m_active->get() ? 0xFFFFFFFF : 0x0 );

    m_bBoxNode->insert( wge::generateBoundingBoxGeode( bb.first, bb.second, WColor( 0.3, 0.3, 0.3, 1 ) ) );

    wmath::WVector3D pos1 = bb.first;
    wmath::WVector3D pos2 = bb.second;
    m_bBoxNode->addChild( wge::vector2label( osg::Vec3( pos1[0], pos1[1], pos1[2] ) ) );
    m_bBoxNode->addChild( wge::vector2label( osg::Vec3( pos2[0], pos2[1], pos2[2] ) ) );
    m_bBoxNode->addChild( wge::vector2label( osg::Vec3( pos2[0], pos2[1], pos1[2] ) ) );
    m_bBoxNode->addChild( wge::vector2label( osg::Vec3( pos1[0], pos2[1], pos1[2] ) ) );
    m_bBoxNode->addChild( wge::vector2label( osg::Vec3( pos2[0], pos1[1], pos1[2] ) ) );
    m_bBoxNode->addChild( wge::vector2label( osg::Vec3( pos1[0], pos2[1], pos2[2] ) ) );
    m_bBoxNode->addChild( wge::vector2label( osg::Vec3( pos2[0], pos1[1], pos2[2] ) ) );
    m_bBoxNode->addChild( wge::vector2label( osg::Vec3( pos1[0], pos1[1], pos2[2] ) ) );


    WGraphicsEngine::getGraphicsEngine()->getScene()->insert( m_bBoxNode );
}

void WMBoundingBox::connectors()
{
    // initialize connectors
    m_input = boost::shared_ptr< WModuleInputData < WDataSetSingle  > >(
        new WModuleInputData< WDataSetSingle >( shared_from_this(),
                                                               "in", "The dataset to filter" )
        );

    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_input );


    // call WModules initialization
    WModule::connectors();
}

void WMBoundingBox::properties()
{
//    ( m_properties->addInt( "Thickness", 1 ) )->connect( boost::bind( &WMBoundingBox::slotPropertyChanged, this, _1 ) );

    // m_active gets initialized in WModule and is available for all modules. Overwrite activate() to have a special callback for m_active
    // changes.
}

void WMBoundingBox::activate()
{
    if ( m_bBoxNode )
    {
        if ( m_active->get() )
        {
            m_bBoxNode->setNodeMask( 0xFFFFFFFF );
        }
        else
        {
            m_bBoxNode->setNodeMask( 0x0 );
        }
    }
    WModule::activate();
}
