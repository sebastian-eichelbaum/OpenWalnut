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

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <cmath>


#include "WMPrototypeBoxManipulation.h"

#include <boost/lexical_cast.hpp>

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/StateSet>
#include <osg/StateAttribute>
#include <osg/PolygonMode>
#include <osg/LightModel>
#include <osg/LineWidth>
#include <osgDB/WriteFile>

#include "../../utils/WStringUtils.h"
#include "../../math/WVector3D.h"
#include "../../math/WPosition.h"
#include "../../kernel/WKernel.h"
#include "../../graphicsEngine/WShader.h"

#include "../data/WMData.h"


WMPrototypeBoxManipulation::WMPrototypeBoxManipulation():
    WModule(),
    m_isPicked( false )
{
    // WARNING: initializing connectors inside the constructor will lead to an exception.
    // Implement WModule::initializeConnectors instead.
}

WMPrototypeBoxManipulation::~WMPrototypeBoxManipulation()
{
    // cleanup
    removeConnectors();
}

boost::shared_ptr< WModule > WMPrototypeBoxManipulation::factory() const
{
    return boost::shared_ptr< WModule >( new WMPrototypeBoxManipulation() );
}

const std::string WMPrototypeBoxManipulation::getName() const
{
    return "Box Manipulation";
}

const std::string WMPrototypeBoxManipulation::getDescription() const
{
    return "Just a prtotype for box manipulation";
}


void WMPrototypeBoxManipulation::moduleMain()
{
    // use the m_input "data changed" flag
    m_moduleState.add( m_input->getDataChangedCondition() );

    // connect updateGFX with picking
    boost::shared_ptr< WGEViewer > viewer = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "main" );
    assert( viewer );
    viewer->getPickHandler()->getPickSignal()->connect( boost::bind( &WMPrototypeBoxManipulation::updateGFX, this, _1 ) );

    // signal ready state
    ready();

    // loop until the module container requests the module to quit
    while ( !m_shutdownFlag() )
    {
        draw( wmath::WPosition( 30, 30, 30 ), wmath::WPosition( 70, 70, 70 ) );

        // this waits for m_moduleState to fire. By default, this is only the m_shutdownFlag condition.
        // NOTE: you can add your own conditions to m_moduleState using m_moduleState.add( ... )
        m_moduleState.wait();
    }
}

void WMPrototypeBoxManipulation::connectors()
{
    // initialize connectors
    m_input = boost::shared_ptr< WModuleInputData < WDataSetSingle  > >(
        new WModuleInputData< WDataSetSingle >( shared_from_this(),
                                                               "in", "Dummy in." )
        );

    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_input );

    // call WModules initialization
    WModule::connectors();
}

void WMPrototypeBoxManipulation::properties()
{
    m_properties->addBool( "textureChanged", false, true );
}

void WMPrototypeBoxManipulation::slotPropertyChanged( std::string propertyName )
{
    if( propertyName == "active" )
    {
        if ( m_properties->getValue<bool>( propertyName ) )
        {
            m_geode->setNodeMask( 0xFFFFFFFF );
        }
        else
        {
            m_geode->setNodeMask( 0x0 );
        }
    }
    else
    {
        std::cout << propertyName << std::endl;
        assert( 0 && "This property name is not supported by this function yet." );
    }
}

void buildFacesFromPoints( osg::DrawElementsUInt* surfaceElements )
{
    surfaceElements->push_back( 0 );
    surfaceElements->push_back( 2 );
    surfaceElements->push_back( 3 );
    surfaceElements->push_back( 1 );

    surfaceElements->push_back( 2 );
    surfaceElements->push_back( 6 );
    surfaceElements->push_back( 7 );
    surfaceElements->push_back( 3 );

    surfaceElements->push_back( 6 );
    surfaceElements->push_back( 4 );
    surfaceElements->push_back( 5 );
    surfaceElements->push_back( 7 );

    surfaceElements->push_back( 4 );
    surfaceElements->push_back( 0 );
    surfaceElements->push_back( 1 );
    surfaceElements->push_back( 5 );

    surfaceElements->push_back( 1 );
    surfaceElements->push_back( 3 );
    surfaceElements->push_back( 7 );
    surfaceElements->push_back( 5 );

    surfaceElements->push_back( 0 );
    surfaceElements->push_back( 4 );
    surfaceElements->push_back( 6 );
    surfaceElements->push_back( 2 );
}

void buildLinesFromPoints( osg::DrawElementsUInt* surfaceElements )
{
    surfaceElements->push_back( 0 );
    surfaceElements->push_back( 2 );
    surfaceElements->push_back( 2 );
    surfaceElements->push_back( 3 );
    surfaceElements->push_back( 3 );
    surfaceElements->push_back( 1 );
    surfaceElements->push_back( 1 );
    surfaceElements->push_back( 0 );

    surfaceElements->push_back( 6 );
    surfaceElements->push_back( 4 );
    surfaceElements->push_back( 4 );
    surfaceElements->push_back( 5 );
    surfaceElements->push_back( 5 );
    surfaceElements->push_back( 7 );
    surfaceElements->push_back( 7 );
    surfaceElements->push_back( 6 );

    surfaceElements->push_back( 2 );
    surfaceElements->push_back( 6 );
    surfaceElements->push_back( 7 );
    surfaceElements->push_back( 3 );


    surfaceElements->push_back( 4 );
    surfaceElements->push_back( 0 );
    surfaceElements->push_back( 1 );
    surfaceElements->push_back( 5 );
}

void setVertices( osg::Vec3Array* vertices, wmath::WPosition minPos, wmath::WPosition maxPos )
{
    vertices->push_back( osg::Vec3( minPos[0], minPos[1], minPos[2] ) );
    vertices->push_back( osg::Vec3( minPos[0], minPos[1], maxPos[2] ) );
    vertices->push_back( osg::Vec3( minPos[0], maxPos[1], minPos[2] ) );
    vertices->push_back( osg::Vec3( minPos[0], maxPos[1], maxPos[2] ) );
    vertices->push_back( osg::Vec3( maxPos[0], minPos[1], minPos[2] ) );
    vertices->push_back( osg::Vec3( maxPos[0], minPos[1], maxPos[2] ) );
    vertices->push_back( osg::Vec3( maxPos[0], maxPos[1], minPos[2] ) );
    vertices->push_back( osg::Vec3( maxPos[0], maxPos[1], maxPos[2] ) );
}

void WMPrototypeBoxManipulation::draw( wmath::WPosition minPos, wmath::WPosition maxPos )
{
    m_minPos = minPos;
    m_maxPos = maxPos;
    osg::Geometry* surfaceGeometry = new osg::Geometry();
    m_geode = new osg::Geode;
    m_geode->setName( "Box" );

    osg::Vec3Array* vertices = new osg::Vec3Array;
    setVertices( vertices, minPos, maxPos );
    surfaceGeometry->setVertexArray( vertices );

    osg::DrawElementsUInt* surfaceElements;
    surfaceElements = new osg::DrawElementsUInt( osg::PrimitiveSet::QUADS, 0 );
    buildFacesFromPoints( surfaceElements );

    osg::DrawElementsUInt* lineElements;
    lineElements = new osg::DrawElementsUInt( osg::PrimitiveSet::LINES, 0 );
    buildLinesFromPoints( lineElements );

    surfaceGeometry->addPrimitiveSet( surfaceElements );
    surfaceGeometry->addPrimitiveSet( lineElements );
    m_geode->addDrawable( surfaceGeometry );
    osg::StateSet* state = m_geode->getOrCreateStateSet();
    state->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

    osg::LineWidth* linewidth = new osg::LineWidth();
    linewidth->setWidth( 2.f );
    state->setAttributeAndModes( linewidth, osg::StateAttribute::ON );

    // ------------------------------------------------
    // colors
    osg::Vec4Array* colors = new osg::Vec4Array;

    colors->push_back( osg::Vec4( 1.f, 1.f, 1.f, 0.5f ) );
    surfaceGeometry->setColorArray( colors );
    surfaceGeometry->setColorBinding( osg::Geometry::BIND_OVERALL );

    osg::ref_ptr<osg::LightModel> lightModel = new osg::LightModel();
    lightModel->setTwoSided( true );
    state->setAttributeAndModes( lightModel.get(), osg::StateAttribute::ON );
    state->setMode( GL_BLEND, osg::StateAttribute::ON  );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->addChild( m_geode );

    debugLog() << "Intial draw " << std::endl;
}

wmath::WPosition getPosFromText( std::string posText )
{
    std::vector< std::string > coordText = string_utils::tokenize( posText );
    double x =  boost::lexical_cast< double >( coordText[0] );
    double y =  boost::lexical_cast< double >( coordText[1] );
    double z =  boost::lexical_cast< double >( coordText[2] );
    return wmath::WPosition( x, y, z );
}

void WMPrototypeBoxManipulation::updateGFX( std::string text )
{
    boost::shared_lock<boost::shared_mutex> slock;
    slock = boost::shared_lock<boost::shared_mutex>( m_updateLock );

    if( text.find( "Object ") != std::string::npos
        && text.find( "\"Box\"" ) != std::string::npos )
    {
        std::string posText = string_utils::tokenize( text, "()" )[1];
        wmath::WPosition newPos = getPosFromText( posText );
        if( m_isPicked )
        {
            wmath::WVector3D moveVec = m_pickedPosition - newPos;
            osg::Vec3Array* vertices = new osg::Vec3Array;
            m_minPos -= moveVec;
            m_maxPos -= moveVec;
            setVertices( vertices, m_minPos, m_maxPos );
            ((osg::Geometry*)(m_geode->getDrawable( 0 )))->setVertexArray( vertices );
        }
        else
        {
            osg::Vec4Array* colors = new osg::Vec4Array;
            colors->push_back( osg::Vec4( 1.f, .0f, .0f, 0.5f ) );
            ((osg::Geometry*)(m_geode->getDrawable( 0 )))->setColorArray( colors );
        }
        m_pickedPosition = newPos;
        m_isPicked = true;
    }
    if( m_isPicked && text.find( "unpick" ) != std::string::npos )
    {
        osg::Vec4Array* colors = new osg::Vec4Array;
        colors->push_back( osg::Vec4( 1.f, 1.f, 1.f, 0.5f ) );
        ((osg::Geometry*)(m_geode->getDrawable( 0 )))->setColorArray( colors );
        m_isPicked = false;
    }
    slock.unlock();
}

