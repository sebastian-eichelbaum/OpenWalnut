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

#include <osg/Geode>
#include <osg/Geometry>

#include "../../dataHandler/WDataSetScalar.h"
#include "../../kernel/WKernel.h"
#include "WMCoordinateSystem.h"
#include "coordinateSystem.xpm"

WMCoordinateSystem::WMCoordinateSystem() :
    WModule()
{
}

WMCoordinateSystem::~WMCoordinateSystem()
{
}

boost::shared_ptr< WModule > WMCoordinateSystem::factory() const
{
    return boost::shared_ptr< WMCoordinateSystem >( new WMCoordinateSystem() );
}

const char** WMCoordinateSystem::getXPMIcon() const
{
    return coordinateSystem_xpm;
}

void WMCoordinateSystem::moduleMain()
{
    // signal ready state
    ready();

    createGeometry();

    // Since the modules run in a separate thread: wait
    waitForStop();

    // clean up stuff
    // NOTE: ALWAYS remove your osg nodes!
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode );
}

const std::string WMCoordinateSystem::getName() const
{
    return "Coordinate System";
}

const std::string WMCoordinateSystem::getDescription() const
{
    return "This module displays coordinate systems as overlay withn the main 3D view.";
}

void WMCoordinateSystem::properties()
{
    m_dataSetAddedProp = m_properties->addProperty( "dataSetAdded", "", false, true );

    m_axialPosProp = m_properties->addProperty( "axialPos", "", 80 );
    m_coronalPosProp = m_properties->addProperty( "coronalPos", "", 100 );
    m_sagittalPosProp = m_properties->addProperty( "sagittalPos", "", 80 );

    // initialize the properties with a certain standard set
    // those properties will be updatet as soon as the first dataset is looaded
    m_zeroXProp = m_properties->addProperty( "zeroX", "", 80.0 );
    m_zeroYProp = m_properties->addProperty( "zeroY", "", 100.0 );
    m_zeroZProp = m_properties->addProperty( "zeroZ", "", 80.0 );

    m_fltXProp = m_properties->addProperty( "fltX", "", 0.0 );
    m_fltYProp = m_properties->addProperty( "fltY", "", 0.0 );
    m_fltZProp = m_properties->addProperty( "fltZ", "", 0.0 );

    m_brbXProp = m_properties->addProperty( "brbX", "", 160.0 );
    m_brbYProp = m_properties->addProperty( "brbY", "", 200.0 );
    m_brbZProp = m_properties->addProperty( "brbZ", "", 160.0 );
}

void WMCoordinateSystem::createGeometry()
{
    m_rootNode = osg::ref_ptr< WGEGroupNode >( new WGEGroupNode() );

    m_boxNode = osg::ref_ptr<osg::Geode>( new osg::Geode() );
    m_boxNode->addDrawable( createGeometryNode() );

    m_rootNode->insert( m_boxNode );

    // this is done during the first call of updateGeometry()

    //float zeroZ = m_properties->getValue<float>( "axialPos" );
    //float zeroY = m_properties->getValue<float>( "coronalPos" );
    //float zeroX = m_properties->getValue<float>( "sagittalPos" );

    //float fltX = m_properties->getValue<float>( "fltX" );
    //float fltY = m_properties->getValue<float>( "fltY" );
    //float fltZ = m_properties->getValue<float>( "fltZ" );

    //float brbX = m_properties->getValue<float>( "brbX" );
    //float brbY = m_properties->getValue<float>( "brbY" );
    //float brbZ = m_properties->getValue<float>( "brbZ" );

    //osg::ref_ptr<WRulerOrtho>ruler1 = osg::ref_ptr<WRulerOrtho>( new WRulerOrtho() );
    //ruler1->create( osg::Vec3( fltX, zeroY, fltZ ), brbX, RULER_ALONG_X_AXIS_SCALE_Y );

    //osg::ref_ptr<WRulerOrtho>ruler2 = osg::ref_ptr<WRulerOrtho>( new WRulerOrtho() );
    //ruler2->create( osg::Vec3( zeroX, fltY, fltZ ), brbY, RULER_ALONG_Y_AXIS_SCALE_X );

    //ruler1->setName( std::string( "ruler1" ) );
    //ruler2->setName( std::string( "ruler2" ) );
    //m_rootNode->insert( ruler1 );
    //m_rootNode->insert( ruler2 );

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_rootNode );

    // osg::StateSet* rootState = m_rootNode->getOrCreateStateSet();

    m_rootNode->setUserData( this );
    m_rootNode->addUpdateCallback( new coordinateNodeCallback );

    if ( m_active->get() )
    {
        m_rootNode->setNodeMask( 0xFFFFFFFF );
    }
    else
    {
        m_rootNode->setNodeMask( 0x0 );
    }
}

void WMCoordinateSystem::updateGeometry()
{
    if ( !m_dataSetAddedProp->get() || !WKernel::getRunningKernel()->getGui()->isInitialized()() )
    {
        return;
    }

    boost::shared_lock< boost::shared_mutex > slock;
    slock = boost::shared_lock< boost::shared_mutex >( m_updateLock );
    // *******************************************************************************************************
    m_dataSetAddedProp->set( false );

    findBoundingBox();

    //float zeroZ = m_axialPosProp->get();
    float zeroY = m_coronalPosProp->get();
    float zeroX = m_sagittalPosProp->get();

    float fltX = m_fltXProp->get();
    float fltY = m_fltYProp->get();
    float fltZ = m_fltZProp->get();

    float brbX = m_brbXProp->get();
    float brbY = m_brbYProp->get();
    //float brbZ = m_brbZProp->get();


    osg::ref_ptr<osg::Drawable> old = osg::ref_ptr<osg::Drawable>( m_boxNode->getDrawable( 0 ) );
    m_boxNode->replaceDrawable( old, createGeometryNode() );

    for ( size_t i = 0; i < m_rootNode->getNumChildren(); ++i)
    {
        if ( m_rootNode->getChild( i )->getName() == "ruler1" )
        {
            m_rootNode->removeChild( i, 1 );
        }
        if ( m_rootNode->getChild( i )->getName() == "ruler2" )
        {
            m_rootNode->removeChild( i, 1 );
        }
    }
    osg::ref_ptr<WRulerOrtho>ruler1 = osg::ref_ptr<WRulerOrtho>( new WRulerOrtho() );
    ruler1->create( osg::Vec3( fltX, zeroY, fltZ ), brbX - fltX, RULER_ALONG_X_AXIS_SCALE_Y );

    osg::ref_ptr<WRulerOrtho>ruler2 = osg::ref_ptr<WRulerOrtho>( new WRulerOrtho() );
    ruler2->create( osg::Vec3( zeroX, fltY, fltZ ), brbY - fltY, RULER_ALONG_Y_AXIS_SCALE_X );

    ruler1->setName( std::string( "ruler1" ) );
    ruler2->setName( std::string( "ruler2" ) );
    m_rootNode->addChild( ruler1 );
    m_rootNode->addChild( ruler2 );
    // *******************************************************************************************************
    slock.unlock();
}

osg::ref_ptr<osg::Geometry> WMCoordinateSystem::createGeometryNode()
{
    float zeroZ = m_axialPosProp->get();
    float zeroY = m_coronalPosProp->get();
    float zeroX = m_sagittalPosProp->get();

    float fltX = m_fltXProp->get();
    float fltY = m_fltYProp->get();
    float fltZ = m_fltZProp->get();

    float brbX = m_brbXProp->get();
    float brbY = m_brbYProp->get();
    float brbZ = m_brbZProp->get();


    osg::ref_ptr<osg::Geometry> geometry = osg::ref_ptr<osg::Geometry>( new osg::Geometry() );


    osg::Vec3Array* vertices = new osg::Vec3Array;

    vertices->push_back( osg::Vec3( zeroX, zeroY, zeroZ ) );

    vertices->push_back( osg::Vec3( fltX, fltY, fltZ ) );
    vertices->push_back( osg::Vec3( fltX, brbY, fltZ ) );
    vertices->push_back( osg::Vec3( fltX, brbY, brbZ ) );
    vertices->push_back( osg::Vec3( fltX, fltY, brbZ ) );

    vertices->push_back( osg::Vec3( brbX, fltY, fltZ ) );
    vertices->push_back( osg::Vec3( brbX, brbY, fltZ ) );
    vertices->push_back( osg::Vec3( brbX, brbY, brbZ ) );
    vertices->push_back( osg::Vec3( brbX, fltY, brbZ ) );

    vertices->push_back( osg::Vec3( zeroX, zeroY, fltZ ) );
    vertices->push_back( osg::Vec3( zeroX, zeroY, brbZ ) );
    vertices->push_back( osg::Vec3( zeroX, fltY, zeroZ ) );
    vertices->push_back( osg::Vec3( zeroX, brbY, zeroZ ) );
    vertices->push_back( osg::Vec3( fltX, zeroY, zeroZ ) );
    vertices->push_back( osg::Vec3( brbX, zeroY, zeroZ ) );

    geometry->setVertexArray( vertices );

    // TODO(schurade): Hi, here is math, I really don't now what this is for, so maybe you can give "data" and "rawData" a better
    // name? Thanks.
    unsigned int rawData[] = { 1, 2, 2, 3, 3, 4, 4, 1,
                               5, 6, 6, 7, 7, 8, 8, 5,
                               1, 5, 2, 6, 3, 7, 4, 8,
                               9, 10, 11, 12, 13, 14 }; // NOLINT
    std::vector< unsigned int > data( rawData, rawData + sizeof( rawData ) / sizeof( unsigned int ) );
    osg::DrawElementsUInt* lines = new osg::DrawElementsUInt( osg::PrimitiveSet::LINES, data.begin(), data.end() );

    geometry->addPrimitiveSet( lines );

    // disable light for this geode as lines can't be lit properly
    osg::StateSet* state = geometry->getOrCreateStateSet();
    state->setMode( GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );

    return geometry;
}

void WMCoordinateSystem::findBoundingBox()
{
    // TODO(schurade): getDataSetList is obsolete.
    // Use
    //
    // std::vector< boost::shared_ptr< WDataTexture3D > > tex = WDataHandler::getDefaultSubject()->getDataTextures( true )
    // for ( std::vector< boost::shared_ptr< WDataTexture3D > >::const_iterator iter = tex.begin(); iter != tex.end(); ++iter )
    // {
        // std::pair< wmath::WPosition, wmath::WPosition > bb = ( *iter )->getGrid()->getBoundingBox();
        // ...
    // }

    std::vector< boost::shared_ptr< WDataSet > > dsl; // = WKernel::getRunningKernel()->getGui()->getDataSetList( 0, true );

    if ( dsl.size() > 0 )
    {
        boost::shared_ptr< WDataSetScalar > ds = boost::shared_dynamic_cast< WDataSetScalar >( dsl[0] );

        if ( ds->getValueSet()->getDataType() != 2 )
        {
            return;
        }

        boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( ds->getGrid() );

        for ( size_t x = 0; x < grid->getNbCoordsX(); ++x )
        {
            int count = 0;
            for ( size_t y = 0; y < grid->getNbCoordsY(); ++y )
            {
                for ( size_t z = 0; z < grid->getNbCoordsZ(); ++z )
                {
                    unsigned char v = ds->getValueAt< unsigned char > ( x, y, z );
                    if ( v > 0 )
                    {
                        ++count;
                    }
                }
            }
            if ( count > 5 )
            {
                m_fltXProp->set( static_cast<float>( x ) );
                break;
            }
        }
        for ( int x = grid->getNbCoordsX() - 1; x > -1; --x )
        {
            int count = 0;
            for ( size_t y = 0; y < grid->getNbCoordsY(); ++y )
            {
                for ( size_t z = 0; z < grid->getNbCoordsZ(); ++z )
                {
                    unsigned char v = ds->getValueAt< unsigned char > ( x, y, z );
                    if ( v > 0 )
                    {
                        ++count;
                    }
                }
            }
            if ( count > 5 )
            {
                m_brbXProp->set( static_cast<float>( x ) );
                break;
            }
        }

        for ( size_t y = 0; y < grid->getNbCoordsY(); ++y )
        {
            int count = 0;
            for ( size_t x = 0; x < grid->getNbCoordsX(); ++x )
            {
                for ( size_t z = 0; z < grid->getNbCoordsZ(); ++z )
                {
                    unsigned char v = ds->getValueAt< unsigned char > ( x, y, z );
                    if ( v > 0 )
                    {
                        ++count;
                    }
                }
            }
            if ( count > 5 )
            {
                m_fltYProp->set( static_cast<float>( y ) );
                break;
            }
        }
        for ( int y = grid->getNbCoordsY() - 1; y > -1; --y )
        {
            int count = 0;
            for ( size_t x = 0; x < grid->getNbCoordsX(); ++x )
            {
                for ( size_t z = 0; z < grid->getNbCoordsZ(); ++z )
                {
                    unsigned char v = ds->getValueAt< unsigned char > ( x, y, z );
                    if ( v > 0 )
                    {
                        ++count;
                    }
                }
            }
            if ( count > 5 )
            {
                m_brbYProp->set( static_cast<float>( y ) );
                break;
            }
        }



        for ( size_t z = 0; z < grid->getNbCoordsZ(); ++z )
        {
            int count = 0;
            for ( size_t x = 0; x < grid->getNbCoordsX(); ++x )
            {
                for ( size_t y = 0; y < grid->getNbCoordsY(); ++y )
                {
                    unsigned char v = ds->getValueAt< unsigned char > ( x, y, z );
                    if ( v > 0 )
                    {
                        ++count;
                    }
                }
            }
            if ( count > 5 )
            {
                m_fltZProp->set( static_cast<float>( z ) );
                break;
            }
        }
        for ( int z = grid->getNbCoordsZ() - 1; z > -1; --z )
        {
            int count = 0;
            for ( size_t x = 0; x < grid->getNbCoordsX(); ++x )
            {
                for ( size_t y = 0; y < grid->getNbCoordsY(); ++y )
                {
                    unsigned char v = ds->getValueAt< unsigned char > ( x, y, z );
                    if ( v > 0 )
                    {
                        ++count;
                    }
                }
            }
            if ( count > 5 )
            {
                m_brbZProp->set( static_cast<float>( z ) );
                break;
            }
        }
    }
}

void WMCoordinateSystem::activate()
{
    // Activate/Deactivate the coordinate system
    if ( m_active->get() )
    {
        m_rootNode->setNodeMask( 0xFFFFFFFF );
    }
    else
    {
        m_rootNode->setNodeMask( 0x0 );
    }

    // Always call WModule's activate!
    WModule::activate();
}
