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
#include <osg/LightModel>
#include <osg/Material>
#include <osg/PolygonMode>
#include <osg/StateAttribute>
#include <osg/StateSet>

#include "core/common/WAssert.h"
#include "core/dataHandler/WDataSetScalar.h"
#include "core/common/algorithms/WMarchingLegoAlgorithm.h"
#include "core/graphicsEngine/WROIArbitrary.h"
#include "core/graphicsEngine/WROIBox.h"
#include "core/kernel/WKernel.h"
#include "core/kernel/WROIManager.h"
#include "core/kernel/WSelectionManager.h"
#include "WMArbitraryROIs.h"
#include "WMArbitraryROIs.xpm"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMArbitraryROIs )

WMArbitraryROIs::WMArbitraryROIs():
    WModule(),
    m_textureChanged( true ),
    m_recompute( boost::shared_ptr< WCondition >( new WCondition() ) ),
    m_dataSet(),
    m_moduleNode( new WGEGroupNode() ),
    m_showSelector( true )
{
}

WMArbitraryROIs::~WMArbitraryROIs()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMArbitraryROIs::factory() const
{
    // See "src/modules/template/" for an extensively documented example.
    return boost::shared_ptr< WModule >( new WMArbitraryROIs() );
}

const char** WMArbitraryROIs::getXPMIcon() const
{
    return arbitraryROI_xpm;
}

const std::string WMArbitraryROIs::getName() const
{
    // Specify your module name here. This name must be UNIQUE!
    return "Arbitrary ROIs";
}

const std::string WMArbitraryROIs::getDescription() const
{
    return "Create non uniformly shaped ROIs for fiber selection. The ROI "
        "is what is enclosed by a surface created as isosurface from "
        "the data within a ROI box.";
}

void WMArbitraryROIs::connectors()
{
    // initialize connectors
    m_input = boost::shared_ptr< WModuleInputData < WDataSetScalar > >(
        new WModuleInputData< WDataSetScalar >( shared_from_this(), "in", "Dataset to cut ROI from." ) );
    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_input );

    // call WModules initialization
    WModule::connectors();
}

void WMArbitraryROIs::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    m_finalizeTrigger = m_properties->addProperty( "Finalize", "Finalize and add to ROI manager", WPVBaseTypes::PV_TRIGGER_READY, m_propCondition  );
    m_threshold = m_properties->addProperty( "Threshold", "", 1.0, m_propCondition );
    m_surfaceColor = m_properties->addProperty( "Surface color", "", WColor( 1.0, 0.3, 0.3, 1.0 ), m_propCondition );

    WModule::properties();
}

void WMArbitraryROIs::moduleMain()
{
    // use the m_input "data changed" flag
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_propCondition );
    m_moduleState.add( m_input->getDataChangedCondition() );
    m_moduleState.add( m_recompute );

    // signal ready state
    ready();

    // init the intial ROI position using the current crosshair position
    WPosition crossHairPos = WKernel::getRunningKernel()->getSelectionManager()->getCrosshair()->getPosition();
    m_lastMinPos = crossHairPos - WPosition( 10., 10., 10. );
    m_lastMaxPos = crossHairPos + WPosition( 10., 10., 10. );

    // loop until the module container requests the module to quit
    while( !m_shutdownFlag() )
    {
        m_moduleState.wait(); // waits for firing of m_moduleState ( dataChanged, shutdown, etc. )

        if( m_shutdownFlag() )
        {
            break;
        }

        if( m_input->getData() && m_dataSet != m_input->getData() )
        {
            // remove old ROI and ISO
            cleanup();

            // acquire data from the input connector
            m_dataSet = m_input->getData();

            m_threshold->setMin( m_dataSet->getMin() );
            m_threshold->setMax( m_dataSet->getMax() );
            m_threshold->set( ( m_dataSet->getMax() - m_dataSet->getMin() ) / 2.0 );

            initSelectionROI();
        }

        if( m_threshold->changed() )
        {
            m_threshold->get( true );
            m_showSelector = true;
            createCutDataset();
            renderMesh();
        }

        if( m_finalizeTrigger->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
        {
            m_showSelector = false;
            createCutDataset();
            renderMesh();
            finalizeROI();
            m_finalizeTrigger->set( WPVBaseTypes::PV_TRIGGER_READY, false );
        }
    }
    cleanup();
}

void WMArbitraryROIs::cleanup()
{
    // if we already had a ROI, we want to keep the position for the next ROI
    if( m_selectionROI )
    {
        m_lastMinPos = m_selectionROI->getMinPos();
        m_lastMaxPos = m_selectionROI->getMaxPos();
    }
    m_moduleNode->clear();
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_moduleNode );
    if( m_selectionROI )
    {
        WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_selectionROI );
    }
}

void WMArbitraryROIs::initSelectionROI()
{
    m_selectionROI = osg::ref_ptr< WROIBox >( new WROIBox( m_lastMinPos, m_lastMaxPos ) );
    m_selectionROI->setColor( osg::Vec4( 0., 0., 1.0, 0.4 ) );
    createCutDataset();
}

void WMArbitraryROIs::createCutDataset()
{
    if( !m_active->get() )
    {
        return;
    }

    boost::shared_ptr< WValueSetBase > newValueSet;

    boost::shared_ptr< WGridRegular3D > grid = boost::dynamic_pointer_cast< WGridRegular3D >( m_dataSet->getGrid() );

    size_t order = ( *m_dataSet ).getValueSet()->order();
    size_t vDim = ( *m_dataSet ).getValueSet()->dimension();

    float threshold = m_threshold->get();
    boost::shared_ptr< std::vector< float > > data = boost::shared_ptr< std::vector< float > >( new std::vector< float >() );

    switch( ( *m_dataSet ).getValueSet()->getDataType() )
    {
        case W_DT_UNSIGNED_CHAR:
        {
            boost::shared_ptr< WValueSet< unsigned char > > vals;
            vals =  boost::dynamic_pointer_cast< WValueSet< unsigned char > >( ( *m_dataSet ).getValueSet() );
            WAssert( vals, "Data type and data type indicator must fit." );
            data = cutArea( ( *m_dataSet ).getGrid(), vals );
            break;
        }
        case W_DT_INT16:
        {
            boost::shared_ptr< WValueSet< int16_t > > vals;
            vals =  boost::dynamic_pointer_cast< WValueSet< int16_t > >( ( *m_dataSet ).getValueSet() );
            WAssert( vals, "Data type and data type indicator must fit." );
            data = cutArea( ( *m_dataSet ).getGrid(), vals );
            break;
        }
        case W_DT_SIGNED_INT:
        {
            boost::shared_ptr< WValueSet< int32_t > > vals;
            vals =  boost::dynamic_pointer_cast< WValueSet< int32_t > >( ( *m_dataSet ).getValueSet() );
            WAssert( vals, "Data type and data type indicator must fit." );
            cutArea( ( *m_dataSet ).getGrid(), vals );
            data = cutArea( ( *m_dataSet ).getGrid(), vals );
            break;
        }
        case W_DT_FLOAT:
        {
            boost::shared_ptr< WValueSet< float > > vals;
            vals =  boost::dynamic_pointer_cast< WValueSet< float > >( ( *m_dataSet ).getValueSet() );
            WAssert( vals, "Data type and data type indicator must fit." );
            data = cutArea( ( *m_dataSet ).getGrid(), vals );
            break;
        }
        case W_DT_DOUBLE:
        {
            boost::shared_ptr< WValueSet< double > > vals;
            vals =  boost::dynamic_pointer_cast< WValueSet< double > >( ( *m_dataSet ).getValueSet() );
            WAssert( vals, "Data type and data type indicator must fit." );
            data = cutArea( ( *m_dataSet ).getGrid(), vals );
            break;
        }
        default:
            WAssert( false, "Unknown data type in ArbitraryROIs module" );
    }
    m_newValueSet = boost::shared_ptr< WValueSet< float > >( new WValueSet< float >( order, vDim, data, W_DT_FLOAT ) );
    WMarchingLegoAlgorithm mlAlgo;
    m_triMesh = mlAlgo.generateSurface( grid->getNbCoordsX(), grid->getNbCoordsY(), grid->getNbCoordsZ(),
                                        grid->getTransformationMatrix(),
                                        m_newValueSet->rawDataVectorPointer(),
                                        threshold );
}

template< typename T >
boost::shared_ptr< std::vector< float > > WMArbitraryROIs::cutArea( boost::shared_ptr< WGrid > inGrid, boost::shared_ptr< WValueSet< T > > vals )
{
    boost::shared_ptr< WGridRegular3D > grid = boost::dynamic_pointer_cast< WGridRegular3D >( inGrid );

    size_t nx = grid->getNbCoordsX();
    size_t ny = grid->getNbCoordsY();
    size_t nz = grid->getNbCoordsZ();

    double dx = grid->getOffsetX();
    double dy = grid->getOffsetY();
    double dz = grid->getOffsetZ();

    size_t xMin = static_cast<size_t>( m_selectionROI->getMinPos()[0] / dx );
    size_t yMin = static_cast<size_t>( m_selectionROI->getMinPos()[1] / dy );
    size_t zMin = static_cast<size_t>( m_selectionROI->getMinPos()[2] / dz );
    size_t xMax = static_cast<size_t>( m_selectionROI->getMaxPos()[0] / dx );
    size_t yMax = static_cast<size_t>( m_selectionROI->getMaxPos()[1] / dy );
    size_t zMax = static_cast<size_t>( m_selectionROI->getMaxPos()[2] / dz );

    boost::shared_ptr< std::vector< float > > newVals = boost::shared_ptr< std::vector< float > >( new std::vector< float >( nx * ny * nz, 0 ) );

    size_t x, y, z;
    for( z = 0; z < nz; ++z )
    {
        for( y = 0 ; y < ny; ++y )
        {
            for( x = 0 ; x < nx; ++x )
            {
                 if( ( x > xMin ) && ( x < xMax ) && ( y > yMin ) && ( y < yMax ) && ( z > zMin ) && ( z < zMax ) )
                 {
                     ( *newVals )[ x + nx * y + nx * ny * z ] = static_cast< float >( vals->getScalar( x + nx * y + nx * ny * z ) );
                 }
            }
        }
    }
    return newVals;
}

void WMArbitraryROIs::renderMesh()
{
    m_moduleNode->remove( m_outputGeode );

    if( m_showSelector )
    {
        osg::Geometry* surfaceGeometry = new osg::Geometry();
        m_outputGeode = osg::ref_ptr< osg::Geode >( new osg::Geode );

        m_outputGeode->setName( "ROI" );

        surfaceGeometry->setVertexArray( m_triMesh->getVertexArray() );

        // ------------------------------------------------
        // normals
        surfaceGeometry->setNormalArray( m_triMesh->getVertexNormalArray() );
        surfaceGeometry->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );

        // ------------------------------------------------
        // colors
        osg::Vec4Array* colors = new osg::Vec4Array;
        colors->push_back( osg::Vec4( 0.2f, 1.0f, 0.2f, 1.0f ) );
        surfaceGeometry->setColorArray( colors );
        surfaceGeometry->setColorBinding( osg::Geometry::BIND_OVERALL );

        osg::DrawElementsUInt* surfaceElement = new osg::DrawElementsUInt( osg::PrimitiveSet::TRIANGLES, 0 );

        std::vector< size_t >tris = m_triMesh->getTriangles();
        surfaceElement->reserve( tris.size() );

        for( unsigned int vertId = 0; vertId < tris.size(); ++vertId )
        {
            surfaceElement->push_back( tris[vertId] );
        }
        surfaceGeometry->addPrimitiveSet( surfaceElement );
        m_outputGeode->addDrawable( surfaceGeometry );

        osg::StateSet* state = m_outputGeode->getOrCreateStateSet();
        osg::ref_ptr<osg::LightModel> lightModel = new osg::LightModel();
        lightModel->setTwoSided( true );
        state->setAttributeAndModes( lightModel.get(), osg::StateAttribute::ON );
        state->setMode(  GL_BLEND, osg::StateAttribute::ON );

    //    {
    //        osg::ref_ptr< osg::Material > material = new osg::Material();
    //        material->setDiffuse(   osg::Material::FRONT, osg::Vec4( 1.0, 1.0, 1.0, 1.0 ) );
    //        material->setSpecular(  osg::Material::FRONT, osg::Vec4( 0.0, 0.0, 0.0, 1.0 ) );
    //        material->setAmbient(   osg::Material::FRONT, osg::Vec4( 0.1, 0.1, 0.1, 1.0 ) );
    //        material->setEmission(  osg::Material::FRONT, osg::Vec4( 0.0, 0.0, 0.0, 1.0 ) );
    //        material->setShininess( osg::Material::FRONT, 25.0 );
    //        state->setAttribute( material );
    //    }

        m_moduleNode->insert( m_outputGeode );
        WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_moduleNode );
    }
}

void WMArbitraryROIs::finalizeROI()
{
    if( !m_active->get() )
    {
        return;
    }

    boost::shared_ptr< WGridRegular3D > grid = boost::dynamic_pointer_cast< WGridRegular3D >( m_dataSet->getGrid() );
    osg::ref_ptr< WROI > newROI = osg::ref_ptr< WROI >( new WROIArbitrary(  grid->getNbCoordsX(), grid->getNbCoordsY(), grid->getNbCoordsZ(),
                                                                            grid->getTransformationMatrix(),
                                                                            *m_newValueSet->rawDataVectorPointer(),
                                                                            m_triMesh,
                                                                            m_threshold->get(),
                                                                            m_dataSet->getMax(), m_surfaceColor->get( true ) ) );

    if( WKernel::getRunningKernel()->getRoiManager()->getSelectedRoi() == NULL )
    {
        WKernel::getRunningKernel()->getRoiManager()->addRoi( newROI );
    }
    else
    {
        WKernel::getRunningKernel()->getRoiManager()->addRoi( newROI, WKernel::getRunningKernel()->getRoiManager()->getSelectedRoi() );
    }
}

void WMArbitraryROIs::activate()
{
    if( m_selectionROI )
    {
        if( m_active->get() )
        {
            m_selectionROI->setNodeMask( 0xFFFFFFFF );
        }
        else
        {
            m_selectionROI->setNodeMask( 0x0 );
        }
    }

    WModule::activate();
}
