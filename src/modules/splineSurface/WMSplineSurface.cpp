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

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <cmath>

#include "WMSplineSurface.xpm"
#include "core/common/WLimits.h"
#include "core/common/WAssert.h"

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Material>
#include <osg/StateSet>
#include <osg/StateAttribute>
#include <osg/PolygonMode>
#include <osg/LightModel>
#include <osgDB/WriteFile>

#include "core/common/WPathHelper.h"
#include "core/common/WProgress.h"
#include "core/common/math/linearAlgebra/WLinearAlgebra.h"
#include "core/common/math/WLinearAlgebraFunctions.h"
#include "core/dataHandler/WDataHandler.h"
#include "core/dataHandler/WSubject.h"
#include "core/graphicsEngine/WGEColormapping.h"
#include "core/graphicsEngine/WGEUtils.h"
#include "core/graphicsEngine/shaders/WGEShaderPropertyDefineOptions.h"
#include "core/kernel/WKernel.h"

#include "core/graphicsEngine/algorithms/WMarchingCubesAlgorithm.h"
#include "WSurface.h"
#include "WMSplineSurface.h"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMSplineSurface )

WMSplineSurface::WMSplineSurface() :
    WModule(), m_recompute( boost::shared_ptr< WCondition >( new WCondition() ) ),
            m_moduleNode( new WGEGroupNode() ), m_surfaceGeode( 0 )
{
    // WARNING: initializing connectors inside the constructor will lead to an exception.
    // Implement WModule::initializeConnectors instead.
}

WMSplineSurface::~WMSplineSurface()
{
    // cleanup
    removeConnectors();
}

boost::shared_ptr< WModule > WMSplineSurface::factory() const
{
    return boost::shared_ptr< WModule >( new WMSplineSurface() );
}

const char** WMSplineSurface::getXPMIcon() const
{
    return spline_surface_xpm;
}

const std::string WMSplineSurface::getName() const
{
    return "Spline Surface";
}

const std::string WMSplineSurface::getDescription() const
{
    return "<font color=\"#0000ff\"><b>[Unfinished Status]</b></font> This module implements the marching cubes"
        " algorithm with a consistent triangulation. It allows to compute isosurfaces"
        " for a given isovalue on data given on a grid only consisting of cubes. It yields"
        " the surface as triangle soup.";
}

void WMSplineSurface::moduleMain()
{
    // use the m_input "data changed" flag
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_recompute );

    // signal ready state
    ready();

    // loop until the module container requests the module to quit
    while( !m_shutdownFlag() )
    {
        // update isosurface
        debugLog() << "Computing surface ...";

        WSurface surf;

        //////////////////////////////////////////////////////////////////////////////////////////////////////
        std::vector< WVector3d > givenPoints;
        for( int y = 0; y < 11; ++y )
        {
            for( int z = 0; z < 11; ++z )
            {
                float pi = 3.14159265;
                float pi2 = pi * 2;
                float yy = pi2 * y / 10.;
                float zz = pi2 * z / 10.;

                WVector3d p;
                p[0] = 60. + sin( yy ) * 10 + cos( zz ) * 10;
                p[1] = y * 20.;
                p[2] = z * 16.;
                givenPoints.push_back( p );
            }
        }
        surf.setSupportPoints( givenPoints );
        //////////////////////////////////////////////////////////////////////////////////////////////////////

        surf.execute();
        m_triMesh = surf.getTriangleMesh();

        debugLog() << "Rendering surface ...";

        renderMesh();
        m_output->updateData( m_triMesh );

        debugLog() << "Done!";

        // this waits for m_moduleState to fire. By default, this is only the m_shutdownFlag condition.
        // NOTE: you can add your own conditions to m_moduleState using m_moduleState.add( ... )
        m_moduleState.wait();
    }

    WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getScene()->remove( m_moduleNode );
}

void WMSplineSurface::connectors()
{
    // TODO(someone): This connector should be used as soon as the module gets more functionality.
    // The surface will aligned to these tracts.
    typedef WModuleInputData< const WFiberCluster > InputType; // just an alias
    m_input = boost::shared_ptr< InputType >( new InputType( shared_from_this(), "Tracts", "A cluster of tracts." ) );
    addConnector( m_input );

    m_output = boost::shared_ptr< WModuleOutputData< WTriangleMesh > >( new WModuleOutputData< WTriangleMesh > ( shared_from_this(), "out",
            "The mesh representing the spline surface." ) );

    addConnector( m_output );

    // call WModules initialization
    WModule::connectors();
}

void WMSplineSurface::properties()
{
    m_opacityProp = m_properties->addProperty( "Opacity %", "Opaqueness of surface.", 100 );
    m_opacityProp->setMin( 0 );
    m_opacityProp->setMax( 100 );

    m_useTextureProp = m_properties->addProperty( "Use texture", "Use texturing of the surface?", true );

    m_surfaceColor = m_properties->addProperty( "Surface color", "Description.", WColor( 0.5, 0.5, 0.5, 1.0 ) );

    m_savePropGroup = m_properties->addPropertyGroup( "Save Surface", "" );
    m_saveTriggerProp = m_savePropGroup->addProperty( "Do save", "Press!", WPVBaseTypes::PV_TRIGGER_READY );
    m_saveTriggerProp->getCondition()->subscribeSignal( boost::bind( &WMSplineSurface::save, this ) );

    m_meshFile = m_savePropGroup->addProperty( "Mesh file", "", WPathHelper::getAppPath() );

    WModule::properties();
}

void WMSplineSurface::renderMesh()
{
    //    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()
    m_moduleNode->remove( m_surfaceGeode );
    osg::Geometry* surfaceGeometry = new osg::Geometry();
    m_surfaceGeode = osg::ref_ptr< osg::Geode >( new osg::Geode );

    m_surfaceGeode->setName( "spline surface" );

    surfaceGeometry->setVertexArray( m_triMesh->getVertexArray() );

    osg::DrawElementsUInt* surfaceElement;

    surfaceElement = new osg::DrawElementsUInt( osg::PrimitiveSet::TRIANGLES, 0 );

    std::vector< size_t > tris = m_triMesh->getTriangles();
    surfaceElement->reserve( tris.size() );

    for( unsigned int vertId = 0; vertId < tris.size(); ++vertId )
    {
        surfaceElement->push_back( tris[vertId] );
    }
    surfaceGeometry->addPrimitiveSet( surfaceElement );

    // ------------------------------------------------
    // normals
    surfaceGeometry->setNormalArray( m_triMesh->getVertexNormalArray() );
    surfaceGeometry->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );

    m_surfaceGeode->addDrawable( surfaceGeometry );
    osg::StateSet* state = m_surfaceGeode->getOrCreateStateSet();

    // ------------------------------------------------
    // colors
    osg::Vec4Array* colors = new osg::Vec4Array;
    colors->push_back( m_surfaceColor->get( true ) );
    surfaceGeometry->setColorArray( colors );
    surfaceGeometry->setColorBinding( osg::Geometry::BIND_OVERALL );

    osg::ref_ptr< osg::LightModel > lightModel = new osg::LightModel();
    lightModel->setTwoSided( true );
    state->setAttributeAndModes( lightModel.get(), osg::StateAttribute::ON );
    state->setMode( GL_BLEND, osg::StateAttribute::ON );

    osg::ref_ptr< WGEShader > shader = osg::ref_ptr< WGEShader >( new WGEShader( "WMSplineSurface", m_localPath ) );
    shader->addPreprocessor( WGEShaderPreprocessor::SPtr(
        new WGEShaderPropertyDefineOptions< WPropBool >( m_useTextureProp, "COLORMAPPING_DISABLED", "COLORMAPPING_ENABLED" ) )
    );
    state->addUniform( new WGEPropertyUniform< WPropInt >( "u_opacity", m_opacityProp ) );
    shader->apply( m_surfaceGeode );

    // Colormapping
    WGEColormapping::apply( m_surfaceGeode, shader );

    m_moduleNode->insert( m_surfaceGeode );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_moduleNode );

    m_moduleNode->addUpdateCallback( new SplineSurfaceNodeCallback( this ) );
}

bool WMSplineSurface::save() const
{
    //    m_saveTriggerProp->set( WPVBaseTypes::PV_TRIGGER_READY, false );
    //
    //    if( m_triMesh->vertSize() == 0 )
    //    {
    //        WLogger::getLogger()->addLogMessage( "Will not write file that contains 0 vertices.", "Marching Cubes", LL_ERROR );
    //        return false;
    //    }
    //
    //    if( m_triMesh->triangleSize() == 0 )
    //    {
    //        WLogger::getLogger()->addLogMessage( "Will not write file that contains 0 triangles.", "Marching Cubes", LL_ERROR );
    //        return false;
    //    }
    //
    //    const char* c_file = m_meshFile->get().string().c_str();
    //    std::ofstream dataFile( c_file );
    //
    //    if( dataFile )
    //    {
    //        WLogger::getLogger()->addLogMessage( "opening file", "Marching Cubes", LL_DEBUG );
    //    }
    //    else
    //    {
    //        WLogger::getLogger()->addLogMessage( "open file failed" + m_meshFile->get().string() , "Marching Cubes", LL_ERROR );
    //        return false;
    //    }
    //
    //    dataFile.precision( 16 );
    //
    //    WLogger::getLogger()->addLogMessage( "start writing file", "Marching Cubes", LL_DEBUG );
    //    dataFile << ( "# vtk DataFile Version 2.0\n" );
    //    dataFile << ( "generated using OpenWalnut\n" );
    //    dataFile << ( "ASCII\n" );
    //    dataFile << ( "DATASET UNSTRUCTURED_GRID\n" );
    //
    //    WPosition point;
    //    dataFile << "POINTS " << m_triMesh->vertSize() << " float\n";
    //    for( size_t i = 0; i < m_triMesh->vertSize(); ++i )
    //    {
    //        point = m_triMesh->getVertex( i );
    //        if( !( myIsfinite( point[0] ) && myIsfinite( point[1] ) && myIsfinite( point[2] ) ) )
    //        {
    //            WLogger::getLogger()->addLogMessage( "Will not write file from data that contains NAN or INF.", "Marching Cubes", LL_ERROR );
    //            return false;
    //        }
    //        dataFile << point[0] << " " << point[1] << " " << point[2] << "\n";
    //    }
    //
    //    dataFile << "CELLS " << m_triMesh->triangleSize() << " " << m_triMesh->triangleSize() * 4 << "\n";
    //    for( size_t i = 0; i < m_triMesh->triangleSize(); ++i )
    //    {
    //        dataFile << "3 " << m_triMesh->getTriVertId0( i ) << " "
    //                 <<  m_triMesh->getTriVertId1( i ) << " "
    //                 <<  m_triMesh->getTriVertId2( i ) << "\n";
    //    }
    //    dataFile << "CELL_TYPES "<< m_triMesh->triangleSize() <<"\n";
    //    for( size_t i = 0; i < m_triMesh->triangleSize(); ++i )
    //    {
    //        dataFile << "5\n";
    //    }
    //    dataFile << "POINT_DATA " << m_triMesh->vertSize() << "\n";
    //    dataFile << "SCALARS scalars float\n";
    //    dataFile << "LOOKUP_TABLE default\n";
    //    for( size_t i = 0; i < m_triMesh->vertSize(); ++i )
    //    {
    //        dataFile << "0\n";
    //    }
    //    dataFile.close();
    //    WLogger::getLogger()->addLogMessage( "saving done", "Marching Cubes", LL_DEBUG );
    return true;
}

void WMSplineSurface::updateGraphics()
{
    if( m_active->get() )
    {
        m_surfaceGeode->setNodeMask( 0xFFFFFFFF );
    }
    else
    {
        m_surfaceGeode->setNodeMask( 0x0 );
    }

    if( m_surfaceColor->changed() )
    {
        osg::Vec4Array* colors = new osg::Vec4Array;
        colors->push_back( m_surfaceColor->get( true ) );
        osg::ref_ptr< osg::Geometry > surfaceGeometry = m_surfaceGeode->getDrawable( 0 )->asGeometry();
        surfaceGeometry->setColorArray( colors );
        surfaceGeometry->setColorBinding( osg::Geometry::BIND_OVERALL );
    }
}
