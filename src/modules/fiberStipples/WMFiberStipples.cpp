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

#include <string>

#include <osg/Geometry>

#include "core/dataHandler/WDataSetFibers.h"
#include "core/dataHandler/WDataSetScalar.h"
#include "core/dataHandler/WDataSetVector.h"
#include "core/dataHandler/WGridRegular3D.h"
#include "core/graphicsEngine/WGEManagedGroupNode.h"
#include "core/kernel/WKernel.h"
#include "core/kernel/WModuleInputData.h"
#include "WMFiberStipples.h"
#include "WMFiberStipples.xpm"

// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMFiberStipples )

WMFiberStipples::WMFiberStipples()
    : WModule()
{
}

WMFiberStipples::~WMFiberStipples()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMFiberStipples::factory() const
{
    return boost::shared_ptr< WModule >( new WMFiberStipples() );
}

const char** WMFiberStipples::getXPMIcon() const
{
    return WMFiberStipples_xpm;
}

const std::string WMFiberStipples::getName() const
{
    return "Fiber Stipples";
}

const std::string WMFiberStipples::getDescription() const
{
    return "Slice based probabilistic tract display using Fiber Stipples. (see http://dx.doi.org/10.1109/BioVis.2011.6094044)";
}

void WMFiberStipples::connectors()
{
    m_vectorIC = WModuleInputData< WDataSetVector >::createAndAdd( shared_from_this(), "vectors", "Principal diffusion direction." );
    m_probIC = WModuleInputData< WDataSetScalar >::createAndAdd( shared_from_this(), "probTract", "Probabilistic tract." );

    // call WModule's initialization
    WModule::connectors();
}

void WMFiberStipples::properties()
{
//    m_sliceGroup     = m_properties->addPropertyGroup( "Slices",  "Slice based probabilistic tractogram display." );
//
//    m_slicePos[ 0 ] = m_sliceGroup->addProperty( "Sagittal Position", "Slice X position.", 0.0, m_sliceChanged );
//    m_slicePos[ 1 ] = m_sliceGroup->addProperty( "Coronal Position", "Slice Y position.", 0.0, m_sliceChanged );
//    m_slicePos[ 2 ] = m_sliceGroup->addProperty( "Axial Position", "Slice Z position.", 0.0, m_sliceChanged );
//
//    // since we don't know anything yet => make them unusable
//    for( size_t i = 0; i < 3; ++i )
//    {
//        m_slicePos[i]->setMax( 0 );
//        m_slicePos[i]->setMin( 0 );
//    }

//    boost::shared_ptr< WItemSelection > drawAlgorithmList( new WItemSelection() );
//    drawAlgorithmList->addItem( "With largest eigen vectors", "A WDataSetVectors is needed." );
//    drawAlgorithmList->addItem( "With deterministic tracts", "A WDataSetFibers is needed." );
//    m_drawAlgorithm = m_properties->addProperty( "Method:", "Method which you want to use for the visualization.",
//            drawAlgorithmList->getSelectorFirst(), m_sliceChanged );
//    WPropertyHelper::PC_SELECTONLYONE::addTo( m_drawAlgorithm );
//    WPropertyHelper::PC_NOTEMPTY::addTo( m_drawAlgorithm );
//
//    double hue_increment = 1.0 / NUM_ICS;
//    for( size_t i = 0; i < NUM_ICS; ++i )
//    {
//        std::stringstream ss;
//        ss << "Color for " << i << "InputConnector";
//        m_colorMap[i] = m_properties->addPropertyGroup( ss.str(), "String and color properties for an input connector" );
//        WPropString label = m_colorMap[i]->addProperty( "Filename", "The file name this group is connected with", std::string( "/no/such/file" ) );
//        label->setPurpose( PV_PURPOSE_INFORMATION );
//        WColor color = convertHSVtoRGBA( i * hue_increment, 1.0, 0.75 );
//        m_colorMap[i]->addProperty( "Color", "The color for the probabilistic tractogram this group is associated with", color, m_colorChanged );
//        m_colorMap[i]->setHidden(); // per default for each unconnected input the property group is hidden
//    }

//    // properties only relevant if the method is: "With deterministic tracts" was selected
//    m_tractGroup     = m_properties->addPropertyGroup( "Tract Group", "Parameters for drawing via deterministic tracts." );
//    m_probThreshold    = m_tractGroup->addProperty( "Prob Threshold", "Only vertices with probabil. greater this contribute.", 0.1, m_sliceChanged );
//    m_probThreshold->setMin( 0.0 );
//    m_probThreshold->setMax( 1.0 );
//    m_showIntersection = m_tractGroup->addProperty( "Show Intersections", "Show intersecition stipplets", false );
//    m_showProjection   = m_tractGroup->addProperty( "Show Projections", "Show projection stipplets", true );
//    m_delta            = m_tractGroup->addProperty( "Slices Environment", "Cut off the tracts after this distance.", 1.0, m_sliceChanged );
//
//    m_vectorGroup     = m_properties->addPropertyGroup( "Vector Group", "Parameters for drawing via eigen vectors." );
//    m_vectorGroup->addProperty( m_probThreshold ); // this is also needed in this property group
//    WPropDouble spacing = m_vectorGroup->addProperty( "Spacing", "Spacing of the sprites", 1.0, m_sliceChanged );
//    spacing->setMin( 0.25 );
//    spacing->setMax( 5.0 );
////    WPropDouble glyphSize = m_vectorGroup->addProperty( "Glyph size", "Size of the quads transformed to the glyphs", 1.0 );
////    glyphSize->setMin( 0.25 );
////    glyphSize->setMax( 5.0 );
////    WPropDouble glyphSpacing = m_vectorGroup->addProperty( "Glyph Spacing", "Spacing ", 0.4, m_sliceChanged );
////    glyphSpacing->setMin( 0.0 );
////    glyphSpacing->setMax( 5.0 );
//    WPropDouble glyphThickness = m_vectorGroup->addProperty( "Glyph Thickness", "Line thickness of the glyphs", 1.0 );
//    glyphThickness->setMin( 0.01 );
//    glyphThickness->setMax( 2.0 );
//
//    WPropBool showGrid = m_vectorGroup->addProperty( "Show Grid", "Shows the grid", false );

    // call WModule's initialization
    WModule::properties();
}

osg::ref_ptr< WGEManagedGroupNode > WMFiberStipples::initOSG()
{
    debugLog() << "Init OSG";
    osg::ref_ptr< WGEManagedGroupNode > rootNode( new WGEManagedGroupNode( m_active ) );
    rootNode->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( rootNode );

    return rootNode;
}

void WMFiberStipples::moduleMain()
{
    // get notified about data changes
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_probIC->getDataChangedCondition() );
    m_moduleState.add( m_vectorIC->getDataChangedCondition() );

    ready();

    m_output = initOSG();

    // main loop
    while( !m_shutdownFlag() )
    {
        infoLog() << "Waiting ...";
        m_moduleState.wait();

        // woke up since the module is requested to finish?
        if( m_shutdownFlag() )
        {
            break;
        }

//        bool dataUpdated = m_vectorIC->handledUpdate() || m_tractsIC->handledUpdate(); // this call must be made befor getData() on the ICs
//        bool probDataValid = false;
//
//        boost::shared_ptr< WDataSetVector > vectors = m_vectorIC->getData();
//        boost::shared_ptr< WDataSetFibers > detTracts = m_tractsIC->getData();
//        boost::shared_ptr< WGridRegular3D > grid;
//        probTracts.clear(); // discard all prob tracts so far
//
//        for( size_t i = 0; i < NUM_ICS; ++i )
//        {
//            dataUpdated = dataUpdated || m_probICs[i]->handledUpdate();
//            probDataValid = probDataValid || m_probICs[i]->getData(); // at least one probabilistic tract must be there
//            if( m_probICs[i]->getData() )
//            {
//                probTracts.push_back( m_probICs[i]->getData() );
//            }
//        }
//        bool dataValid = ( detTracts || vectors ) && probDataValid;
//
//        if( dataValid )
//        {
//            grid = boost::shared_dynamic_cast< WGridRegular3D >( probTracts.front()->getGrid() ); // assume all grids are the same and get the first
//            if( !grid )
//            {
//                errorLog() << "A grid beside WGridRegular3D was used, aborting...";
//                continue;
//            }
//        }
//        else
//        {
//            m_output->clear();
//            builder.reset();
//            continue;
//        }
//
//        if( dataUpdated || m_drawAlgorithm->changed() )
//        {
//            infoLog() << "Handling data update..";
//            checkProbabilityRanges( probTracts );
//
//            if( grid->getNbCoordsX() > 0 && grid->getNbCoordsY() > 0 &&  grid->getNbCoordsZ() > 0 )
//            {
//                resetSlicePos( grid );
//            }
//            else
//            {
//                warnLog() << "Invalid grid: at least one dimension is not greater zero.";
//                continue;
//            }
//
//            WItemSelector algo = m_drawAlgorithm->get( true );
//            if( algo.at( 0 )->getName() == std::string( "With largest eigen vectors" ) )
//            {
//                m_vectorGroup->setHidden( false );
//                m_tractGroup->setHidden( true );
//                if( !vectors )
//                {
//                    errorLog() << "No vector dataset available, but selected method needs one. Please connect one!";
//                    continue;
//                }
//                else
//                {
//                    builder = boost::shared_ptr< WSPSliceBuilderVectors >( new WSPSliceBuilderVectors( probTracts, m_sliceGroup, m_colorMap,
//                                vectors, m_vectorGroup, m_localPath ) );
//                }
//            }
//            else if( algo.at( 0 )->getName() == std::string( "With deterministic tracts" ) )
//            {
//                m_vectorGroup->setHidden( true );
//                m_tractGroup->setHidden( false );
//                if( !detTracts )
//                {
//                    errorLog() << "No deterministic tracts on the connector present, but selected method needs one. Please connect one!";
//                    continue;
//                }
//                else
//                {
//                    builder = boost::shared_ptr< WSPSliceBuilderTracts >( new WSPSliceBuilderTracts( probTracts, m_sliceGroup, m_colorMap, detTracts,
//                                m_tractGroup ) );
//                }
//            }
//            else
//            {
//                throw WNotImplemented( "Invalid method: Either \"With largest eigen vectors\" or \"With deterministic tracts\" expected." );
//            }
//        }
//
//        // NOTE: we know that we could arrange much more specific updates here, just update the slice that has changed, but this is too complex to
//        // consider also color, m_delta, etc. changes..., and ofcourse: we have the time to calc that every loop-cycle again and again, it does
//        // not really matter
//        debugLog() << "Building the geodes...";
//        updateSlices( builder );
    }

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_output );
}
