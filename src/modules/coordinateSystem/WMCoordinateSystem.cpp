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

#include "../../kernel/WKernel.h"
#include "../data/WMData.h"
#include "WMCoordinateSystem.h"

WMCoordinateSystem::WMCoordinateSystem() :
    WModule()
{
    properties();
}

WMCoordinateSystem::~WMCoordinateSystem()
{
}

boost::shared_ptr< WModule > WMCoordinateSystem::factory() const
{
    return boost::shared_ptr< WMCoordinateSystem >( new WMCoordinateSystem() );
}

void WMCoordinateSystem::moduleMain()
{
    createGeometry();

    // Since the modules run in a separate thread: such loops are possible
    while ( !m_FinishRequested )
    {
        // do fancy stuff
        sleep( 1 );
    }

    // clean up stuff
}

const std::string WMCoordinateSystem::getName() const
{
    return "Coordinate System Module";
}

const std::string WMCoordinateSystem::getDescription() const
{
    return "This module displays coordinate systems as overlay withn the main 3D view.";
}

void WMCoordinateSystem::connectToGui()
{
    WKernel::getRunningKernel()->getGui()->connectProperties( m_properties );
}

void WMCoordinateSystem::properties()
{
    m_properties->addBool( "textureChanged", false );

    m_properties->addInt( "type", 1 );
    // initialize the properties with a certain standard set
    // those properties will be updatet as soon as the first dataset is looaded
    m_properties->addFloat( "zeroX", 80.0 );
    m_properties->addFloat( "zeroY", 100.0 );
    m_properties->addFloat( "zeroZ", 80.0 );

    m_properties->addFloat( "fltX", 0.0 );
    m_properties->addFloat( "fltY", 0.0 );
    m_properties->addFloat( "fltZ", 0.0 );

    m_properties->addFloat( "brbX", 160.0 );
    m_properties->addFloat( "brbY", 200.0 );
    m_properties->addFloat( "brbZ", 160.0 );
}

void WMCoordinateSystem::createGeometry()
{
    float zeroX = m_properties->getValue<float>( "zeroX" );
    float zeroY = m_properties->getValue<float>( "zeroY" );
    float zeroZ = m_properties->getValue<float>( "zeroZ" );

    float fltX = m_properties->getValue<float>( "fltX" );
    float fltY = m_properties->getValue<float>( "fltY" );
    float fltZ = m_properties->getValue<float>( "fltZ" );

    float brbX = m_properties->getValue<float>( "brbX" );
    float brbY = m_properties->getValue<float>( "brbY" );
    float brbZ = m_properties->getValue<float>( "brbZ" );

    m_rootNode = osg::ref_ptr<osg::Geode>( new osg::Geode() );
    osg::ref_ptr<osg::Geometry> geometry = osg::ref_ptr<osg::Geometry>( new osg::Geometry() );
    m_rootNode->addDrawable( geometry );

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

    osg::DrawElementsUInt* lines = new osg::DrawElementsUInt( osg::PrimitiveSet::LINES, 0 );

    lines->push_back( 1 );
    lines->push_back( 2 );
    lines->push_back( 2 );
    lines->push_back( 3 );
    lines->push_back( 3 );
    lines->push_back( 4 );
    lines->push_back( 4 );
    lines->push_back( 1 );

    lines->push_back( 5 );
    lines->push_back( 6 );
    lines->push_back( 6 );
    lines->push_back( 7 );
    lines->push_back( 7 );
    lines->push_back( 8 );
    lines->push_back( 8 );
    lines->push_back( 5 );

    lines->push_back( 1 );
    lines->push_back( 5 );
    lines->push_back( 2 );
    lines->push_back( 6 );
    lines->push_back( 3 );
    lines->push_back( 7 );
    lines->push_back( 4 );
    lines->push_back( 8 );

    lines->push_back( 9 );
    lines->push_back( 10 );
    lines->push_back( 11 );
    lines->push_back( 12 );
    lines->push_back( 13 );
    lines->push_back( 14 );


    geometry->addPrimitiveSet( lines );

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->addChild( m_rootNode );

    // osg::StateSet* rootState = m_rootNode->getOrCreateStateSet();

    m_rootNode->setUpdateCallback( new coordinateNodeCallback( boost::shared_dynamic_cast<WMCoordinateSystem>( shared_from_this() ) ) );
}

void WMCoordinateSystem::updateGeometry()
{
    boost::shared_lock< boost::shared_mutex > slock;
    slock = boost::shared_lock< boost::shared_mutex >( m_updateLock );
    // *******************************************************************************************************
    if ( !m_properties->getValue< bool > ( "textureChanged" ) )
    {
        return;
    }
    if ( m_properties->getValue< bool > ( "textureChanged" ) && WKernel::getRunningKernel()->getGui()->isInitalized() )
    {
        m_properties->setValue( "textureChanged", false );
        std::vector< boost::shared_ptr< WModule > > datasetList = WKernel::getRunningKernel()->getGui()->getDataSetList( 0 );

        if ( datasetList.size() > 0 )
        {
            boost::shared_ptr< WMData > module = boost::shared_dynamic_cast< WMData >( datasetList[0] );
            boost::shared_ptr< WDataSetSingle > ds = boost::shared_dynamic_cast< WDataSetSingle >( module->getDataSet() );

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
                    m_properties->setValue( "fltX", static_cast<float>( x ) );
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
                    m_properties->setValue( "brbX", static_cast<float>( x ) );
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
                    m_properties->setValue( "fltY", static_cast<float>( y ) );
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
                    m_properties->setValue( "brbY", static_cast<float>( y ) );
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
                    m_properties->setValue( "fltZ", static_cast<float>( z ) );
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
                    m_properties->setValue( "brbZ", static_cast<float>( z ) );
                    break;
                }
            }

            m_properties->setValue( "textureChanged", false );
        }
    }

    float zeroX = m_properties->getValue<float>( "zeroX" );
    float zeroY = m_properties->getValue<float>( "zeroY" );
    float zeroZ = m_properties->getValue<float>( "zeroZ" );

    float fltX = m_properties->getValue<float>( "fltX" );
    float fltY = m_properties->getValue<float>( "fltY" );
    float fltZ = m_properties->getValue<float>( "fltZ" );

    float brbX = m_properties->getValue<float>( "brbX" );
    float brbY = m_properties->getValue<float>( "brbY" );
    float brbZ = m_properties->getValue<float>( "brbZ" );

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

    osg::DrawElementsUInt* lines = new osg::DrawElementsUInt( osg::PrimitiveSet::LINES, 0 );

    lines->push_back( 1 );
    lines->push_back( 2 );
    lines->push_back( 2 );
    lines->push_back( 3 );
    lines->push_back( 3 );
    lines->push_back( 4 );
    lines->push_back( 4 );
    lines->push_back( 1 );

    lines->push_back( 5 );
    lines->push_back( 6 );
    lines->push_back( 6 );
    lines->push_back( 7 );
    lines->push_back( 7 );
    lines->push_back( 8 );
    lines->push_back( 8 );
    lines->push_back( 5 );

    lines->push_back( 1 );
    lines->push_back( 5 );
    lines->push_back( 2 );
    lines->push_back( 6 );
    lines->push_back( 3 );
    lines->push_back( 7 );
    lines->push_back( 4 );
    lines->push_back( 8 );

    lines->push_back( 9 );
    lines->push_back( 10 );
    lines->push_back( 11 );
    lines->push_back( 12 );
    lines->push_back( 13 );
    lines->push_back( 14 );

    geometry->addPrimitiveSet( lines );

    osg::ref_ptr<osg::Drawable> old = osg::ref_ptr<osg::Drawable>( m_rootNode->getDrawable( 0 ) );
    m_rootNode->replaceDrawable( old, geometry );

    // *******************************************************************************************************
    slock.unlock();
}
