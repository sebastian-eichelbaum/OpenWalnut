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
#include "../../common/WLimits.h"
#include "../../common/WAssert.h"

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Material>
#include <osg/StateSet>
#include <osg/StateAttribute>
#include <osg/PolygonMode>
#include <osg/LightModel>
#include <osgDB/WriteFile>

#include "../../common/WPathHelper.h"
#include "../../common/WProgress.h"
#include "../../common/WPreferences.h"
#include "../../common/math/WVector3D.h"
#include "../../common/math/WLinearAlgebraFunctions.h"
#include "../../dataHandler/WDataHandler.h"
#include "../../dataHandler/WSubject.h"
#include "../../dataHandler/WDataTexture3D.h"
#include "../../graphicsEngine/WGEUtils.h"
#include "../../kernel/WKernel.h"

#include "../../graphicsEngine/algorithms/WMarchingCubesAlgorithm.h"
#include "WSurface.h"
#include "WMSplineSurface.h"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMSplineSurface )

WMSplineSurface::WMSplineSurface() :
    WModule(), m_recompute( boost::shared_ptr< WCondition >( new WCondition() ) ), m_shaderUseLighting( false ), m_shaderUseTransparency( false ),
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
    return "This module implements the marching cubes"
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

    // now, to watch changing/new textures use WSubject's change condition
    boost::signals2::connection con = WDataHandler::getDefaultSubject()->getChangeCondition()->subscribeSignal( boost::bind(
            &WMSplineSurface::notifyTextureChange, this ) );

    // loop until the module container requests the module to quit
    while ( !m_shutdownFlag() )
    {
        // update isosurface
        debugLog() << "Computing surface ...";

        WSurface surf;

        //////////////////////////////////////////////////////////////////////////////////////////////////////
        std::vector< wmath::WVector3D > givenPoints;
        for( int y = 0; y < 11; ++y )
        {
            for( int z = 0; z < 11; ++z )
            {
                float pi = 3.14159265;
                float pi2 = pi * 2;
                float yy = pi2 * y / 10.;
                float zz = pi2 * z / 10.;

                wmath::WVector3D p;
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

        // settings for normal isosurface
        m_shaderUseLighting = true;
        m_shaderUseTransparency = true;

        renderMesh();
        m_output->updateData( m_triMesh );

        debugLog() << "Done!";

        // this waits for m_moduleState to fire. By default, this is only the m_shutdownFlag condition.
        // NOTE: you can add your own conditions to m_moduleState using m_moduleState.add( ... )
        m_moduleState.wait();
    }

    con.disconnect();

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

    m_useTextureProp = m_properties->addProperty( "Use texture", "Use texturing of the surface?", false );

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

    for ( unsigned int vertId = 0; vertId < tris.size(); ++vertId )
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

    WColor c = m_surfaceColor->get( true );
    colors->push_back( osg::Vec4( c.getRed(), c.getGreen(), c.getBlue(), 1.0f ) );
    surfaceGeometry->setColorArray( colors );
    surfaceGeometry->setColorBinding( osg::Geometry::BIND_OVERALL );

    osg::ref_ptr< osg::LightModel > lightModel = new osg::LightModel();
    lightModel->setTwoSided( true );
    state->setAttributeAndModes( lightModel.get(), osg::StateAttribute::ON );
    state->setMode( GL_BLEND, osg::StateAttribute::ON );

    {
        osg::ref_ptr< osg::Material > material = new osg::Material();
        material->setDiffuse( osg::Material::FRONT_AND_BACK, osg::Vec4( 1.0, 1.0, 1.0, 1.0 ) );
        material->setSpecular( osg::Material::FRONT_AND_BACK, osg::Vec4( 0.0, 0.0, 0.0, 1.0 ) );
        material->setAmbient( osg::Material::FRONT_AND_BACK, osg::Vec4( 0.1, 0.1, 0.1, 1.0 ) );
        material->setEmission( osg::Material::FRONT_AND_BACK, osg::Vec4( 0.0, 0.0, 0.0, 1.0 ) );
        material->setShininess( osg::Material::FRONT_AND_BACK, 25.0 );
        state->setAttribute( material );
    }

    // ------------------------------------------------
    // Shader stuff

    m_typeUniforms.push_back( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "type0", 0 ) ) );
    m_typeUniforms.push_back( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "type1", 0 ) ) );
    m_typeUniforms.push_back( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "type2", 0 ) ) );
    m_typeUniforms.push_back( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "type3", 0 ) ) );
    m_typeUniforms.push_back( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "type4", 0 ) ) );
    m_typeUniforms.push_back( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "type5", 0 ) ) );
    m_typeUniforms.push_back( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "type6", 0 ) ) );
    m_typeUniforms.push_back( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "type7", 0 ) ) );
    m_typeUniforms.push_back( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "type8", 0 ) ) );
    m_typeUniforms.push_back( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "type9", 0 ) ) );

    m_alphaUniforms.push_back( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "alpha0", 1.0f ) ) );
    m_alphaUniforms.push_back( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "alpha1", 1.0f ) ) );
    m_alphaUniforms.push_back( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "alpha2", 1.0f ) ) );
    m_alphaUniforms.push_back( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "alpha3", 1.0f ) ) );
    m_alphaUniforms.push_back( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "alpha4", 1.0f ) ) );
    m_alphaUniforms.push_back( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "alpha5", 1.0f ) ) );
    m_alphaUniforms.push_back( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "alpha6", 1.0f ) ) );
    m_alphaUniforms.push_back( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "alpha7", 1.0f ) ) );
    m_alphaUniforms.push_back( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "alpha8", 1.0f ) ) );
    m_alphaUniforms.push_back( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "alpha9", 1.0f ) ) );

    m_thresholdUniforms.push_back( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "threshold0", 0.0f ) ) );
    m_thresholdUniforms.push_back( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "threshold1", 0.0f ) ) );
    m_thresholdUniforms.push_back( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "threshold2", 0.0f ) ) );
    m_thresholdUniforms.push_back( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "threshold3", 0.0f ) ) );
    m_thresholdUniforms.push_back( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "threshold4", 0.0f ) ) );
    m_thresholdUniforms.push_back( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "threshold5", 0.0f ) ) );
    m_thresholdUniforms.push_back( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "threshold6", 0.0f ) ) );
    m_thresholdUniforms.push_back( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "threshold7", 0.0f ) ) );
    m_thresholdUniforms.push_back( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "threshold8", 0.0f ) ) );
    m_thresholdUniforms.push_back( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "threshold9", 0.0f ) ) );

    m_samplerUniforms.push_back( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "tex0", 0 ) ) );
    m_samplerUniforms.push_back( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "tex1", 1 ) ) );
    m_samplerUniforms.push_back( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "tex2", 2 ) ) );
    m_samplerUniforms.push_back( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "tex3", 3 ) ) );
    m_samplerUniforms.push_back( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "tex4", 4 ) ) );
    m_samplerUniforms.push_back( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "tex5", 5 ) ) );
    m_samplerUniforms.push_back( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "tex6", 6 ) ) );
    m_samplerUniforms.push_back( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "tex7", 7 ) ) );
    m_samplerUniforms.push_back( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "tex8", 8 ) ) );
    m_samplerUniforms.push_back( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "tex9", 9 ) ) );

    m_cmapUniforms.push_back( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "useCmap0", 0 ) ) );
    m_cmapUniforms.push_back( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "useCmap1", 0 ) ) );
    m_cmapUniforms.push_back( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "useCmap2", 0 ) ) );
    m_cmapUniforms.push_back( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "useCmap3", 0 ) ) );
    m_cmapUniforms.push_back( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "useCmap4", 0 ) ) );
    m_cmapUniforms.push_back( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "useCmap5", 0 ) ) );
    m_cmapUniforms.push_back( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "useCmap6", 0 ) ) );
    m_cmapUniforms.push_back( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "useCmap7", 0 ) ) );
    m_cmapUniforms.push_back( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "useCmap8", 0 ) ) );
    m_cmapUniforms.push_back( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "useCmap9", 0 ) ) );

    for ( int i = 0; i < m_maxNumberOfTextures; ++i )
    {
        state->addUniform( m_typeUniforms[i] );
        state->addUniform( m_thresholdUniforms[i] );
        state->addUniform( m_alphaUniforms[i] );
        state->addUniform( m_samplerUniforms[i] );
        state->addUniform( m_cmapUniforms[i] );
    }

    state->addUniform( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "useLighting", m_shaderUseLighting ) ) );
    if ( m_shaderUseTransparency )
    {
        state->addUniform( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "opacity", m_opacityProp->get( true ) ) ) );
    }
    else
    {
        state->addUniform( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "opacity", 100 ) ) );
    }

    // NOTE: the following code should not be necessary. The update callback does this job just before the mesh is rendered
    // initially. Just set the texture changed flag to true. If this however might be needed use WSubject::getDataTextures.
    m_textureChanged = true;

    m_shader = osg::ref_ptr< WShader >( new WShader( "surface", m_localPath ) );
    m_shader->apply( m_surfaceGeode );

    m_moduleNode->insert( m_surfaceGeode );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_moduleNode );

    m_moduleNode->addUpdateCallback( new SplineSurfaceNodeCallback( this ) );
}

void WMSplineSurface::notifyTextureChange()
{
    m_textureChanged = true;
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
    //    const char* c_file = m_meshFile->get().file_string().c_str();
    //    std::ofstream dataFile( c_file );
    //
    //    if ( dataFile )
    //    {
    //        WLogger::getLogger()->addLogMessage( "opening file", "Marching Cubes", LL_DEBUG );
    //    }
    //    else
    //    {
    //        WLogger::getLogger()->addLogMessage( "open file failed" + m_meshFile->get().file_string() , "Marching Cubes", LL_ERROR );
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
    //    wmath::WPosition point;
    //    dataFile << "POINTS " << m_triMesh->vertSize() << " float\n";
    //    for ( size_t i = 0; i < m_triMesh->vertSize(); ++i )
    //    {
    //        point = m_triMesh->getVertexAsPosition( i );
    //        if( !( myIsfinite( point[0] ) && myIsfinite( point[1] ) && myIsfinite( point[2] ) ) )
    //        {
    //            WLogger::getLogger()->addLogMessage( "Will not write file from data that contains NAN or INF.", "Marching Cubes", LL_ERROR );
    //            return false;
    //        }
    //        dataFile << point[0] << " " << point[1] << " " << point[2] << "\n";
    //    }
    //
    //    dataFile << "CELLS " << m_triMesh->triangleSize() << " " << m_triMesh->triangleSize() * 4 << "\n";
    //    for ( size_t i = 0; i < m_triMesh->triangleSize(); ++i )
    //    {
    //        dataFile << "3 " << m_triMesh->getTriVertId0( i ) << " "
    //                 <<  m_triMesh->getTriVertId1( i ) << " "
    //                 <<  m_triMesh->getTriVertId2( i ) << "\n";
    //    }
    //    dataFile << "CELL_TYPES "<< m_triMesh->triangleSize() <<"\n";
    //    for ( size_t i = 0; i < m_triMesh->triangleSize(); ++i )
    //    {
    //        dataFile << "5\n";
    //    }
    //    dataFile << "POINT_DATA " << m_triMesh->vertSize() << "\n";
    //    dataFile << "SCALARS scalars float\n";
    //    dataFile << "LOOKUP_TABLE default\n";
    //    for ( size_t i = 0; i < m_triMesh->vertSize(); ++i )
    //    {
    //        dataFile << "0\n";
    //    }
    //    dataFile.close();
    //    WLogger::getLogger()->addLogMessage( "saving done", "Marching Cubes", LL_DEBUG );
    return true;
}

void WMSplineSurface::updateGraphics()
{
    if ( m_active->get() )
    {
        m_surfaceGeode->setNodeMask( 0xFFFFFFFF );
    }
    else
    {
        m_surfaceGeode->setNodeMask( 0x0 );
    }

    if ( m_surfaceColor->changed() )
    {
        osg::Vec4Array* colors = new osg::Vec4Array;

        WColor c = m_surfaceColor->get( true );
        colors->push_back( osg::Vec4( c.getRed(), c.getGreen(), c.getBlue(), 1.0f ) );
        osg::ref_ptr< osg::Geometry > surfaceGeometry = m_surfaceGeode->getDrawable( 0 )->asGeometry();
        surfaceGeometry->setColorArray( colors );
        surfaceGeometry->setColorBinding( osg::Geometry::BIND_OVERALL );
    }

    if ( m_textureChanged || m_opacityProp->changed() || m_useTextureProp->changed() )
    {
        bool localTextureChangedFlag = m_textureChanged;
        m_textureChanged = false;
        m_opacityProp->get( true );
        m_useTextureProp->get( true );

        // grab a list of data textures
        std::vector< boost::shared_ptr< WDataTexture3D > > tex = WDataHandler::getDefaultSubject()->getDataTextures( true );

        if ( tex.size() > 0 )
        {
            osg::StateSet* rootState = m_surfaceGeode->getOrCreateStateSet();

            // reset all uniforms
            for ( int i = 0; i < m_maxNumberOfTextures; ++i )
            {
                m_typeUniforms[i]->set( 0 );
            }

            rootState->addUniform( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "useTexture", m_useTextureProp->get( true ) ) ) );
            rootState->addUniform( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "useLighting", m_shaderUseLighting ) ) );

            if ( m_shaderUseTransparency )
            {
                rootState->addUniform( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "opacity", m_opacityProp->get( true ) ) ) );
            }
            else
            {
                rootState->addUniform( osg::ref_ptr< osg::Uniform >( new osg::Uniform( "opacity", 100 ) ) );
            }

            // for each texture -> apply
            int c = 0;
            for ( std::vector< boost::shared_ptr< WDataTexture3D > >::const_iterator iter = tex.begin(); iter != tex.end(); ++iter )
            {
                if ( localTextureChangedFlag )
                {
                    osg::ref_ptr< osg::Geometry > surfaceGeometry = m_surfaceGeode->getDrawable( 0 )->asGeometry();
                    osg::Vec3Array* texCoords = new osg::Vec3Array;
                    boost::shared_ptr< WGridRegular3D > grid = ( *iter )->getGrid();
                    for ( size_t i = 0; i < m_triMesh->vertSize(); ++i )
                    {
                        osg::Vec3 vertPos = m_triMesh->getVertex( i );
                        texCoords->push_back( grid->worldCoordToTexCoord( wmath::WPosition( vertPos[0], vertPos[1], vertPos[2] ) ) );
                    }
                    surfaceGeometry->setTexCoordArray( c, texCoords );
                }

                osg::ref_ptr< osg::Texture3D > texture3D = ( *iter )->getTexture();
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
                if ( c == m_maxNumberOfTextures )
                {
                    break;
                }
            }
        }
    }
}
