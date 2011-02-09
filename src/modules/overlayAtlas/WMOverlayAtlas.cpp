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

#include <boost/regex.hpp>

#include "../../common/WPathHelper.h"
#include "../../common/WPropertyHelper.h"
#include "../../dataHandler/WDataHandler.h"
#include "../../dataHandler/WDataSet.h"
#include "../../dataHandler/WDataSetScalar.h"
#include "../../dataHandler/WDataSetSingle.h"
#include "../../dataHandler/WDataTexture3D.h"
#include "../../dataHandler/WGridRegular3D.h"
#include "../../dataHandler/WSubject.h"
#include "../../dataHandler/WValueSet.h"
#include "../../graphicsEngine/shaders/WGEShader.h"
#include "../../kernel/WKernel.h"
#include "../../kernel/WSelectionManager.h"
#include "WMOverlayAtlas.h"
#include "WMOverlayAtlas.xpm"

// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMOverlayAtlas )

WMOverlayAtlas::WMOverlayAtlas():
    WModule()
{
}

WMOverlayAtlas::~WMOverlayAtlas()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMOverlayAtlas::factory() const
{
    // See "src/modules/template/" for an extensively documented example.
    return boost::shared_ptr< WModule >( new WMOverlayAtlas() );
}

const char** WMOverlayAtlas::getXPMIcon() const
{
    return WMOverlayAtlas_xpm; // Please put a real icon here.
}
const std::string WMOverlayAtlas::getName() const
{
    // Specify your module name here. This name must be UNIQUE!
    return "OverlayAtlas";
}

const std::string WMOverlayAtlas::getDescription() const
{
    // Specify your module description here. Be detailed. This text is read by the user.
    // See "src/modules/template/" for an extensively documented example.
    return "Creates 2D overlays on slices";
}

void WMOverlayAtlas::connectors()
{
    // Put the code for your connectors here. See "src/modules/template/" for an extensively documented example.

    // call WModules initialization
    WModule::connectors();
}

void WMOverlayAtlas::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );
    WPropertyBase::PropertyChangeNotifierType propertyCallback = boost::bind( &WMOverlayAtlas::propertyChanged, this, _1 );

    //m_propMetaFile = m_properties->addProperty( "Tree file", "", WPathHelper::getAppPath() );
    m_propMetaFile = m_properties->addProperty( "Tree file", "", boost::filesystem::path( "/SCR/schurade/data/manh/rat_atlas/atlasCoronal.txt" ) );

    m_propReadTrigger = m_properties->addProperty( "Do read",  "Press!", WPVBaseTypes::PV_TRIGGER_READY, m_propCondition );

    m_showComplete = m_properties->addProperty( "Show complete", "Slice should be drawn complete even if the texture value is zero.",
            false, m_propCondition );

    m_showManipulators = m_properties->addProperty( "Show manipulators", "", false, m_propCondition );

    m_propCoronalSlicePos = m_properties->addProperty( "Slice Number", "", 0, m_propCondition );

    WPropertyHelper::PC_PATHEXISTS::addTo( m_propMetaFile );
}

void WMOverlayAtlas::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_propCondition );
    m_moduleState.add( m_active->getUpdateCondition() );

    ready();

    bool atlasLoaded = false;

    // wait for the user to manually load an atlas file
    while ( !m_shutdownFlag() )
    {
        if ( m_shutdownFlag() )
        {
            break;
        }

        m_moduleState.wait();

        if ( m_propReadTrigger->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
        {
            std::string fileName = m_propMetaFile->get().file_string().c_str();

            atlasLoaded = loadAtlas( fileName );
            m_propReadTrigger->set( WPVBaseTypes::PV_TRIGGER_READY, true );
            if ( atlasLoaded )
            {
                break;
            }
        }
    }

    m_propMetaFile->setHidden( true );
    m_propReadTrigger->setHidden( true );

    m_propCoronalSlicePos->setMin( 0 );
    m_propCoronalSlicePos->setMax( m_coronalSlices.size() - 1 );
    m_propCoronalSlicePos->set( m_coronalSlices.size() / 2 );

    m_shader = osg::ref_ptr< WGEShader > ( new WGEShader( "WMOverlayAtlas", m_localPath ) );

    init();

    updatePlane();

    osg::StateSet* rootState = m_rootNode->getOrCreateStateSet();

    initUniforms( rootState );

    updateTextures();

    m_shader->apply( m_rootNode );


    boost::signals2::connection con = WDataHandler::getDefaultSubject()->getChangeCondition()->subscribeSignal(
            boost::bind( &WMOverlayAtlas::notifyTextureChange, this ) );

    while ( !m_shutdownFlag() )
    {
        if ( m_shutdownFlag() )
        {
            break;
        }

        if ( m_active->changed() )
        {
            if ( !m_active->get( true ) )
            {
            }
            else
            {
                manipulatorMoved();
            }
        }

        if ( m_active->get() )
        {
            if ( m_propCoronalSlicePos->changed() )
            {
                if ( !m_coronalSlices.empty() )
                {
                    m_textureChanged = true;
                    manipulatorMoved();
                }
            }

            if ( m_showManipulators->changed() )
            {
                toggleManipulators();
            }
        }
        m_moduleState.wait();
    }

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode );

    m_s0->removeROIChangeNotifier( m_changeRoiSignal );
    m_s1->removeROIChangeNotifier( m_changeRoiSignal );
    m_s2->removeROIChangeNotifier( m_changeRoiSignal );
    m_s3->removeROIChangeNotifier( m_changeRoiSignal );
    m_s4->removeROIChangeNotifier( m_changeRoiSignal );
}

void WMOverlayAtlas::init()
{
    m_rootNode = new WGEManagedGroupNode( m_active );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_rootNode );
    m_geode = new osg::Geode();
    m_geode->setName( "_atlasSlice" );

    m_rootNode->addUpdateCallback( new WGEFunctorCallback< osg::Node >( boost::bind( &WMOverlayAtlas::updateCallback, this ) ) );
    m_rootNode->insert( m_geode );

    wmath::WPosition center;

    size_t p = m_propCoronalSlicePos->get();

    center[0] = m_coronalSlices[p].left() + ( ( m_coronalSlices[p].right() - m_coronalSlices[p].left() ) / 2. );
    center[1] = m_coronalSlices[p].position();
    center[2] = m_coronalSlices[p].bottom() + ( ( m_coronalSlices[p].top() - m_coronalSlices[p].bottom() ) / 2. );

    m_p0 = wmath::WPosition( center );
    m_p1 = wmath::WPosition( center[0], center[1], center[2] );
    m_p2 = wmath::WPosition( center[0], center[1], center[2] );
    m_p3 = wmath::WPosition( center[0], center[1], center[2] );
    m_p4 = wmath::WPosition( center[0], center[1], center[2] );

    m_p1[0] = m_coronalSlices[0].left();
    m_p2[2] = m_coronalSlices[0].top();
    m_p3[0] = m_coronalSlices[0].right();
    m_p4[2] = m_coronalSlices[0].bottom();


    m_s0 = boost::shared_ptr<WROISphere>( new WROISphere( m_p0, 0.5 ) );
    m_s0->setLockY();
    m_s1 = boost::shared_ptr<WROISphere>( new WROISphere( m_p1, 0.5 ) );
    m_s1->setLockY();
    m_s1->setLockZ();
    m_s2 = boost::shared_ptr<WROISphere>( new WROISphere( m_p2, 0.5 ) );
    m_s2->setLockX();
    m_s2->setLockY();
    m_s3 = boost::shared_ptr<WROISphere>( new WROISphere( m_p3, 0.5 ) );
    m_s3->setLockY();
    m_s3->setLockZ();
    m_s4 = boost::shared_ptr<WROISphere>( new WROISphere( m_p4, 0.5 ) );
    m_s4->setLockX();
    m_s4->setLockY();

    WGraphicsEngine::getGraphicsEngine()->getScene()->addChild( &( *m_s0 ) );
    WGraphicsEngine::getGraphicsEngine()->getScene()->addChild( &( *m_s1 ) );
    WGraphicsEngine::getGraphicsEngine()->getScene()->addChild( &( *m_s2 ) );
    WGraphicsEngine::getGraphicsEngine()->getScene()->addChild( &( *m_s3 ) );
    WGraphicsEngine::getGraphicsEngine()->getScene()->addChild( &( *m_s4 ) );

    m_changeRoiSignal =
        boost::shared_ptr< boost::function< void() > >( new boost::function< void() >( boost::bind( &WMOverlayAtlas::manipulatorMoved, this ) ) );
    m_s0->addROIChangeNotifier( m_changeRoiSignal );
    m_s1->addROIChangeNotifier( m_changeRoiSignal );
    m_s2->addROIChangeNotifier( m_changeRoiSignal );
    m_s3->addROIChangeNotifier( m_changeRoiSignal );
    m_s4->addROIChangeNotifier( m_changeRoiSignal );

    toggleManipulators();
}

void WMOverlayAtlas::manipulatorMoved()
{
    size_t i = m_propCoronalSlicePos->get();
    m_s0->setPosition( wmath::WPosition( m_s0->getPosition()[0], m_coronalSlices[i].position(), m_s0->getPosition()[2] ) );

    wmath::WPosition s0Move = m_s0->getPosition() - m_p0;
    m_p0 = m_s0->getPosition();

    m_s1->setPosition( m_s1->getPosition() + s0Move );
    m_s2->setPosition( m_s2->getPosition() + s0Move );
    m_s3->setPosition( m_s3->getPosition() + s0Move );
    m_s4->setPosition( m_s4->getPosition() + s0Move );

    m_p1 = m_s1->getPosition();
    m_p2 = m_s2->getPosition();
    m_p3 = m_s3->getPosition();
    m_p4 = m_s4->getPosition();

    for ( size_t i = 0; i < m_coronalSlices.size(); ++i )
    {
        m_coronalSlices[i].setLeft(   m_p1[0] );
        m_coronalSlices[i].setTop(    m_p2[2] );
        m_coronalSlices[i].setRight(  m_p3[0] );
        m_coronalSlices[i].setBottom( m_p4[2] );
    }

    updateCoronalSlice();
}


void WMOverlayAtlas::updateCoronalSlice()
{
    if ( m_coronalSlices.empty() )
    {
        return;
    }

//    size_t i = m_propCoronalSlicePos->get();
//    WKernel::getRunningKernel()->getSelectionManager()->setTexture( m_coronalSlices[i].getImage(), m_coronalSlices[i].getGrid() );
//    WKernel::getRunningKernel()->getSelectionManager()->setShader( 0 );
//    WKernel::getRunningKernel()->getSelectionManager()->setUseTexture( true );
//    WDataHandler::getDefaultSubject()->getChangeCondition()->notify();

    m_dirty = true;
}

void WMOverlayAtlas::propertyChanged( boost::shared_ptr< WPropertyBase > property )
{
    //TODO(all): remove void cast when the property variable is used
    ( void ) property; // NOLINT cstyle cast
}

void WMOverlayAtlas::updatePlane()
{
    m_geode->removeDrawables( 0, 1 );

    wmath::WPosition v0( m_p1[0], m_p1[1], m_p2[2] );
    wmath::WPosition v1( m_p3[0], m_p3[1], m_p2[2] );
    wmath::WPosition v2( m_p3[0], m_p3[1], m_p4[2] );
    wmath::WPosition v3( m_p1[0], m_p1[1], m_p4[2] );

    osg::ref_ptr<osg::Geometry> planeGeometry = osg::ref_ptr<osg::Geometry>( new osg::Geometry() );
    osg::Vec3Array* planeVertices = new osg::Vec3Array;

    planeVertices->push_back( v0 );
    planeVertices->push_back( v1 );
    planeVertices->push_back( v2 );
    planeVertices->push_back( v3 );

    planeGeometry->setVertexArray( planeVertices );
    osg::DrawElementsUInt* quad = new osg::DrawElementsUInt( osg::PrimitiveSet::QUADS, 0 );
    quad->push_back( 3 );
    quad->push_back( 2 );
    quad->push_back( 1 );
    quad->push_back( 0 );
    planeGeometry->addPrimitiveSet( quad );

    // grab a list of data textures
    std::vector< boost::shared_ptr< WDataTexture3D > > tex = WDataHandler::getDefaultSubject()->getDataTextures( true );

    size_t c = 0;
    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    if ( m_active->get() )
    {
        size_t i = m_propCoronalSlicePos->get();
        boost::shared_ptr< WGridRegular3D > grid = m_coronalSlices[i].getGrid();
        osg::Vec3Array* texCoords = new osg::Vec3Array;

        texCoords->push_back( grid->worldCoordToTexCoord( v0 ) );
        texCoords->push_back( grid->worldCoordToTexCoord( v1 ) );
        texCoords->push_back( grid->worldCoordToTexCoord( v2 ) );
        texCoords->push_back( grid->worldCoordToTexCoord( v3 ) );

        planeGeometry->setTexCoordArray( c, texCoords );
        ++c;
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    if ( WKernel::getRunningKernel()->getSelectionManager()->getUseTexture() )
    {
        boost::shared_ptr< WGridRegular3D > grid = WKernel::getRunningKernel()->getSelectionManager()->getGrid();
        osg::Vec3Array* texCoords = new osg::Vec3Array;

        texCoords->push_back( grid->worldCoordToTexCoord( v0 ) );
        texCoords->push_back( grid->worldCoordToTexCoord( v1 ) );
        texCoords->push_back( grid->worldCoordToTexCoord( v2 ) );
        texCoords->push_back( grid->worldCoordToTexCoord( v3 ) );

        planeGeometry->setTexCoordArray( c, texCoords );
        ++c;
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    for ( std::vector< boost::shared_ptr< WDataTexture3D > >::const_iterator iter = tex.begin(); iter != tex.end(); ++iter )
    {
        boost::shared_ptr< WGridRegular3D > grid = ( *iter )->getGrid();

        osg::Vec3Array* texCoords = new osg::Vec3Array;

        texCoords->push_back( grid->worldCoordToTexCoord( v0 ) );
        texCoords->push_back( grid->worldCoordToTexCoord( v1 ) );
        texCoords->push_back( grid->worldCoordToTexCoord( v2 ) );
        texCoords->push_back( grid->worldCoordToTexCoord( v3 ) );

        planeGeometry->setTexCoordArray( c, texCoords );
        ++c;
        if( c == wlimits::MAX_NUMBER_OF_TEXTURES )
        {
            break;
        }
    }

    m_geode->addDrawable( planeGeometry );

    m_dirty = false;
}

void WMOverlayAtlas::updateTextures()
{
    osg::StateSet* rootState = m_rootNode->getOrCreateStateSet();
    if ( m_textureChanged )
    {
        m_textureChanged = false;

        // grab a list of data textures
        std::vector< boost::shared_ptr< WDataTexture3D > > tex = WDataHandler::getDefaultSubject()->getDataTextures( true );

        if ( tex.size() > 0 )
        {
            // reset all uniforms
            for ( size_t i = 0; i < wlimits::MAX_NUMBER_OF_TEXTURES; ++i )
            {
                m_typeUniforms[i]->set( 0 );
            }

            // for each texture -> apply
            size_t c = 0;
            //////////////////////////////////////////////////////////////////////////////////////////////////
            if ( m_active->get() )
            {
                size_t i = m_propCoronalSlicePos->get();
                osg::ref_ptr<osg::Texture3D> texture3D = m_coronalSlices[i].getImage();

                m_typeUniforms[c]->set( W_DT_UNSIGNED_CHAR  );
                m_thresholdUniforms[c]->set( 0.0f );
                m_alphaUniforms[c]->set( 1.0f );
                m_cmapUniforms[c]->set( 4 );

                texture3D->setFilter( osg::Texture::MIN_FILTER, osg::Texture::NEAREST );
                texture3D->setFilter( osg::Texture::MAG_FILTER, osg::Texture::NEAREST );

                rootState->setTextureAttributeAndModes( c, texture3D, osg::StateAttribute::ON );
                ++c;
            }
            //////////////////////////////////////////////////////////////////////////////////////////////////
            if ( WKernel::getRunningKernel()->getSelectionManager()->getUseTexture() )
            {
                osg::ref_ptr<osg::Texture3D> texture3D = WKernel::getRunningKernel()->getSelectionManager()->getTexture();


                m_typeUniforms[c]->set( W_DT_UNSIGNED_CHAR  );
                m_thresholdUniforms[c]->set( 0.0f );
                m_alphaUniforms[c]->set( WKernel::getRunningKernel()->getSelectionManager()->getTextureOpacity() );
                m_cmapUniforms[c]->set( 4 );

                texture3D->setFilter( osg::Texture::MIN_FILTER, osg::Texture::NEAREST );
                texture3D->setFilter( osg::Texture::MAG_FILTER, osg::Texture::NEAREST );

                rootState->setTextureAttributeAndModes( c, texture3D, osg::StateAttribute::ON );
                ++c;
            }
            //////////////////////////////////////////////////////////////////////////////////////////////////

            for ( std::vector< boost::shared_ptr< WDataTexture3D > >::const_iterator iter = tex.begin(); iter != tex.end(); ++iter )
            {
                osg::ref_ptr<osg::Texture3D> texture3D = ( *iter )->getTexture();

                if ( ( *iter )->isInterpolated() )
                {
                    texture3D->setFilter( osg::Texture::MIN_FILTER, osg::Texture::LINEAR );
                    texture3D->setFilter( osg::Texture::MAG_FILTER, osg::Texture::LINEAR );
                }
                else
                {
                    texture3D->setFilter( osg::Texture::MIN_FILTER, osg::Texture::NEAREST );
                    texture3D->setFilter( osg::Texture::MAG_FILTER, osg::Texture::NEAREST );
                }
                rootState->setTextureAttributeAndModes( c, texture3D, osg::StateAttribute::ON );

                // set threshold/opacity as uniforms
                float minValue = ( *iter )->getMinValue();
                float maxValue = ( *iter )->getMaxValue();
                float t = ( ( *iter )->getThreshold() - minValue ) / ( maxValue - minValue ); // rescale to [0,1]
                float a = ( *iter )->getAlpha();
                int cmap = ( *iter )->getSelectedColormap();

                m_typeUniforms[c]->set( ( *iter )->getDataType() );
                m_thresholdUniforms[c]->set( t );
                m_alphaUniforms[c]->set( a );
                m_cmapUniforms[c]->set( cmap );

                ++c;
                if( c == wlimits::MAX_NUMBER_OF_TEXTURES )
                {
                    break;
                }
            }
        }
    }

    m_showCompleteUniform->set( m_showComplete->get() );
}

void WMOverlayAtlas::initUniforms( osg::StateSet* rootState )
{
    m_typeUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "type0", 0 ) ) );
    m_typeUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "type1", 0 ) ) );
    m_typeUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "type2", 0 ) ) );
    m_typeUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "type3", 0 ) ) );
    m_typeUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "type4", 0 ) ) );
    m_typeUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "type5", 0 ) ) );
    m_typeUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "type6", 0 ) ) );
    m_typeUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "type7", 0 ) ) );
    m_typeUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "type8", 0 ) ) );
    m_typeUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "type9", 0 ) ) );

    m_alphaUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "alpha0", 1.0f ) ) );
    m_alphaUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "alpha1", 1.0f ) ) );
    m_alphaUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "alpha2", 1.0f ) ) );
    m_alphaUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "alpha3", 1.0f ) ) );
    m_alphaUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "alpha4", 1.0f ) ) );
    m_alphaUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "alpha5", 1.0f ) ) );
    m_alphaUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "alpha6", 1.0f ) ) );
    m_alphaUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "alpha7", 1.0f ) ) );
    m_alphaUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "alpha8", 1.0f ) ) );
    m_alphaUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "alpha9", 1.0f ) ) );

    m_thresholdUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "threshold0", 0.0f ) ) );
    m_thresholdUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "threshold1", 0.0f ) ) );
    m_thresholdUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "threshold2", 0.0f ) ) );
    m_thresholdUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "threshold3", 0.0f ) ) );
    m_thresholdUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "threshold4", 0.0f ) ) );
    m_thresholdUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "threshold5", 0.0f ) ) );
    m_thresholdUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "threshold6", 0.0f ) ) );
    m_thresholdUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "threshold7", 0.0f ) ) );
    m_thresholdUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "threshold8", 0.0f ) ) );
    m_thresholdUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "threshold9", 0.0f ) ) );

    m_samplerUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "tex0", 0 ) ) );
    m_samplerUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "tex1", 1 ) ) );
    m_samplerUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "tex2", 2 ) ) );
    m_samplerUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "tex3", 3 ) ) );
    m_samplerUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "tex4", 4 ) ) );
    m_samplerUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "tex5", 5 ) ) );
    m_samplerUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "tex6", 6 ) ) );
    m_samplerUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "tex7", 7 ) ) );
    m_samplerUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "tex8", 8 ) ) );
    m_samplerUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "tex9", 9 ) ) );

    m_cmapUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "useCmap0", 0 ) ) );
    m_cmapUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "useCmap1", 0 ) ) );
    m_cmapUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "useCmap2", 0 ) ) );
    m_cmapUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "useCmap3", 0 ) ) );
    m_cmapUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "useCmap4", 0 ) ) );
    m_cmapUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "useCmap5", 0 ) ) );
    m_cmapUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "useCmap6", 0 ) ) );
    m_cmapUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "useCmap7", 0 ) ) );
    m_cmapUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "useCmap8", 0 ) ) );
    m_cmapUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "useCmap9", 0 ) ) );

    for ( size_t i = 0; i < wlimits::MAX_NUMBER_OF_TEXTURES; ++i )
    {
        rootState->addUniform( m_typeUniforms[i] );
        rootState->addUniform( m_thresholdUniforms[i] );
        rootState->addUniform( m_alphaUniforms[i] );
        rootState->addUniform( m_samplerUniforms[i] );
        rootState->addUniform( m_cmapUniforms[i] );
    }

    m_showCompleteUniform = osg::ref_ptr<osg::Uniform>( new osg::Uniform( "showComplete", 0 ) );

    rootState->addUniform( m_showCompleteUniform );
}

void WMOverlayAtlas::notifyTextureChange()
{
    m_textureChanged = true;
    m_dirty = true;
}

std::vector< std::string > WMOverlayAtlas::readFile( const std::string fileName )
{
    std::ifstream ifs( fileName.c_str(), std::ifstream::in );

    std::vector< std::string > lines;

    std::string line;

    if ( ifs.is_open() )
    {
        debugLog() << "trying to load " << fileName;
        debugLog() << "file exists";
    }
    else
    {
        debugLog() << "trying to load " << fileName;
        debugLog() << "file doesn\'t exist";
        ifs.close();
        return lines;
    }

    while ( !ifs.eof() )
    {
        getline( ifs, line );

        lines.push_back( std::string( line ) );
    }

    ifs.close();

    return lines;
}

bool WMOverlayAtlas::loadAtlas( boost::filesystem::path path )
{
    std::vector<std::string> lines;

    boost::filesystem::path sliceDir( path );
    //debugLog() << path.file_string().c_str();
    std::string dirString = sliceDir.remove_filename().file_string();
    //debugLog() << dirString;

    bool parseError = false;

    debugLog() << "start parsing meta file...";

    lines = readFile( path.file_string().c_str() );

    if ( lines.size() == 0 )
    {
        debugLog() << "empty file";
        return false;
    }
    size_t i = 0;
    if ( lines[i] != "openwalnut overlay atlas meta file" )
    {
        debugLog() << "not an atlas meta file";
        return false;
    }
    ++i;

    boost::shared_ptr<WProgressCombiner> newProgress = boost::shared_ptr<WProgressCombiner>( new WProgressCombiner() );

    boost::shared_ptr<WProgress>pro = boost::shared_ptr<WProgress>( new WProgress( "dummy", lines.size() ) );

    m_progress->addSubProgress( pro );

    while ( i < lines.size() )
    {
        std::string currentLine = lines[i];

        //debugLog() << currentLine;

        std::vector< std::string >lineVec;
        boost::regex reg( " " );
        boost::sregex_token_iterator it( lines[i].begin(), lines[i].end(), reg, -1 );
        boost::sregex_token_iterator end;
        while ( it != end )
        {
            lineVec.push_back( *it++ );
        }

        if ( lineVec.size() != 7 )
        {
            parseError = true;
            break;
        }

        std::string slicePath = dirString + lineVec[0];

        //debugLog() << slicePath;

        WOASliceOrientation so = OA_SAGITTAL;

        if ( lineVec[1] == "a" )
        {
            so = OA_AXIAL;
        }
        else if ( lineVec[1] == "c" )
        {
            so = OA_CORONAL;
        }

        float pos    = s2f( lineVec[2] );
        float left   = s2f( lineVec[3] );
        float bottom = s2f( lineVec[4] );
        float right  = s2f( lineVec[5] );
        float top    = s2f( lineVec[6] );

        WAtlasSlice as( slicePath, so, pos, left, bottom, right, top );

        if ( so == OA_AXIAL )
        {
            m_axialSlices.push_back( as );
        }
        else if ( so == OA_CORONAL )
        {
            m_coronalSlices.push_back( as );
        }
        else
        {
            m_sagittalSlices.push_back( as );
        }
        ++*pro;
        ++i;
    }

    pro->finish();

    if ( parseError )
    {
        debugLog() << "parse errors";
        return false;
    }
    debugLog() << "no errors";
    debugLog() << m_coronalSlices.size() << " coronal slices";

    return true;
}

void WMOverlayAtlas::toggleManipulators()
{
    if ( m_showManipulators->get( true ) )
    {
        m_s0->unhide();
        m_s1->unhide();
        m_s2->unhide();
        m_s3->unhide();
        m_s4->unhide();
    }
    else
    {
        m_s0->hide();
        m_s1->hide();
        m_s2->hide();
        m_s3->hide();
        m_s4->hide();
    }
}

void WMOverlayAtlas::updateCallback()
{
    if ( isDirty() )
    {
        updatePlane();
        updateTextures();
    }
}
