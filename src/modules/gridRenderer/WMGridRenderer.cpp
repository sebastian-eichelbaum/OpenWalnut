//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2009 OpenWalnut Community, BSV-Leipzig and CNCF-CBS
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

#include <string>

#include "gridRenderer.xpm"
#include "../../kernel/WKernel.h"

#include "WMGridRenderer.h"

// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMGridRenderer )

WMGridRenderer::WMGridRenderer():
    WModule()
{
}

WMGridRenderer::~WMGridRenderer()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMGridRenderer::factory() const
{
    // See "src/modules/template/" for an extensively documented example.
    return boost::shared_ptr< WModule >( new WMGridRenderer() );
}

const char** WMGridRenderer::getXPMIcon() const
{
    return gridRenderer_xpm;
}

const std::string WMGridRenderer::getName() const
{
    return "Grid Renderer";
}

const std::string WMGridRenderer::getDescription() const
{
    return "Renders the grid structure of a data set.";
}

void WMGridRenderer::connectors()
{
    // initialize connectors
    m_input = boost::shared_ptr< WModuleInputData < WDataSetSingle > >(
        new WModuleInputData< WDataSetSingle >( shared_from_this(), "grid", "Dataset containing the grid" ) );
    addConnector( m_input );

    // call WModules initialization
    WModule::connectors();
}

void WMGridRenderer::properties()
{
    // Put the code for your properties here. See "src/modules/template/" for an extensively documented example.
}

void WMGridRenderer::moduleMain()
{
    m_moduleState.add( m_input->getDataChangedCondition() );

    ready();

    // loop until the module container requests the module to quit
    while ( !m_shutdownFlag() )
    {
        if ( !m_input->getData().get() )
        {
            // OK, the output has not yet sent data
            debugLog() << "Waiting for data ...";
            m_moduleState.wait();
            continue;
        }
        render();

        m_moduleState.wait();
    }
}

void WMGridRenderer::render()
{
     if( m_gridGeode )
     {
         m_moduleNode->remove( m_gridGeode );
     }

    boost::shared_ptr< WGrid > grid = m_input->getData()->getGrid();
    boost::shared_ptr< WGridRegular3D > gridR3D;
    gridR3D = boost::shared_dynamic_cast< WGridRegular3D >( grid );
    WAssert( gridR3D, "This module can only handle regular 3D grids so far." );

    //-------------------------------
    // DRAW
    m_moduleNode = new WGEGroupNode();
    osg::Geometry* gridGeometry = new osg::Geometry();
    m_gridGeode = osg::ref_ptr< osg::Geode >( new osg::Geode );

    m_gridGeode->setName( "grid" );

    osg::ref_ptr< osg::Vec3Array > vertArray = new osg::Vec3Array( gridR3D->size() );

    for( unsigned int vertId = 0; vertId < gridR3D->size(); ++vertId )
    {
        wmath::WPosition gridPos = gridR3D->getPosition( vertId );
        ( *vertArray )[vertId][0] = gridPos[0];
        ( *vertArray )[vertId][1] = gridPos[1];
        ( *vertArray )[vertId][2] = gridPos[2];
    }
    gridGeometry->setVertexArray( vertArray );

    osg::DrawElementsUInt* gridElement;

    gridElement = new osg::DrawElementsUInt( osg::PrimitiveSet::LINES, 0 );

    gridElement->reserve( gridR3D->size() * 2 );

    size_t sx = gridR3D->getNbCoordsX();
    size_t sy = gridR3D->getNbCoordsY();
    size_t sz = gridR3D->getNbCoordsZ();
    for( unsigned int vertIdX = 0; vertIdX < sx; ++vertIdX )
    {
        for( unsigned int vertIdY = 0; vertIdY < sy; ++vertIdY )
        {
            for( unsigned int vertIdZ = 0; vertIdZ < sz; ++vertIdZ )
            {
                size_t id = vertIdX + vertIdY * sx + vertIdZ * sx * sy;

                if( vertIdX < sx - 1 )
                {
                    gridElement->push_back( id );
                    gridElement->push_back( id + 1 );
                }

                if( vertIdY < sy - 1 )
                {
                    gridElement->push_back( id );
                    gridElement->push_back( id + sx );
                }

                if( vertIdZ < sz - 1 )
                {
                    gridElement->push_back( id );
                    gridElement->push_back( id + sx * sy );
                }
            }
        }
    }

    gridGeometry->addPrimitiveSet( gridElement );

    m_gridGeode->addDrawable( gridGeometry );
    m_moduleNode->insert( m_gridGeode );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_moduleNode );

}

void WMGridRenderer::activate()
{
    if ( m_gridGeode )
    {
        if ( m_active->get() )
        {
            m_gridGeode->setNodeMask( 0xFFFFFFFF );
        }
        else
        {
            m_gridGeode->setNodeMask( 0x0 );
        }
    }
    WModule::activate();
}
