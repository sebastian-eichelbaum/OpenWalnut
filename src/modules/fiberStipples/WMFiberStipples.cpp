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
#include <osg/MatrixTransform>
// #include <osg/Vec3>

#include "core/dataHandler/WDataSetFibers.h"
#include "core/dataHandler/WDataSetScalar.h"
#include "core/dataHandler/WDataSetVector.h"
#include "core/dataHandler/WGridRegular3D.h"
#include "core/graphicsEngine/WGEManagedGroupNode.h"
#include "core/kernel/WKernel.h"
#include "core/kernel/WModuleInputData.h"
#include "WMFiberStipples.h"
#include "WMFiberStipples.xpm"

#include "core/common/math/WMath.h"
#include "core/common/WPropertyHelper.h"
#include "core/graphicsEngine/callbacks/WGELinearTranslationCallback.h"
#include "core/graphicsEngine/callbacks/WGENodeMaskCallback.h"
#include "core/graphicsEngine/callbacks/WGEPropertyUniformCallback.h"
#include "core/graphicsEngine/shaders/WGEPropertyUniform.h"
#include "core/graphicsEngine/shaders/WGEShader.h"
#include "core/graphicsEngine/shaders/WGEShaderDefineOptions.h"
#include "core/graphicsEngine/shaders/WGEShaderPropertyDefineOptions.h"
#include "core/graphicsEngine/WGEColormapping.h"
#include "core/graphicsEngine/WGEGeodeUtils.h"
#include "core/graphicsEngine/WGraphicsEngine.h"
#include "core/kernel/WSelectionManager.h"


// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMFiberStipples )

WMFiberStipples::WMFiberStipples()
    : WModule(),
      m_first( true )
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
    m_Pos = m_properties->addProperty( "Slice position", "Slice position.", 0.0 );
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
//    m_probThreshold    = m_tractGroup->addProperty( "Prob Threshold", "Only vertices with probabil. greater this contribute.", 0.1,
//                                             m_sliceChanged );
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

void WMFiberStipples::initOSG()
{
    debugLog() << "Init OSG";

    m_output->clear();

    // no colormaps -> no slices
    bool empty = !WGEColormapping::instance()->size();
    if( empty )
    {
        // hide the slider properties.
        m_Pos->setHidden();
        return;
    }

    // grab the current bounding box for computing the size of the slice
    WBoundingBox bb = WGEColormapping::instance()->getBoundingBox();
    WVector3d minV = bb.getMin();
    WVector3d maxV = bb.getMax();
    WVector3d sizes = ( maxV - minV );
    WVector3d midBB = minV + ( sizes * 0.5 );

    // update the properties
    m_Pos->setMin( minV[1] );
    m_Pos->setMax( maxV[1] );
    m_Pos->setHidden( false );

    // if this is done the first time, set the slices to the center of the dataset
    if( m_first )
    {
        m_first = false;
        m_Pos->set( midBB[1] );
    }

    // create a new geode containing the slices
    osg::ref_ptr< osg::Node > slice = wge::genFinitePlane( minV, osg::Vec3( sizes[0], 0.0, 0.0 ),
                                                                 osg::Vec3( 0.0, 0.0, sizes[2] ) );
    slice->setName( "Coronal Slice" );
    osg::Uniform* sliceUniform = new osg::Uniform( "u_WorldTransform", osg::Matrix::identity() );
    slice->getOrCreateStateSet()->addUniform( sliceUniform );
    slice->setCullingActive( false );

    // each slice is child of an transformation node
    osg::ref_ptr< osg::MatrixTransform > mT = new osg::MatrixTransform();
    mT->addChild( slice );

    // Control transformation node by properties. We use an additional uniform here to provide the shader
    // the transformation matrix used to translate the slice.
    mT->addUpdateCallback( new WGELinearTranslationCallback< WPropDouble >( osg::Vec3( 0.0, 1.0, 0.0 ), m_Pos, sliceUniform ) );

    m_output->getOrCreateStateSet()->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
    m_output->getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );
    m_output->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    m_output->insert( mT );
    m_output->dirtyBound();
}

void WMFiberStipples::moduleMain()
{
    // get notified about data changes
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_probIC->getDataChangedCondition() );
    m_moduleState.add( m_vectorIC->getDataChangedCondition() );

    ready();

    // graphics setup
    m_output = osg::ref_ptr< WGEManagedGroupNode >( new WGEManagedGroupNode( m_active ) );
    initOSG();
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_output );
    osg::ref_ptr< WGEShader > shader = new WGEShader( "WFiberStipples", m_localPath );
    WGEColormapping::apply( m_output, shader ); // this automatically applies the shader

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

        // save data behind connectors since it might change during processing
        boost::shared_ptr< WDataSetVector > vectors = m_vectorIC->getData();
        boost::shared_ptr< WDataSetScalar > probTract = m_probIC->getData();
        boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( probTract->getGrid() );

        if( !( vectors && probTract ) || !grid ) // if data valid
        {
            continue;
        }

        initOSG();

        wge::bindTexture( m_output, vectors->getTexture(), 0, "u_vectors" );
        wge::bindTexture( m_output, probTract->getTexture(), 1, "u_probTract" );

        // TODO(math): unbind textures, so we have a clean OSG root node for this module again
    }

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_output );
}
