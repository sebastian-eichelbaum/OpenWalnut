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
#include <vector>

#include <osg/Geometry>

#include "../../common/exceptions/WNotImplemented.h"
#include "../../common/exceptions/WOutOfBounds.h"
#include "../../dataHandler/WDataSetFibers.h"
#include "../../dataHandler/WDataSetScalar.h"
#include "../../dataHandler/WDataSetVector.h"
#include "../../dataHandler/WGridRegular3D.h"
#include "../../graphicsEngine/WGEManagedGroupNode.h"
#include "../../kernel/WKernel.h"
#include "../../kernel/WModuleConnectorSignals.h"
#include "../../kernel/WModuleInputData.h"
#include "WMProbTractDisplaySP.h"
#include "WMProbTractDisplaySP.xpm"
#include "WSPSliceBuilder.h"
#include "WSPSliceBuilderTracts.h"
#include "WSPSliceBuilderVectors.h"

// This line is needed by the module loader to actually find your module. You need to add this to your module too. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMProbTractDisplaySP )

namespace
{
    const size_t NUM_ICS = 9;
}

WMProbTractDisplaySP::WMProbTractDisplaySP()
    : WModule(),
      m_probICs( NUM_ICS ),
      m_colorMap( NUM_ICS ),
      m_colorChanged( new WCondition ),
      m_sliceChanged( new WCondition )
{
}

WMProbTractDisplaySP::~WMProbTractDisplaySP()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMProbTractDisplaySP::factory() const
{
    return boost::shared_ptr< WModule >( new WMProbTractDisplaySP() );
}

const char** WMProbTractDisplaySP::getXPMIcon() const
{
    return WMProbTractDisplaySP_xpm;
}

const std::string WMProbTractDisplaySP::getName() const
{
    return "Prob Tract Display SP";
}

const std::string WMProbTractDisplaySP::getDescription() const
{
    return "Slice based probabilistic tract display based on Schmahmann y Pandya";
}

void WMProbTractDisplaySP::updateProperitesForTheInputConnectors( boost::shared_ptr< WModuleConnector > /* receiver */,
        boost::shared_ptr< WModuleConnector > /* sender */ )
{
    debugLog() << "Input Connector was connected or closed => so all ICs and corresponding PropertyGroups will be checked....";
    // iterate overall connectors and if one is left with an hidden property unhide it or do the opposite
    for( size_t i = 0; i < NUM_ICS; ++i )
    {
        if( ( m_probICs[i]->isConnected() > 0 ) && ( m_colorMap[i]->isHidden() ) )
        {
            debugLog() << "Found a connected IC, but hidden prop group at index: " << i;
            m_colorMap[i]->setHidden( false );
            if( m_probICs[i]->getData( false ) )
            {
                 m_colorMap[i]->findProperty( "Filename" )->toPropString()->set( m_probICs[i]->getData( false )->getFileName() );
            }
        }
        if( ( m_probICs[i]->isConnected() == 0 ) && ( !m_colorMap[i]->isHidden() ) )
        {
            debugLog() << "Found an unconnected IC, but unhidden prop group at index: " << i;
            m_colorMap[i]->setHidden();
            m_colorMap[i]->findProperty( "Filename" )->toPropString()->set( "/no/such/file" );
        }
    }
}

void WMProbTractDisplaySP::connectors()
{
    m_tractsIC = WModuleInputData< WDataSetFibers >::createAndAdd( shared_from_this(), "tractsInput", "The deterministic tractograms." );

    m_vectorIC = WModuleInputData< WDataSetVector >::createAndAdd( shared_from_this(), "vectorInput", "The largest eigen vectors of the tensors." );

    for( size_t i = 0; i < NUM_ICS; ++i )
    {
        std::stringstream ss;
        ss << "probTract" << i << "Input";
        m_probICs[i] = WModuleInputData< WDataSetScalar >::createAndAdd( shared_from_this(), ss.str(), "A probabilistic tractogram" );
        m_probICs[i]->subscribeSignal( CONNECTION_ESTABLISHED,
                boost::bind( &WMProbTractDisplaySP::updateProperitesForTheInputConnectors, this, _1, _2 ) );
        m_probICs[i]->subscribeSignal( CONNECTION_CLOSED,
                boost::bind( &WMProbTractDisplaySP::updateProperitesForTheInputConnectors, this, _1, _2 ) );
    }

    // call WModule's initialization
    WModule::connectors();
}

void WMProbTractDisplaySP::properties()
{
    m_sliceGroup      = m_properties->addPropertyGroup( "Slices",  "Slice based probabilistic tractogram display." );

    m_showonX        = m_sliceGroup->addProperty( "Show Sagittal", "Show instersection of deterministic tracts on sagittal slice.", true );
    m_showonY        = m_sliceGroup->addProperty( "Show Coronal", "Show instersection of deterministic tracts on coronal slice.", true );
    m_showonZ        = m_sliceGroup->addProperty( "Show Axial", "Show instersection of deterministic tracts on axial slice.", true );
    // The slice positions. These get update externally.
    // TODO(math): get the dimensions and MinMax's directly from the probabilistic tractorgram
    // TODO(all): this should somehow be connected to the nav slices.
    m_xPos           = m_sliceGroup->addProperty( "Sagittal Position", "Slice X position.", 0, m_sliceChanged );
    m_yPos           = m_sliceGroup->addProperty( "Coronal Position", "Slice Y position.", 0, m_sliceChanged );
    m_zPos           = m_sliceGroup->addProperty( "Axial Position", "Slice Z position.", 0, m_sliceChanged );

    // since we don't know anything yet => make them unusable
    m_xPos->setMin( 0 );
    m_xPos->setMax( 0 );
    m_yPos->setMin( 0 );
    m_yPos->setMax( 0 );
    m_zPos->setMin( 0 );
    m_zPos->setMax( 0 );

    m_drawAlgorithmList = boost::shared_ptr< WItemSelection >( new WItemSelection() );
    m_drawAlgorithmList->addItem( "With deterministic tracts", "A WDataSetFibers is needed." );
    m_drawAlgorithmList->addItem( "With largest eigen vectors", "A WDataSetVectors is needed." );
    m_drawAlgorithm = m_properties->addProperty( "Method:", "Method which you want to use for the visualization.",
            m_drawAlgorithmList->getSelectorFirst(), m_sliceChanged );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_drawAlgorithm );
    WPropertyHelper::PC_NOTEMPTY::addTo( m_drawAlgorithm );

    double hue_increment = 1.0 / NUM_ICS;
    for( size_t i = 0; i < NUM_ICS; ++i )
    {
        std::stringstream ss;
        ss << "Color for " << i << "InputConnector";
        m_colorMap[i] = m_properties->addPropertyGroup( ss.str(), "String and color properties for an input connector" );
        WPropString label = m_colorMap[i]->addProperty( "Filename", "The file name this group is connected with", std::string( "/no/such/file" ) );
        label->setPurpose( PV_PURPOSE_INFORMATION );
        WColor color = convertHSVtoRGBA( i * hue_increment, 1.0, 0.75 );
        m_colorMap[i]->addProperty( "Color", "The color for the probabilistic tractogram this group is associated with", color, m_colorChanged );
        m_colorMap[i]->setHidden(); // per default for each unconnected input the property group is hidden
    }

    // properties only relevant if the method is: "With deterministic tracts" was selected
    m_tractGroup     = m_properties->addPropertyGroup( "Tract Group", "Parameters for drawing via deterministic tracts." );
    m_probThreshold    = m_tractGroup->addProperty( "Prob Threshold", "Only vertices with probabil. greater this contribute.", 0.1, m_sliceChanged );
    m_probThreshold->setMin( 0.0 );
    m_probThreshold->setMax( 1.0 );
    m_showIntersection = m_tractGroup->addProperty( "Show Intersections", "Show intersecition stipplets", false );
    m_showProjection   = m_tractGroup->addProperty( "Show Projections", "Show projection stipplets", true );
    m_delta            = m_tractGroup->addProperty( "Slices Environment", "Cut off the tracts after this distance.", 1.0, m_sliceChanged );

    m_vectorGroup     = m_properties->addPropertyGroup( "Vector Group", "Parameters for drawing via eigen vectors." );
    m_vectorGroup->addProperty( m_probThreshold ); // this is also needed in this property group
    WPropDouble spacing = m_vectorGroup->addProperty( "Spacing", "Spacing of the sprites", 0.5, m_sliceChanged );
    spacing->setMin( 0.01 );
    spacing->setMax( 10 );
    WPropDouble glyphSize = m_vectorGroup->addProperty( "Glyph size", "Size of the quads transformed to the glyphs", 0.5 );
    glyphSize->setMin( 0.01 );
    glyphSize->setMax( 3.0 );
    WPropDouble glyphSpacing = m_vectorGroup->addProperty( "Glyph Spacing", "Spacing ", 0.4, m_sliceChanged );
    glyphSpacing->setMin( 0.0 );
    glyphSpacing->setMax( 5.0 );
    WPropDouble glyphThickness = m_vectorGroup->addProperty( "Glyph Thickness", "Line thickness of the glyphs", 0.2 );
    glyphThickness->setMin( 0.01 );
    glyphThickness->setMax( 1.0 );

    // call WModule's initialization
    WModule::properties();
}

void WMProbTractDisplaySP::updateProperties( boost::shared_ptr< const WGridRegular3D > grid )
{
    m_xPos->setMax( grid->getNbCoordsX() - 1 );
    m_xPos->set( ( grid->getNbCoordsX() - 1 ) / 2, true );
    m_yPos->setMax( grid->getNbCoordsY() - 1 );
    m_yPos->set( ( grid->getNbCoordsY() - 1 ) / 2, true );
    m_zPos->setMax( grid->getNbCoordsZ() - 1 );
    m_zPos->set( ( grid->getNbCoordsZ() - 1 ) / 2, true );
}

void WMProbTractDisplaySP::initOSG()
{
    // remove the old slices
    m_output->clear();

    // create all the transformation nodes
    m_xSlice = osg::ref_ptr< WGEManagedGroupNode > ( new WGEManagedGroupNode( m_showonX ) );
    m_ySlice = osg::ref_ptr< WGEManagedGroupNode > ( new WGEManagedGroupNode( m_showonY ) );
    m_zSlice = osg::ref_ptr< WGEManagedGroupNode > ( new WGEManagedGroupNode( m_showonZ ) );

    // add the transformation nodes to the output group
    m_output->insert( m_xSlice );
    m_output->insert( m_ySlice );
    m_output->insert( m_zSlice );
}

void WMProbTractDisplaySP::moduleMain()
{
    // get notified about data changes
    m_moduleState.setResetable( true, true );
    for( size_t i = 0; i < NUM_ICS; ++i )
    {
        m_moduleState.add( m_probICs[i]->getDataChangedCondition() );
    }
    m_moduleState.add( m_tractsIC->getDataChangedCondition() );
    m_moduleState.add( m_vectorIC->getDataChangedCondition() );
    m_moduleState.add( m_sliceChanged );
    m_moduleState.add( m_colorChanged );

    ready();

    // create the root node for all the geometry
    m_output = osg::ref_ptr< WGEManagedGroupNode > ( new WGEManagedGroupNode( m_active ) );
    m_output->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    debugLog() << "Init OSG";
    initOSG();
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_output );
    boost::shared_ptr< WSPSliceBuilder > builder;
    std::vector< boost::shared_ptr< const WDataSetScalar > > probTracts;

    // main loop
    while ( !m_shutdownFlag() )
    {
        infoLog() << "Waiting ...";
        m_moduleState.wait();

        // woke up since the module is requested to finish?
        if ( m_shutdownFlag() )
        {
            break;
        }

        bool dataUpdated = m_vectorIC->handledUpdate() || m_tractsIC->handledUpdate(); // this call must be made befor getData() on the ICs
        bool probDataValid = false;

        boost::shared_ptr< WDataSetVector > vectors = m_vectorIC->getData();
        boost::shared_ptr< WDataSetFibers > detTracts = m_tractsIC->getData();
        boost::shared_ptr< WGridRegular3D > grid;
        probTracts.clear(); // discard all prob tracts so far

        for( size_t i = 0; i < NUM_ICS; ++i )
        {
            dataUpdated = dataUpdated || m_probICs[i]->handledUpdate();
            probDataValid = probDataValid || m_probICs[i]->getData(); // at least one probabilistic tract must be there
            if( m_probICs[i]->getData() )
            {
                probTracts.push_back( m_probICs[i]->getData() );
            }
        }
        bool dataValid = ( detTracts || vectors ) && probDataValid;

        if( dataValid )
        {
            grid = boost::shared_dynamic_cast< WGridRegular3D >( probTracts.front()->getGrid() ); // assume all grids are the same and get the first
            if( !grid )
            {
                errorLog() << "A grid beside WGridRegular3D was used, aborting...";
                continue;
            }
        }
        else
        {
            continue;
        }

        if( dataUpdated || m_drawAlgorithm->changed() )
        {
            infoLog() << "Handling data update..";
            checkProbabilityRanges( probTracts );

            if( grid->getNbCoordsX() > 0 && grid->getNbCoordsY() > 0 &&  grid->getNbCoordsZ() > 0 )
            {
                updateProperties( grid );
            }
            else
            {
                warnLog() << "Invalid grid: at least one dimension is not greater zero.";
                continue;
            }

            WItemSelector algo = m_drawAlgorithm->get( true );
            if( algo.at( 0 )->getName() == std::string( "With largest eigen vectors" ) )
            {
                m_vectorGroup->setHidden( false );
                m_tractGroup->setHidden( true );
                if( !vectors )
                {
                    errorLog() << "No vector dataset available, but selected method needs one. Please connect one!";
                    continue;
                }
                else
                {
                    builder = boost::shared_ptr< WSPSliceBuilderVectors >( new WSPSliceBuilderVectors( probTracts, m_sliceGroup, m_colorMap,
                                vectors, m_vectorGroup, m_localPath ) );
                }
            }
            else if( algo.at( 0 )->getName() == std::string( "With deterministic tracts" ) )
            {
                m_vectorGroup->setHidden( true );
                m_tractGroup->setHidden( false );
                if( !detTracts )
                {
                    errorLog() << "No deterministic tracts on the connector present, but selected method needs one. Please connect one!";
                    continue;
                }
                else
                {
                    builder = boost::shared_ptr< WSPSliceBuilderTracts >( new WSPSliceBuilderTracts( probTracts, m_sliceGroup, m_colorMap, detTracts,
                                m_tractGroup ) );
                }
            }
            else
            {
                throw WNotImplemented( "Invalid method: Either \"With largest eigen vectors\" or \"With deterministic tracts\" expected." );
            }
        }

        // NOTE: we know that we could arrange much more specific updates here, just update the slice that has changed, but this is too complex to
        // consider also color, m_delta, etc. changes..., and ofcourse: we have the time to calc that every loop-cycle again and again, it does
        // not really matter
        debugLog() << "Building the geodes...";
        updateSlices( builder );
    }
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_output );
}

void WMProbTractDisplaySP::checkProbabilityRanges( std::vector< boost::shared_ptr< const WDataSetScalar > > probTracts ) const
{
    bool otherRange = false;

    // check probabilistic tractograms on their probability distribution, we assume value between 0..1 but also 0..255 may be possible sometimes
    std::stringstream ss;
    ss << "The probabilistic tractograms: ";
    size_t i = 0;
    for( std::vector< boost::shared_ptr< const WDataSetScalar > >::const_iterator p = probTracts.begin(); p != probTracts.end(); ++p, ++i )
    {
        if( ( *p )->getMax() > 10 ) // Note: same check is made in the builder, later when colors are deterimined and alpha values depending on prob
        {
            ss << ( *p )->getFileName() << " ";
            otherRange = true;
        }
    }
    ss << "which have probabilites > 10 may be invalid => range 0..255 assumed instead";
    if( otherRange )
    {
        warnLog() << ss.str();
    }
}

void WMProbTractDisplaySP::updateSlices( boost::shared_ptr< WSPSliceBuilder > builder )
{
    builder->preprocess();
    for( unsigned char sliceNum = 0; sliceNum < 3; ++sliceNum )
    {
        osg::ref_ptr< WGEGroupNode > intersectionAndProjection = builder->generateSlice( sliceNum );

        // determine correct slice group node
        osg::ref_ptr< WGEManagedGroupNode > sliceGroup;
        switch( sliceNum )
        {
            case 0 : sliceGroup = m_xSlice; break;
            case 1 : sliceGroup = m_ySlice; break;
            case 2 : sliceGroup = m_zSlice; break;
            default : throw WOutOfBounds( "Invalid slice number given. Must be in 0..2 in order to select x-, y- or z-Slice" );
        }
        sliceGroup->clear();
        sliceGroup->insert( intersectionAndProjection );
    }
}
