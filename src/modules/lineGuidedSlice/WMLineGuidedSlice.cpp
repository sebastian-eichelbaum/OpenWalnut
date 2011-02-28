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
#include <utility>

#include "../../common/math/WVector3D.h"
#include "../../common/WAssert.h"
#include "../../common/WLogger.h"
#include "../../dataHandler/datastructures/WFiberCluster.h"
#include "../../dataHandler/WDataHandler.h"
#include "../../dataHandler/WDataTexture3D.h"
#include "../../dataHandler/WSubject.h"
#include "../../graphicsEngine/shaders/WGEShader.h"
#include "../../graphicsEngine/WGEGeodeUtils.h"
#include "../../graphicsEngine/WGEUtils.h"
#include "../../kernel/WKernel.h"
#include "WMLineGuidedSlice.h"
#include "WMLineGuidedSlice.xpm"

W_LOADABLE_MODULE( WMLineGuidedSlice )

WMLineGuidedSlice::WMLineGuidedSlice():
    WModule(),
    m_textureChanged( true ),
    m_isPicked( false )
{
    m_shader = osg::ref_ptr< WGEShader >( new WGEShader( "WMLineGuidedSlice" ) );
}

WMLineGuidedSlice::~WMLineGuidedSlice()
{
}

boost::shared_ptr< WModule > WMLineGuidedSlice::factory() const
{
    return boost::shared_ptr< WModule >( new WMLineGuidedSlice() );
}

const char** WMLineGuidedSlice::getXPMIcon() const
{
    return lineGuidedSlice_xpm;
}

const std::string WMLineGuidedSlice::getName() const
{
    return "Line Guided Slice";
}

const std::string WMLineGuidedSlice::getDescription() const
{
    return "<font color=\"#0000ff\"><b>[Experimental Status]</b></font> A slice that can be moved along a line. ";
}

void WMLineGuidedSlice::connectors()
{
    typedef WModuleInputData< const WFiberCluster > ClusterInputType; // just an alias
    m_input = boost::shared_ptr< ClusterInputType >( new ClusterInputType( shared_from_this(),
                                                                           "cluster",
                                                                           "A fiber cluster, that can"
                                                                           " be used to get the centerline from." ) );
    addConnector( m_input );

    // call WModules initialization
    WModule::connectors();
}

void WMLineGuidedSlice::properties()
{
    m_pos = m_properties->addProperty( "Slice Position", "Position of ths slice along the line.", 0., true );
    m_pos->setMin( 0. );
    m_pos->setMax( 1. );

    WModule::properties();
}

void WMLineGuidedSlice::moduleMain()
{
    m_moduleState.add( m_input->getDataChangedCondition() );  // additional fire-condition: "data changed" flag
    boost::shared_ptr< WGraphicsEngine > ge = WGraphicsEngine::getGraphicsEngine();
    assert( ge );

    m_viewer = ge->getViewerByName( "main" );
    assert( m_viewer );
    m_viewer->getPickHandler()->getPickSignal()->connect( boost::bind( &WMLineGuidedSlice::setSlicePosFromPick, this, _1 ) );

    // signal ready state
    ready();

    // now, to watch changing/new textures use WSubject's change condition
    boost::signals2::connection con = WDataHandler::getDefaultSubject()->getChangeCondition()->subscribeSignal(
            boost::bind( &WMLineGuidedSlice::notifyTextureChange, this )
    );

    m_rootNode = osg::ref_ptr< WGEGroupNode >( new WGEGroupNode() );

    while( !m_shutdownFlag() ) // loop until the module container requests the module to quit
    {
        if( !m_input->getData() ) // ok, the output has not yet sent data
        {
            m_moduleState.wait();
            continue;
        }

        updateCenterLine();

        create();

        m_moduleState.wait(); // waits for firing of m_moduleState ( dataChanged, shutdown, etc. )
    }

    // clean up stuff
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode );

    // deregister from WSubject's change condition
    con.disconnect();
}

void WMLineGuidedSlice::notifyTextureChange()
{
    m_textureChanged = true;
}

void WMLineGuidedSlice::updateCenterLine()
{
    debugLog() << "Draw center line.";

    assert( m_rootNode );
    assert( m_input );
    assert( m_input->getData() );
    m_centerLine = m_input->getData()->getCenterLine();
    if( m_centerLine )
    {
        debugLog() << "Draw center line representation." << pathLength( *m_centerLine );
        m_centerLineGeode = wge::generateLineStripGeode( *m_centerLine, 2.f );
    }
    else
    {
        warnLog() << "CenterLine update on non existing CenterLine (null)";
        m_centerLineGeode = osg::ref_ptr< osg::Geode >( new osg::Geode() );
    }
    m_rootNode->insert( m_centerLineGeode );
}

void WMLineGuidedSlice::create()
{
    m_sliceNode = osg::ref_ptr<osg::Geode>( new osg::Geode() );
    m_sliceNode->setDataVariance( osg::Object::DYNAMIC );

    m_sliceNode->setName( "Line Guided Slice" );

    m_sliceNode->addDrawable( createGeometry() );

    m_rootNode->insert( m_sliceNode );
    m_shader->apply( m_sliceNode );

    osg::StateSet* sliceState = m_sliceNode->getOrCreateStateSet();
    initUniforms( sliceState );

    sliceState->setMode( GL_BLEND, osg::StateAttribute::ON );

    m_sliceNode->setUserData( this );
    m_sliceNode->addUpdateCallback( new sliceNodeCallback );

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_rootNode );
}

void WMLineGuidedSlice::setSlicePosFromPick( WPickInfo pickInfo )
{
    if( pickInfo.getName() == "Line Guided Slice" )
    {
        boost::unique_lock< boost::shared_mutex > lock;
        lock = boost::unique_lock< boost::shared_mutex >( m_updateLock );

        WVector3D normal = pickInfo.getPickNormal();

        std::pair< float, float > newPixelPos( pickInfo.getPickPixelPosition() );
        if( m_isPicked )
        {
            float diff = newPixelPos.first - m_oldPixelPosition.first;

            diff *= 0.01;

            m_pos->set( m_pos->get() + diff );

            lock.unlock();
        }
        m_oldPixelPosition = newPixelPos;
        m_isPicked |= true;
    }
    else
    {
        m_isPicked &= false;
    }
}

osg::ref_ptr<osg::Geometry> WMLineGuidedSlice::createGeometry()
{
    int posOnLine = m_pos->get() * m_centerLine->size();

    if( posOnLine > ( m_centerLine->size() - 2. ) )
    {
        posOnLine = m_centerLine->size() - 2;
    }

    if( posOnLine < 0 )
    {
        posOnLine = 0;
    }

    WAssert( m_centerLine->size() > 1, "To few positions in center line." );
    WPosition startPos = ( *m_centerLine )[posOnLine];
    WVector3D startSliceNormal = ( startPos - ( *m_centerLine )[posOnLine + 1] ).normalized();
    WVector3D sliceVec1 = WVector3D( 1, 0, 0 ).crossProduct( startSliceNormal ).normalized();
    WVector3D sliceVec2 =  sliceVec1.crossProduct( startSliceNormal ).normalized();

    osg::ref_ptr<osg::Geometry> sliceGeometry = osg::ref_ptr<osg::Geometry>( new osg::Geometry() );

    osg::Vec3Array* sliceVertices = new osg::Vec3Array;

    // grab a list of data textures
    std::vector< boost::shared_ptr< WDataTexture3D > > tex = WDataHandler::getDefaultSubject()->getDataTextures( true );

    if( tex.size() > 0 )
    {
        const double radius = 100;
        std::vector< WPosition > vertices;
        vertices.push_back( startPos + (      sliceVec1 + sliceVec2 ) * radius );
        vertices.push_back( startPos + ( -1 * sliceVec1 + sliceVec2 ) * radius );
        vertices.push_back( startPos + ( -1 * sliceVec1 - sliceVec2 ) * radius );
        vertices.push_back( startPos + (      sliceVec1 - sliceVec2 ) * radius );

        const size_t nbVerts = 4;
        for( size_t i = 0; i < nbVerts; ++i )
        {
            sliceVertices->push_back( vertices[i] );
        }
        sliceGeometry->setVertexArray( sliceVertices );

        int counter = 0;
        for( std::vector< boost::shared_ptr< WDataTexture3D > >::const_iterator iter = tex.begin(); iter != tex.end(); ++iter )
        {
            boost::shared_ptr< WGridRegular3D > grid = ( *iter )->getGrid();

            osg::Vec3Array* texCoords = new osg::Vec3Array;
            texCoords->clear();
            for( size_t i = 0; i < nbVerts; ++i )
            {
                texCoords->push_back( grid->worldCoordToTexCoord( vertices[i] + WVector3D( 0.5, 0.5, 0.5 ) ) );
            }
            sliceGeometry->setTexCoordArray( counter, texCoords );
            ++counter;
        }
    }

    osg::DrawElementsUInt* quad = new osg::DrawElementsUInt( osg::PrimitiveSet::QUADS, 0 );
    quad->push_back( 3 );
    quad->push_back( 2 );
    quad->push_back( 1 );
    quad->push_back( 0 );
    sliceGeometry->addPrimitiveSet( quad );

    return sliceGeometry;
}

void WMLineGuidedSlice::updateGeometry()
{
    boost::shared_lock<boost::shared_mutex> slock;
    slock = boost::shared_lock<boost::shared_mutex>( m_updateLock );

    osg::ref_ptr<osg::Geometry> sliceGeometry = createGeometry();

    osg::ref_ptr<osg::Drawable> old = osg::ref_ptr<osg::Drawable>( m_sliceNode->getDrawable( 0 ) );
    m_sliceNode->replaceDrawable( old, sliceGeometry );

    std::vector< boost::shared_ptr< WDataTexture3D > > tex = WDataHandler::getDefaultSubject()->getDataTextures( true );

    slock.unlock();
}


void WMLineGuidedSlice::updateTextures()
{
    osg::StateSet* sliceState = m_sliceNode->getOrCreateStateSet();
    if( m_textureChanged )
    {
        m_textureChanged = false;

        // grab a list of data textures
        std::vector< boost::shared_ptr< WDataTexture3D > > tex = WDataHandler::getDefaultSubject()->getDataTextures( true );

        if( tex.size() > 0 )
        {
            // reset all uniforms
            for( int i = 0; i < 2; ++i )
            {
                m_typeUniforms[i]->set( 0 );
            }

            // for each texture -> apply
            int c = 0;
            for( std::vector< boost::shared_ptr< WDataTexture3D > >::const_iterator iter = tex.begin(); iter != tex.end(); ++iter )
            {
                osg::ref_ptr<osg::Texture3D> texture3D = ( *iter )->getTexture();
                sliceState->setTextureAttributeAndModes( c, texture3D, osg::StateAttribute::ON );

                // set threshold/opacity as uniforms
                float t = ( *iter )->getThreshold() / 255.0;
                float a = ( *iter )->getAlpha();

                m_typeUniforms[c]->set( ( *iter )->getDataType() );
                m_thresholdUniforms[c]->set( t );
                m_alphaUniforms[c]->set( a );

                ++c;
            }

            bool useTexture = m_properties->getProperty( "Use texture" )->toPropBool()->get();
            sliceState->addUniform( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "useTexture", useTexture ) ) );
        }
    }

    m_highlightUniform->set( m_isPicked );

    m_sliceNode->getOrCreateStateSet()->merge( *sliceState );
}

void WMLineGuidedSlice::initUniforms( osg::StateSet* sliceState )
{
    boost::shared_lock<boost::shared_mutex> slock;
    slock = boost::shared_lock<boost::shared_mutex>( m_updateLock );

    m_typeUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "type0", 0 ) ) );
    m_typeUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "type1", 0 ) ) );

    m_alphaUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "alpha0", 1.0f ) ) );
    m_alphaUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "alpha1", 1.0f ) ) );

    m_thresholdUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "threshold0", 0.0f ) ) );
    m_thresholdUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "threshold1", 0.0f ) ) );

    m_samplerUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "tex0", 0 ) ) );
    m_samplerUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "tex1", 1 ) ) );

    for( int i = 0; i < 2; ++i )
    {
        sliceState->addUniform( m_typeUniforms[i] );
        sliceState->addUniform( m_thresholdUniforms[i] );
        sliceState->addUniform( m_alphaUniforms[i] );
        sliceState->addUniform( m_samplerUniforms[i] );
    }

    m_highlightUniform = osg::ref_ptr<osg::Uniform>( new osg::Uniform( "highlighted", 0 ) );

    m_sliceNode->getOrCreateStateSet()->addUniform( m_highlightUniform );

    slock.unlock();
}
