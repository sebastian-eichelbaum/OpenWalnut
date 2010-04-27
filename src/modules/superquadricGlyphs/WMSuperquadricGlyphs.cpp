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
#include <vector>

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Material>
#include <osg/StateSet>
#include <osg/StateAttribute>
#include <osg/PolygonMode>
#include <osg/LightModel>

#include "../../kernel/WKernel.h"

#include "../../common/math/WMatrix.h"
#include "../../common/math/WVector3D.h"

#include "WMSuperquadricGlyphs.h"

WMSuperquadricGlyphs::WMSuperquadricGlyphs():
    WModule()
{
}

WMSuperquadricGlyphs::~WMSuperquadricGlyphs()
{
    // Cleanup!
    removeConnectors();
}

boost::shared_ptr< WModule > WMSuperquadricGlyphs::factory() const
{
    // create prototype
    return boost::shared_ptr< WModule >( new WMSuperquadricGlyphs() );
}

const std::string WMSuperquadricGlyphs::getName() const
{
    return "SuperquadricGlyphs";
}

const std::string WMSuperquadricGlyphs::getDescription() const
{
    return "GPU based raytracing of second order, superquadric tensor glyphs.";
}

void WMSuperquadricGlyphs::connectors()
{
    // The input second order tensor dataset
    m_input = boost::shared_ptr< WModuleInputData< WDataSetSingle > >( new WModuleInputData< WDataSetSingle >( shared_from_this(),
        "tensor input", "An input set of 2nd-order tensors on a regular 3D-grid." )
    );
    addConnector( m_input );

    // call WModule's initialization
    WModule::connectors();
}

void WMSuperquadricGlyphs::properties()
{
    // The condition fires on property updates
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    // The slice positions. These get update externally
    m_xPos           = m_properties->addProperty( "X Pos of the slice", "Slice X position.", 80, m_propCondition);
    m_yPos           = m_properties->addProperty( "Y Pos of the slice", "Slice Y position.", 100, m_propCondition);
    m_zPos           = m_properties->addProperty( "Z Pos of the slice", "Slice Z position.", 80, m_propCondition);
    m_xPos->setHidden( true );
    m_yPos->setHidden( true );
    m_zPos->setHidden( true );
    m_xPos->setMin( 0 );
    m_xPos->setMax( 160 );
    m_yPos->setMin( 0 );
    m_yPos->setMax( 200 );
    m_zPos->setMin( 0 );
    m_zPos->setMax( 160 );

    // Flags denoting whether the glyphs should be shown on the specific slice
    m_showonX        = m_properties->addProperty( "Show Sagittal", "Show vectors on sagittal slice.", true, m_propCondition);
    m_showonY        = m_properties->addProperty( "Show Coronal", "Show vectors on coronal slice.", true, m_propCondition );
    m_showonZ        = m_properties->addProperty( "Show Axial", "Show vectors on axial slice.", true, m_propCondition);
}

void WMSuperquadricGlyphs::moduleMain()
{
    // use the m_input "data changed" flag
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );
    // use property condition to wake up on property changes
    m_moduleState.add( m_propCondition );

    // signal ready state
    ready();

    // loop until the module container requests the module to quit
    while ( !m_shutdownFlag() )
    {
        debugLog() << "Waiting ...";
        m_moduleState.wait();

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // After waking up, the module has to check whether the shutdownFlag fired. If yes, simply quit the module.

        // woke up since the module is requested to finish
        if ( m_shutdownFlag() )
        {
            break;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Collect changes

        // has the data changes? And even more important: is it valid?
        boost::shared_ptr< WDataSetSingle > newDataSet = m_input->getData();
        bool dataChanged = ( m_dataSet != newDataSet );
        bool dataValid   = ( newDataSet );
        // TODO(ebaum): check order here

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Handle changes

        // data changes
        if ( dataChanged && dataValid )
        {
            // The data is different. Copy it to our internal data variable:
            debugLog() << "Received Data.";
            m_dataSet = newDataSet;

            // create a new geode containing the glyphs proxy geometry
            osg::ref_ptr< osg::Geode > newRootNode = new osg::Geode();

            // fill the geode with glyph proxy geometry
            osg::ref_ptr< osg::Geometry > geometry = new osg::Geometry();
            newRootNode->addDrawable( geometry );

            // as we use quads -> four verts per qlyph
            // NOTE: the vertices are only the position of the glyph. The shader actually spans them up according to the proper bbox
            osg::ref_ptr< osg::Vec3Array > vertices = new osg::Vec3Array;
            osg::ref_ptr< osg::DrawElementsUInt > glyphs = new osg::DrawElementsUInt( osg::PrimitiveSet::QUADS );

            // for each glyph, create the vertices and texture coordinates (used to transfer tensor values)
            vertices->push_back( osg::Vec3( 0, 0, 0) );
            glyphs->push_back( 0 );
            vertices->push_back( osg::Vec3(10, 0, 0) );
            glyphs->push_back( 1 );
            vertices->push_back( osg::Vec3(10,10, 0) );
            glyphs->push_back( 2 );
            vertices->push_back( osg::Vec3( 0,10, 0) );
            glyphs->push_back( 3 );

            geometry->setVertexArray( vertices );
            geometry->addPrimitiveSet( glyphs );




            // remove old node
            WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_outputGeode );
            m_outputGeode = newRootNode;

            // please also ensure that, according to m_active, the node is active or not. Setting it here allows the user to deactivate modules
            // in project files for example.
            m_outputGeode->setNodeMask( m_active->get() ? 0xFFFFFFFF : 0x0 );

            // set the update callback

            // add new node
            WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_outputGeode );
        }
    }

    // At this point, the container managing this module signalled to shutdown. The main loop has ended and you should clean up. Always remove
    // allocated memory and remove all OSG nodes.
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_outputGeode );
}

void WMSuperquadricGlyphs::activate()
{
    if ( m_outputGeode )   // always ensure the root node exists
    {
        if ( m_active->get() )
        {
            m_outputGeode->setNodeMask( 0xFFFFFFFF );
        }
        else
        {
            m_outputGeode->setNodeMask( 0x0 );
        }
    }

    // Always call WModule's activate!
    WModule::activate();
}

