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

#include "../../kernel/WKernel.h"

#include "../../dataHandler/WDataSet.h"
#include "../../dataHandler/WDataHandler.h"
#include "../../dataHandler/WDataSetSingle.h"
#include "../../dataHandler/WDataSetScalar.h"
#include "../../dataHandler/WDataTexture3D.h"
#include "../../dataHandler/WGridRegular3D.h"
#include "../../dataHandler/WSubject.h"
#include "../../dataHandler/WValueSet.h"
#include "../../graphicsEngine/WShader.h"

#include "../../graphicsEngine/WGEUtils.h"

#include "WMArbitraryPlane.h"
#include "arbitraryPlane.xpm"

// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMArbitraryPlane )

WMArbitraryPlane::WMArbitraryPlane():
    WModule(),
    m_dirty( false ),
    m_textureChanged( true )
{
    // initialize members
}

WMArbitraryPlane::~WMArbitraryPlane()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMArbitraryPlane::factory() const
{
    // See "src/modules/template/" for an extensively documented example.
    return boost::shared_ptr< WModule >( new WMArbitraryPlane() );
}

const char** WMArbitraryPlane::getXPMIcon() const
{
    return arbitraryPlane_xpm;
}

const std::string WMArbitraryPlane::getName() const
{
    // Specify your module name here. This name must be UNIQUE!
    return "Arbitrary Plane";
}

const std::string WMArbitraryPlane::getDescription() const
{
    return "Modul draws an arbitrarily moveable plane through the brain. That rhymes.";
}

void WMArbitraryPlane::connectors()
{
    // Put the code for your connectors here. See "src/modules/template/" for an extensively documented example.

    // call WModules initialization
    WModule::connectors();
}

void WMArbitraryPlane::properties()
{
    // Initialize the properties
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    m_showComplete = m_properties->addProperty( "Show complete", "Slice should be drawn complete even if the texture value is zero.",
            false, m_propCondition );
    m_showManipulators = m_properties->addProperty( "Show manipulators", "Hide/Show manipulators.", true, m_propCondition );

    m_attach2Crosshair = m_properties->addProperty( "Attach to crosshair", "Attach to Crosshair", false, m_propCondition );

    m_buttonReset2Axial = m_properties->addProperty( "Axial", "Resets and aligns the plane", WPVBaseTypes::PV_TRIGGER_READY, m_propCondition  );
    m_buttonReset2Coronal = m_properties->addProperty( "Coronal", "Resets and aligns the plane", WPVBaseTypes::PV_TRIGGER_READY, m_propCondition  );
    m_buttonReset2Sagittal = m_properties->addProperty( "Sagittal", "Resets and aligns the plane", WPVBaseTypes::PV_TRIGGER_READY, m_propCondition  );
}

void WMArbitraryPlane::moduleMain()
{
    m_shader = osg::ref_ptr< WShader > ( new WShader( "WMArbitraryPlane", m_localPath ) );

    initPlane();

    updatePlane();

    osg::StateSet* rootState = m_rootNode->getOrCreateStateSet();

    initUniforms( rootState );

    updateTextures();

    m_shader->apply( m_rootNode );


    boost::signals2::connection con = WDataHandler::getDefaultSubject()->getChangeCondition()->subscribeSignal(
            boost::bind( &WMArbitraryPlane::notifyTextureChange, this ) );

    ready();

    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_propCondition );
    m_moduleState.add( m_active->getUpdateCondition() );

    while ( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        if ( m_showComplete->changed() )
        {
            m_dirty = true;
        }

        if ( m_active->changed() )
        {
            if ( m_active->get() && m_showManipulators->get() )
            {
                m_s0->unhide();
                m_s1->unhide();
                m_s2->unhide();
            }
            else
            {
                m_s0->hide();
                m_s1->hide();
                m_s2->hide();
            }
        }

        if ( m_showManipulators->changed() )
        {
            if ( m_showManipulators->get( true ) )
            {
                if ( m_active->get() )
                {
                    m_s0->unhide();
                    m_s1->unhide();
                    m_s2->unhide();
                }
            }
            else
            {
                m_s0->hide();
                m_s1->hide();
                m_s2->hide();
            }
        }

        if ( m_buttonReset2Axial->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
        {
            wmath::WPosition center = WKernel::getRunningKernel()->getSelectionManager()->getCrosshair()->getPosition();
            m_s0->setPosition( center );
            m_s1->setPosition( wmath::WPosition( center[0] - 100, center[1], center[2] ) );
            m_s2->setPosition( wmath::WPosition( center[0], center[1] - 100, center[2] ) );
            m_buttonReset2Axial->set( WPVBaseTypes::PV_TRIGGER_READY, false );
            m_dirty = true;
        }

        if ( m_buttonReset2Coronal->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
        {
            wmath::WPosition center = WKernel::getRunningKernel()->getSelectionManager()->getCrosshair()->getPosition();
            m_s0->setPosition( center );
            m_s1->setPosition( wmath::WPosition( center[0] - 100, center[1], center[2] ) );
            m_s2->setPosition( wmath::WPosition( center[0], center[1], center[2] - 100 ) );
            m_buttonReset2Coronal->set( WPVBaseTypes::PV_TRIGGER_READY, false );
            m_dirty = true;
        }

        if ( m_buttonReset2Sagittal->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
        {
            wmath::WPosition center = WKernel::getRunningKernel()->getSelectionManager()->getCrosshair()->getPosition();
            m_s0->setPosition( center );
            m_s1->setPosition( wmath::WPosition( center[0], center[1], center[2] - 100 ) );
            m_s2->setPosition( wmath::WPosition( center[0], center[1] - 100, center[2] ) );
            m_buttonReset2Sagittal->set( WPVBaseTypes::PV_TRIGGER_READY, false );
            m_dirty = true;
        }

        if ( m_shutdownFlag() )
        {
            break;
        }
    }
    con.disconnect();

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode );
}

void WMArbitraryPlane::initPlane()
{
    m_rootNode = new WGEManagedGroupNode( m_active );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_rootNode );
    m_geode = new osg::Geode();
    m_geode->setName( "_arbitraryPlane" );
    m_rootNode->addUpdateCallback( new SafeUpdateCallback( this ) );
    m_rootNode->insert( m_geode );

    wmath::WPosition center = WKernel::getRunningKernel()->getSelectionManager()->getCrosshair()->getPosition();
    m_p0 = wmath::WPosition( center );
    m_p1 = wmath::WPosition( center[0] - 100, center[1], center[2] );
    m_p2 = wmath::WPosition( center[0], center[1] - 100, center[2] );

    m_s0 = boost::shared_ptr<WROISphere>( new WROISphere( m_p0, 2.5 ) );
    m_s1 = boost::shared_ptr<WROISphere>( new WROISphere( m_p1, 2.5 ) );
    m_s1->setLockY();
    m_s2 = boost::shared_ptr<WROISphere>( new WROISphere( m_p2, 2.5 ) );
    m_s2->setLockX();

    m_s0->getSignalIsModified()->connect( boost::bind( &WMArbitraryPlane::setDirty, this ) );
    m_s1->getSignalIsModified()->connect( boost::bind( &WMArbitraryPlane::setDirty, this ) );
    m_s2->getSignalIsModified()->connect( boost::bind( &WMArbitraryPlane::setDirty, this ) );
}

void WMArbitraryPlane::updatePlane()
{
    m_geode->removeDrawables( 0, 1 );

    if ( m_attach2Crosshair->get() )
    {
        m_s0->setPosition( WKernel::getRunningKernel()->getSelectionManager()->getCrosshair()->getPosition() );
    }

    wmath::WPosition p0 = m_s0->getPosition();

    if ( p0 != m_p0 )
    {
        wmath::WVector3D offset = p0 - m_p0;
        m_p0 = p0;
        m_s1->setPosition( m_s1->getPosition() + offset );
        m_s2->setPosition( m_s2->getPosition() + offset );
    }
    wmath::WPosition p1 = m_s1->getPosition();
    wmath::WPosition p2 = m_s2->getPosition();

    wmath::WPosition v0( p1[0]                  , p2[1]                 , p0[2] - ( p0[2] - p1[2] ) - ( p0[2] - p2[2] ) );
    wmath::WPosition v1( p1[0]                  , p0[1] + p0[1] - p2[1] , p0[2] - ( p0[2] - p1[2] ) + ( p0[2] - p2[2] ) );
    wmath::WPosition v2( p0[0] + p0[0] - p1[0]  , p0[1] + p0[1] - p2[1] , p0[2] + ( p0[2] - p1[2] ) + ( p0[2] - p2[2] ) );
    wmath::WPosition v3( p0[0] + p0[0] - p1[0]  , p2[1]                 , p0[2] + ( p0[2] - p1[2] ) - ( p0[2] - p2[2] ) );

    osg::ref_ptr<osg::Geometry> planeGeometry = osg::ref_ptr<osg::Geometry>( new osg::Geometry() );
    osg::Vec3Array* planeVertices = new osg::Vec3Array;

    planeVertices->push_back( wge::wv3D2ov3( v0 ) );
    planeVertices->push_back( wge::wv3D2ov3( v1 ) );
    planeVertices->push_back( wge::wv3D2ov3( v2 ) );
    planeVertices->push_back( wge::wv3D2ov3( v3 ) );

    planeGeometry->setVertexArray( planeVertices );
    osg::DrawElementsUInt* quad = new osg::DrawElementsUInt( osg::PrimitiveSet::QUADS, 0 );
    quad->push_back( 3 );
    quad->push_back( 2 );
    quad->push_back( 1 );
    quad->push_back( 0 );
    planeGeometry->addPrimitiveSet( quad );

    // grab a list of data textures
    std::vector< boost::shared_ptr< WDataTexture3D > > tex = WDataHandler::getDefaultSubject()->getDataTextures( true );

    int c = 0;
    //////////////////////////////////////////////////////////////////////////////////////////////////
    if ( WKernel::getRunningKernel()->getSelectionManager()->getUseTexture() )
    {
        boost::shared_ptr< WGridRegular3D > grid = WKernel::getRunningKernel()->getSelectionManager()->getGrid();
        osg::Vec3Array* texCoords = new osg::Vec3Array;

        texCoords->push_back( wge::wv3D2ov3( grid->worldCoordToTexCoord( v0 ) ) );
        texCoords->push_back( wge::wv3D2ov3( grid->worldCoordToTexCoord( v1 ) ) );
        texCoords->push_back( wge::wv3D2ov3( grid->worldCoordToTexCoord( v2 ) ) );
        texCoords->push_back( wge::wv3D2ov3( grid->worldCoordToTexCoord( v3 ) ) );

        planeGeometry->setTexCoordArray( c, texCoords );
        ++c;
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    for ( std::vector< boost::shared_ptr< WDataTexture3D > >::const_iterator iter = tex.begin(); iter != tex.end(); ++iter )
    {
        boost::shared_ptr< WGridRegular3D > grid = ( *iter )->getGrid();

        osg::Vec3Array* texCoords = new osg::Vec3Array;

        texCoords->push_back( wge::wv3D2ov3( grid->worldCoordToTexCoord( v0 ) ) );
        texCoords->push_back( wge::wv3D2ov3( grid->worldCoordToTexCoord( v1 ) ) );
        texCoords->push_back( wge::wv3D2ov3( grid->worldCoordToTexCoord( v2 ) ) );
        texCoords->push_back( wge::wv3D2ov3( grid->worldCoordToTexCoord( v3 ) ) );

        planeGeometry->setTexCoordArray( c, texCoords );
        ++c;
    }

    m_geode->addDrawable( planeGeometry );

    m_dirty = false;
}

void WMArbitraryPlane::SafeUpdateCallback::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    wmath::WPosition ch = WKernel::getRunningKernel()->getSelectionManager()->getCrosshair()->getPosition();
    wmath::WPosition cho = m_module->getCenterPosition();
    if ( ch[0] != cho[0] || ch[1] != cho[1] || ch[2] != cho[2] )
    {
        m_module->setDirty();
    }

    if ( m_module->isDirty() )
    {
        m_module->updatePlane();
        m_module->updateTextures();
    }
    traverse( node, nv );
}

void WMArbitraryPlane::setDirty()
{
    m_dirty = true;
}

bool WMArbitraryPlane::isDirty()
{
    return m_dirty;
}

void WMArbitraryPlane::notifyTextureChange()
{
    m_textureChanged = true;
    m_dirty = true;
}


void WMArbitraryPlane::updateTextures()
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
            for ( int i = 0; i < m_maxNumberOfTextures; ++i )
            {
                m_typeUniforms[i]->set( 0 );
            }

            // for each texture -> apply
            int c = 0;
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
                if( c == m_maxNumberOfTextures )
                {
                    break;
                }
            }
        }
    }

    m_showCompleteUniform->set( m_showComplete->get() );
}

void WMArbitraryPlane::initUniforms( osg::StateSet* rootState )
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

    for ( int i = 0; i < m_maxNumberOfTextures; ++i )
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

wmath::WPosition WMArbitraryPlane::getCenterPosition()
{
    return m_s0->getPosition();
}
