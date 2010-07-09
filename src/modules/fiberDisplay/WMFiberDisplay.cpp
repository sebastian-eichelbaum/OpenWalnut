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

#include <list>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include <osg/Geode>
#include <osg/Geometry>

#include "../../common/WColor.h"
#include "../../common/WLogger.h"
#include "../../dataHandler/WDataHandler.h"
#include "../../dataHandler/WDataTexture3D.h"
#include "../../dataHandler/WSubject.h"
#include "../../graphicsEngine/WGEUtils.h"
#include "../../kernel/WKernel.h"
#include "fiberdisplay.xpm"
#include "WMFiberDisplay.h"

bool WMFiberDisplay::m_fiberDisplayRunning = false;

WMFiberDisplay::WMFiberDisplay()
    : WModule(),
      m_noData( new WCondition, true ),
      m_osgNode( osg::ref_ptr< osg::Group >() )
{
    m_shaderTubes = osg::ref_ptr< WShader > ( new WShader( "fake-tubes" ) );
    m_shaderTexturedFibers = osg::ref_ptr< WShader > ( new WShader( "texturedFibers" ) );
    m_textureChanged = true;
}

WMFiberDisplay::~WMFiberDisplay()
{
}

boost::shared_ptr< WModule > WMFiberDisplay::factory() const
{
    m_fiberDisplayRunning = true;
    return boost::shared_ptr< WModule >( new WMFiberDisplay() );
}

bool WMFiberDisplay::isRunning()
{
    return m_fiberDisplayRunning;
}

const char** WMFiberDisplay::getXPMIcon() const
{
    return fiberdisplay_xpm;
}

void WMFiberDisplay::moduleMain()
{
    // additional fire-condition: "data changed" flag
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_fiberInput->getDataChangedCondition() );

    // now, to watch changing/new textures use WSubject's change condition
    boost::signals2::connection con = WDataHandler::getDefaultSubject()->getChangeCondition()->subscribeSignal(
            boost::bind( &WMFiberDisplay::notifyTextureChange, this )
    );

    ready();

    while ( !m_shutdownFlag() ) // loop until the module container requests the module to quit
    {
        debugLog() << "Waiting for event";

        m_moduleState.wait(); // waits for firing of m_moduleState ( dataChanged, shutdown, etc. )

        /////////////////////////////////////////////////////////////////////////////////////////
        // what caused wait to return?
        /////////////////////////////////////////////////////////////////////////////////////////

        // data changed?
        if ( m_dataset != m_fiberInput->getData() )
        {
            // data has changed
            // -> recalculate
            debugLog() << "Data changed on " << m_fiberInput->getCanonicalName();

            m_dataset = m_fiberInput->getData();

            // ensure the data is valid (not NULL)
            if ( !m_fiberInput->getData().get() ) // ok, the output has been reset, so we can ignore the "data change"
            {
                m_noData.set( true );
                debugLog() << "Data reset on " << m_fiberInput->getCanonicalName() << ". Ignoring.";
                continue;
            }

            m_noData.set( false );

            infoLog() << "Fiber dataset for display with: " << m_dataset->size() << " fibers loaded.";

            if( m_dataset->size() != 0 ) // incase of an empty fiber dataset nothing is to display
            {
                boost::shared_ptr< WProgress > progress = boost::shared_ptr< WProgress >( new WProgress( "Fiber Display", 2 ) );
                m_progress->addSubProgress( progress );

                WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->removeChild( m_osgNode.get() );
                ++*progress;
                WKernel::getRunningKernel()->getRoiManager()->addFiberDataset( m_dataset );
                ++*progress;
                create();
                progress->finish();
            }
            else
            {
                warnLog() << "Nothing to display for an empty fiber dataset";
            }
        }
    }
}

void WMFiberDisplay::update()
{
    if( m_noData.changed() )
    {
        if( m_osgNode && m_noData.get( true ) )
        {
            m_osgNode->setNodeMask( 0x0 );
        }
        else
        {
            m_osgNode->setNodeMask( 0xFFFFFFFF );
        }
    }
}

void WMFiberDisplay::create()
{
    // create new node
    osg::ref_ptr< osg::Group > osgNodeNew = osg::ref_ptr< osg::Group >( new osg::Group );

    m_tubeDrawable = osg::ref_ptr< WTubeDrawable >( new WTubeDrawable );
    m_tubeDrawable->setDataset( m_dataset );
    m_tubeDrawable->setUseDisplayList( false );
    m_tubeDrawable->setDataVariance( osg::Object::DYNAMIC );

    osg::ref_ptr< osg::Geode > geode = osg::ref_ptr< osg::Geode >( new osg::Geode );
    geode->addDrawable( m_tubeDrawable );

    osgNodeNew->addChild( geode );

    osgNodeNew->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->addChild( osgNodeNew.get() );

    osgNodeNew->setUserData( this );
    osgNodeNew->addUpdateCallback( new fdNodeCallback );

    // remove previous nodes if there are any
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->removeChild( m_osgNode.get() );

    m_osgNode = osgNodeNew;

    activate();

    osg::StateSet* rootState = m_osgNode->getOrCreateStateSet();
    initUniforms( rootState );
}

void WMFiberDisplay::connectors()
{
    using boost::shared_ptr;
    typedef WModuleInputData< const WDataSetFibers > FiberInputData;  // just an alias

    m_fiberInput = shared_ptr< FiberInputData >( new FiberInputData( shared_from_this(), "fiberInput", "A loaded fiber dataset." ) );

    typedef WModuleOutputData< WFiberCluster > ClusterOutputData; // just an alias

    m_clusterOC = shared_ptr< ClusterOutputData >( new ClusterOutputData( shared_from_this(), "clusterOut", "FiberCluster of current selection" ) );

    addConnector( m_fiberInput );
    addConnector( m_clusterOC );

    WModule::connectors();  // call WModules initialization
}

void WMFiberDisplay::activate()
{
    if( m_osgNode )
    {
        if( m_active->get() )
        {
            m_osgNode->setNodeMask( 0xFFFFFFFF );
        }
        else
        {
            m_osgNode->setNodeMask( 0x0 );
        }
    }

    WModule::activate();
}

void WMFiberDisplay::properties()
{
    m_customColoring = m_properties->addProperty( "Custom Coloring", "Switches the coloring between custom and predefined.", false );
    m_coloring = m_properties->addProperty( "Global or Local Coloring", "Switches the coloring between global and local.", true );

    m_useTubesProp = m_properties->addProperty( "Use Tubes", "Draw fiber tracts as fake tubes.", false );
    m_useTextureProp = m_properties->addProperty( "Use Texture", "Texture fibers with the texture on top of the list.", false );
    m_tubeThickness = m_properties->addProperty( "Tube Thickness", "Adjusts the thickness of the tubes.", 50.,
            boost::bind( &WMFiberDisplay::adjustTubes, this ) );
    m_tubeThickness->setMin( 0 );
    m_tubeThickness->setMax( 300 );

    m_save = m_properties->addProperty( "Save", "saves the selected fiber bundles.", false, boost::bind( &WMFiberDisplay::saveSelected, this ) );
    m_saveFileName = m_properties->addProperty( "File Name", "no description yet", WKernel::getAppPathObject() );
    m_updateOC = m_properties->addProperty( "Update Output Conn.",
                                            "Updates the output connector with the currently selected fibers",
                                            WPVBaseTypes::PV_TRIGGER_READY,
                                            boost::bind( &WMFiberDisplay::updateOutput, this ) );
}

void WMFiberDisplay::toggleTubes()
{
    osg::StateSet* rootState = m_osgNode->getOrCreateStateSet();

    if ( m_textureChanged )
    {
        m_textureChanged = false;
        updateTexture();
    }

    if( m_useTubesProp->changed() || m_useTextureProp->changed() )
    {
        if ( m_useTubesProp->get( true ) )
        {
            updateTexture();
            m_useTextureProp->get( true );
            m_tubeDrawable->setUseTubes( true );
            m_shaderTubes->apply( m_osgNode );
            rootState->addUniform( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "globalColor", 1 ) ) );
            m_uniformTubeThickness = osg::ref_ptr<osg::Uniform>( new osg::Uniform( "u_thickness", static_cast<float>( m_tubeThickness->get() ) ) );
            rootState->addUniform( m_uniformTubeThickness );
            rootState->addUniform( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "useTexture", m_useTextureProp->get() ) ) );
        }
        else if ( m_useTextureProp->get( true ) && !m_useTubesProp->get( true ) )
        {
            m_tubeDrawable->setUseTubes( false );
            updateTexture();
            m_shaderTubes->deactivate( m_osgNode );
            m_shaderTexturedFibers->apply( m_osgNode );
        }
        else
        {
            m_tubeDrawable->setUseTubes( false );
            m_shaderTubes->deactivate( m_osgNode );
            m_shaderTexturedFibers->deactivate( m_osgNode );
        }
    }

    if  ( !m_useTextureProp->get( true ) && !m_useTubesProp->get( true ) )
    {
        rootState->setTextureMode( 0, GL_TEXTURE_3D, osg::StateAttribute::OFF );
    }
}

void WMFiberDisplay::toggleColoring()
{
    if( m_coloring->changed() || m_customColoring->changed() )
    {
        m_tubeDrawable->setColoringMode( m_coloring->get( true ) );
        m_tubeDrawable->setCustomColoring( m_customColoring->get( true ) );
    }
}

void WMFiberDisplay::adjustTubes()
{
    if ( m_tubeThickness.get() && m_useTubesProp.get() )
    {
        if ( m_tubeThickness->changed() && m_useTubesProp->get( true ) )
        {
            m_uniformTubeThickness->set( static_cast<float>( m_tubeThickness->get() ) );
        }
    }
}

void WMFiberDisplay::saveSelected()
{
    boost::shared_ptr< std::vector< bool > > active = WKernel::getRunningKernel()->getRoiManager()->getBitField();
    m_dataset->saveSelected( m_saveFileName->getAsString(), active );
}

void WMFiberDisplay::updateOutput() const
{
    std::vector< wmath::WFiber > fibs;
    std::list< size_t > indices;
    boost::shared_ptr< std::vector< bool > > active = WKernel::getRunningKernel()->getRoiManager()->getBitField();
    for( size_t i = 0; i < active->size(); ++i ) // std::vector< bool >::const_iterator cit = active->begin(); cit != active->end(); ++cit, ++index )
    {
        if( ( *active )[i] )
        {
            size_t length = ( * m_dataset->getLineLengths() )[ i ];
            wmath::WFiber f;
            f.reserve( 3 * length );
            for( size_t p = 0; p < length; ++p )
            {
                f.push_back( m_dataset->getPosition( i, p ) );
            }
            indices.push_back( fibs.size() );
            fibs.push_back( f );
        }
    }
    boost::shared_ptr< WFiberCluster > result( new WFiberCluster );
    result->setDataSetReference( boost::shared_ptr< WDataSetFiberVector >( new WDataSetFiberVector( boost::shared_ptr< std::vector< wmath::WFiber > >( new std::vector< wmath::WFiber >( fibs ) ) ) ) ); // NOLINT
    result->setIndices( indices );
    m_clusterOC->updateData( result );
    m_updateOC->set( WPVBaseTypes::PV_TRIGGER_READY, false );
}

void WMFiberDisplay::updateTexture()
{
    osg::StateSet* rootState = m_osgNode->getOrCreateStateSet();

    // grab a list of data textures
    std::vector< boost::shared_ptr< WDataTexture3D > > tex = WDataHandler::getDefaultSubject()->getDataTextures();

    if ( tex.size() > 0 )
    {
        osg::ref_ptr<osg::Texture3D> texture3D = tex[0]->getTexture();

        if ( tex[0]->isInterpolated() )
        {
            texture3D->setFilter( osg::Texture::MIN_FILTER, osg::Texture::LINEAR );
            texture3D->setFilter( osg::Texture::MAG_FILTER, osg::Texture::LINEAR );
        }
        else
        {
            texture3D->setFilter( osg::Texture::MIN_FILTER, osg::Texture::NEAREST );
            texture3D->setFilter( osg::Texture::MAG_FILTER, osg::Texture::NEAREST );
        }
        rootState->setTextureAttributeAndModes( 0, texture3D, osg::StateAttribute::ON );


        m_uniformType->set( tex[0]->getDataType() );
        float minValue = tex[0]->getMinValue();
        float maxValue = tex[0]->getMaxValue();
        float thresh = ( tex[0]->getThreshold() - minValue ) / ( maxValue - minValue ); // rescale to [0,1]

        m_uniformThreshold->set( thresh );
        m_uniformsColorMap->set( tex[0]->getSelectedColormap() );

        m_uniformDimX->set( static_cast<int>( tex[0]->getGrid()->getNbCoordsX() ) );
        m_uniformDimY->set( static_cast<int>( tex[0]->getGrid()->getNbCoordsY() ) );
        m_uniformDimZ->set( static_cast<int>( tex[0]->getGrid()->getNbCoordsZ() ) );
    }
}

void WMFiberDisplay::initUniforms( osg::StateSet* rootState )
{
    m_uniformSampler = osg::ref_ptr<osg::Uniform>( new osg::Uniform( "tex", 0 ) );
    m_uniformType = osg::ref_ptr<osg::Uniform>( new osg::Uniform( "type", 0 ) );
    m_uniformThreshold = osg::ref_ptr<osg::Uniform>( new osg::Uniform( "threshold", 0.0f ) );
    m_uniformsColorMap = osg::ref_ptr<osg::Uniform>( new osg::Uniform( "cMap", 0 ) );

    m_uniformDimX = osg::ref_ptr<osg::Uniform>( new osg::Uniform( "dimX", 1 ) );
    m_uniformDimY = osg::ref_ptr<osg::Uniform>( new osg::Uniform( "dimY", 1 ) );
    m_uniformDimZ = osg::ref_ptr<osg::Uniform>( new osg::Uniform( "dimZ", 1 ) );

    rootState->addUniform( m_uniformSampler );
    rootState->addUniform( m_uniformType );
    rootState->addUniform( m_uniformThreshold );
    rootState->addUniform( m_uniformsColorMap );

    rootState->addUniform( m_uniformDimX );
    rootState->addUniform( m_uniformDimY );
    rootState->addUniform( m_uniformDimZ );
}

void WMFiberDisplay::notifyTextureChange()
{
    m_textureChanged = true;
}
