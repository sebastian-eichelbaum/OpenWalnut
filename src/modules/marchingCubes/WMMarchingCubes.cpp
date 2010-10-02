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
#include <limits>

#include "WMMarchingCubes.xpm"
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
#include "../../common/math/WMath.h"
#include "../../dataHandler/WDataHandler.h"
#include "../../dataHandler/WSubject.h"
#include "../../dataHandler/WDataTexture3D.h"
#include "../../graphicsEngine/WGEUtils.h"
#include "../../kernel/WKernel.h"

#include "../../graphicsEngine/algorithms/WMarchingCubesAlgorithm.h"
#include "WMMarchingCubes.h"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMMarchingCubes )

WMMarchingCubes::WMMarchingCubes():
    WModule(),
    m_recompute( boost::shared_ptr< WCondition >( new WCondition() ) ),
    m_dataSet(),
    m_shaderUseLighting( false ),
    m_shaderUseTransparency( false ),
    m_moduleNodeInserted( false ),
    m_surfaceGeode( 0 )
{
    // WARNING: initializing connectors inside the constructor will lead to an exception.
    // Implement WModule::initializeConnectors instead.
}

WMMarchingCubes::~WMMarchingCubes()
{
    // cleanup
    removeConnectors();
}

boost::shared_ptr< WModule > WMMarchingCubes::factory() const
{
    return boost::shared_ptr< WModule >( new WMMarchingCubes() );
}

const char** WMMarchingCubes::getXPMIcon() const
{
    return iso_surface_xpm;
}

const std::string WMMarchingCubes::getName() const
{
    return "Isosurface";
}

const std::string WMMarchingCubes::getDescription() const
{
    return "This module implements the marching cubes"
" algorithm with a consistent triangulation. It allows to compute isosurfaces"
" for a given isovalue on data given on a grid only consisting of cubes. It yields"
" the surface as triangle soup.";
}

void WMMarchingCubes::moduleMain()
{
    // use the m_input "data changed" flag
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );
    m_moduleState.add( m_recompute );

    // signal ready state
    ready();

    m_moduleNode = new WGEManagedGroupNode( m_active );

    // now, to watch changing/new textures use WSubject's change condition
    boost::signals2::connection con = WDataHandler::getDefaultSubject()->getChangeCondition()->subscribeSignal(
            boost::bind( &WMMarchingCubes::notifyTextureChange, this )
    );

    // loop until the module container requests the module to quit
    while ( !m_shutdownFlag() )
    {
        if ( !m_input->getData().get() )
        {
            // OK, the output has not yet sent data
            // NOTE: see comment at the end of this while loop for m_moduleState
            debugLog() << "Waiting for data ...";
            m_moduleState.wait();
            continue;
        }

        if( m_dataSet != m_input->getData() )
        {
            // acquire data from the input connector
            m_dataSet = m_input->getData();

            if( m_isoValueProp->get() >= m_dataSet->getMax() || m_isoValueProp->get() <= m_dataSet->getMin() )
            {
                // set appropriate constraints for properties
                m_isoValueProp->setMin( m_dataSet->getMin() );
                m_isoValueProp->setMax( m_dataSet->getMax() );
                m_isoValueProp->set( 0.5 * ( m_dataSet->getMax() +  m_dataSet->getMin() ), true );
            }
        }

        // update isosurface
        debugLog() << "Computing surface ...";

        boost::shared_ptr< WProgress > progress = boost::shared_ptr< WProgress >( new WProgress( "Marching Cubes", 2 ) );
        m_progress->addSubProgress( progress );

        generateSurfacePre( m_isoValueProp->get() );

        ++*progress;
        debugLog() << "Rendering surface ...";

        // settings for normal isosurface
        m_shaderUseLighting = true;
        m_shaderUseTransparency = true;

        renderMesh();
        m_output->updateData( m_triMesh );

        debugLog() << "Done!";
        progress->finish();

        // this waits for m_moduleState to fire. By default, this is only the m_shutdownFlag condition.
        // NOTE: you can add your own conditions to m_moduleState using m_moduleState.add( ... )
        m_moduleState.wait();
    }

    WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getScene()->remove( m_moduleNode );
}

void WMMarchingCubes::connectors()
{
    // initialize connectors
    m_input = boost::shared_ptr< WModuleInputData < WDataSetScalar > >(
        new WModuleInputData< WDataSetScalar >( shared_from_this(),
                                                               "values", "Dataset to compute isosurface for." )
        );

    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_input );

    m_output = boost::shared_ptr< WModuleOutputData< WTriangleMesh2 > >(
            new WModuleOutputData< WTriangleMesh2 >( shared_from_this(), "surface mesh", "The mesh representing the isosurface." ) );

    addConnector( m_output );

    // call WModules initialization
    WModule::connectors();
}

void WMMarchingCubes::properties()
{
    m_nbTriangles = m_infoProperties->addProperty( "Triangles", "The number of triangles in the produced mesh.", 0 );
    m_nbTriangles->setMax( std::numeric_limits< int >::max() );

    m_nbVertices = m_infoProperties->addProperty( "Vertices", "The number of vertices in the produced mesh.", 0 );
    m_nbVertices->setMax( std::numeric_limits< int >::max() );

    m_isoValueProp = m_properties->addProperty( "Iso value", "The surface will show the area that has this value.", 100., m_recompute );
    m_isoValueProp->setMin( wlimits::MIN_DOUBLE );
    m_isoValueProp->setMax( wlimits::MAX_DOUBLE );
    {
        // If set in config file use standard isovalue from config file
        double tmpIsoValue;
        if( WPreferences::getPreference( "modules.MC.isoValue", &tmpIsoValue ) )
        {
            m_isoValueProp->set( tmpIsoValue );
        }
    }

    m_opacityProp = m_properties->addProperty( "Opacity %", "Opaqueness of surface.", 100 );
    m_opacityProp->setMin( 0 );
    m_opacityProp->setMax( 100 );

    m_useTextureProp = m_properties->addProperty( "Use texture", "Use texturing of the surface?", false );

    m_surfaceColor = m_properties->addProperty( "Surface color", "Description.", WColor( 0.5, 0.5, 0.5, 1.0 ) );

    m_savePropGroup = m_properties->addPropertyGroup( "Save Surface",  "" );
    m_saveTriggerProp = m_savePropGroup->addProperty( "Do Save",  "Press!",
                                                  WPVBaseTypes::PV_TRIGGER_READY );
    m_saveTriggerProp->getCondition()->subscribeSignal( boost::bind( &WMMarchingCubes::save, this ) );

    m_meshFile = m_savePropGroup->addProperty( "Mesh File", "", WPathHelper::getAppPath() );

    WModule::properties();
}

void WMMarchingCubes::generateSurfacePre( double isoValue )
{
    debugLog() << "Isovalue: " << isoValue;
    WAssert( ( *m_dataSet ).getValueSet()->order() == 0, "This module only works on scalars." );

    WMarchingCubesAlgorithm mcAlgo;
    boost::shared_ptr< WGridRegular3D > gridRegular3D = boost::shared_dynamic_cast< WGridRegular3D >( ( *m_dataSet ).getGrid() );
    WAssert( gridRegular3D, "Grid is not of type WGridRegular3D." );
    m_grid = gridRegular3D;

    switch( (*m_dataSet).getValueSet()->getDataType() )
    {
        case W_DT_UNSIGNED_CHAR:
        {
            boost::shared_ptr< WValueSet< unsigned char > > vals;
            vals =  boost::shared_dynamic_cast< WValueSet< unsigned char > >( ( *m_dataSet ).getValueSet() );
            WAssert( vals, "Data type and data type indicator must fit." );
            m_triMesh = mcAlgo.generateSurface( m_grid->getNbCoordsX(), m_grid->getNbCoordsY(), m_grid->getNbCoordsZ(),
                                                m_grid->getTransformationMatrix(),
                                                vals->rawDataVectorPointer(),
                                                isoValue,
                                                m_progress );
            break;
        }
        case W_DT_INT16:
        {
            boost::shared_ptr< WValueSet< int16_t > > vals;
            vals =  boost::shared_dynamic_cast< WValueSet< int16_t > >( ( *m_dataSet ).getValueSet() );
            WAssert( vals, "Data type and data type indicator must fit." );
            m_triMesh = mcAlgo.generateSurface( m_grid->getNbCoordsX(), m_grid->getNbCoordsY(), m_grid->getNbCoordsZ(),
                                                m_grid->getTransformationMatrix(),
                                                vals->rawDataVectorPointer(),
                                                isoValue,
                                                m_progress );
            break;
        }
        case W_DT_SIGNED_INT:
        {
            boost::shared_ptr< WValueSet< int32_t > > vals;
            vals =  boost::shared_dynamic_cast< WValueSet< int32_t > >( ( *m_dataSet ).getValueSet() );
            WAssert( vals, "Data type and data type indicator must fit." );
            m_triMesh = mcAlgo.generateSurface( m_grid->getNbCoordsX(), m_grid->getNbCoordsY(), m_grid->getNbCoordsZ(),
                                                m_grid->getTransformationMatrix(),
                                                vals->rawDataVectorPointer(),
                                                isoValue,
                                                m_progress );
            break;
        }
        case W_DT_FLOAT:
        {
            boost::shared_ptr< WValueSet< float > > vals;
            vals =  boost::shared_dynamic_cast< WValueSet< float > >( ( *m_dataSet ).getValueSet() );
            WAssert( vals, "Data type and data type indicator must fit." );
            m_triMesh = mcAlgo.generateSurface( m_grid->getNbCoordsX(), m_grid->getNbCoordsY(), m_grid->getNbCoordsZ(),
                                                m_grid->getTransformationMatrix(),
                                                vals->rawDataVectorPointer(),
                                                isoValue,
                                                m_progress );
            break;
        }
        case W_DT_DOUBLE:
        {
            boost::shared_ptr< WValueSet< double > > vals;
            vals =  boost::shared_dynamic_cast< WValueSet< double > >( ( *m_dataSet ).getValueSet() );
            WAssert( vals, "Data type and data type indicator must fit." );
            m_triMesh = mcAlgo.generateSurface( m_grid->getNbCoordsX(), m_grid->getNbCoordsY(), m_grid->getNbCoordsZ(),
                                                m_grid->getTransformationMatrix(),
                                                vals->rawDataVectorPointer(),
                                                isoValue,
                                                m_progress );
            break;
        }
        default:
            WAssert( false, "Unknow data type in MarchingCubes module" );
    }

    // Set the info properties
    m_nbTriangles->set( m_triMesh->triangleSize() );
    m_nbVertices->set( m_triMesh->vertSize() );
}

void WMMarchingCubes::renderMesh()
{
    {
        // Remove the previous node in a thread save way.
        boost::unique_lock< boost::shared_mutex > lock;
        lock = boost::unique_lock< boost::shared_mutex >( m_updateLock );

        m_moduleNode->remove( m_surfaceGeode );

        lock.unlock();
    }

    osg::Geometry* surfaceGeometry = new osg::Geometry();
    m_surfaceGeode = osg::ref_ptr< osg::Geode >( new osg::Geode );

    m_surfaceGeode->setName( "iso surface" );

    surfaceGeometry->setVertexArray( m_triMesh->getVertexArray() );
    osg::DrawElementsUInt* surfaceElement;

    surfaceElement = new osg::DrawElementsUInt( osg::PrimitiveSet::TRIANGLES, 0 );

    std::vector< size_t >tris = m_triMesh->getTriangles();
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

    WColor c = m_surfaceColor->get( true );
    colors->push_back( osg::Vec4( c.getRed(), c.getGreen(), c.getBlue(), 1.0f ) );
    surfaceGeometry->setColorArray( colors );
    surfaceGeometry->setColorBinding( osg::Geometry::BIND_OVERALL );

    osg::ref_ptr<osg::LightModel> lightModel = new osg::LightModel();
    lightModel->setTwoSided( true );
    state->setAttributeAndModes( lightModel.get(), osg::StateAttribute::ON );
    {
        osg::ref_ptr< osg::Material > material = new osg::Material();
        material->setDiffuse(   osg::Material::FRONT, osg::Vec4( 1.0, 1.0, 1.0, 1.0 ) );
        material->setSpecular(  osg::Material::FRONT, osg::Vec4( 0.0, 0.0, 0.0, 1.0 ) );
        material->setAmbient(   osg::Material::FRONT, osg::Vec4( 0.1, 0.1, 0.1, 1.0 ) );
        material->setEmission(  osg::Material::FRONT, osg::Vec4( 0.0, 0.0, 0.0, 1.0 ) );
        material->setShininess( osg::Material::FRONT, 25.0 );
        state->setAttribute( material );
    }

    // Enable blending, select transparent bin.
    if ( m_shaderUseTransparency )
    {
        state->setMode( GL_BLEND, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON );
        state->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

        // Enable depth test so that an opaque polygon will occlude a transparent one behind it.
        state->setMode( GL_DEPTH_TEST, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON );

        // Conversely, disable writing to depth buffer so that
        // a transparent polygon will allow polygons behind it to shine thru.
        // OSG renders transparent polygons after opaque ones.
        osg::Depth* depth = new osg::Depth;
        depth->setWriteMask( false );
        state->setAttributeAndModes( depth, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON );

        // Disable conflicting modes.
        state->setMode( GL_LIGHTING,  osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF );
        state->setMode( GL_CULL_FACE, osg::StateAttribute::OFF );
    }

    surfaceGeometry->setUseDisplayList( false );
    surfaceGeometry->setUseVertexBufferObjects( true );

    // ------------------------------------------------
    // Shader stuff

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
        state->addUniform( m_typeUniforms[i] );
        state->addUniform( m_thresholdUniforms[i] );
        state->addUniform( m_alphaUniforms[i] );
        state->addUniform( m_samplerUniforms[i] );
        state->addUniform( m_cmapUniforms[i] );
    }

    state->addUniform( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "useLighting", m_shaderUseLighting ) ) );
    if( m_shaderUseTransparency )
    {
        state->addUniform( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "opacity", m_opacityProp->get( true ) ) ) );
    }
    else
    {
        state->addUniform( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "opacity", 100 ) ) );
    }

    // NOTE: the following code should not be necessary. The update callback does this job just before the mesh is rendered
    // initially. Just set the texture changed flag to true. If this however might be needed use WSubject::getDataTextures.
    m_textureChanged = true;

    m_shader = osg::ref_ptr< WShader > ( new WShader( "WMMarchingCubes", m_localPath ) );
    m_shader->apply( m_surfaceGeode );

    m_moduleNode->insert( m_surfaceGeode );
    if ( !m_moduleNodeInserted )
    {
        WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_moduleNode );
        m_moduleNodeInserted = true;
    }

    m_moduleNode->addUpdateCallback( new SurfaceNodeCallback( this ) );
}

void WMMarchingCubes::notifyTextureChange()
{
    m_textureChanged = true;
}

bool WMMarchingCubes::save() const
{
    m_saveTriggerProp->set( WPVBaseTypes::PV_TRIGGER_READY, false );

    if( m_triMesh->vertSize() == 0 )
    {
        WLogger::getLogger()->addLogMessage( "Will not write file that contains 0 vertices.", "Marching Cubes", LL_ERROR );
        return false;
    }

    if( m_triMesh->triangleSize() == 0 )
    {
        WLogger::getLogger()->addLogMessage( "Will not write file that contains 0 triangles.", "Marching Cubes", LL_ERROR );
        return false;
    }

    const char* c_file = m_meshFile->get().file_string().c_str();
    std::ofstream dataFile( c_file, std::ios_base::binary );

    if ( dataFile )
    {
        WLogger::getLogger()->addLogMessage( "opening file", "Marching Cubes", LL_DEBUG );
    }
    else
    {
        WLogger::getLogger()->addLogMessage( "open file failed" + m_meshFile->get().file_string() , "Marching Cubes", LL_ERROR );
        return false;
    }

    dataFile.precision( 16 );

    WLogger::getLogger()->addLogMessage( "start writing file", "Marching Cubes", LL_DEBUG );
    dataFile << ( "# vtk DataFile Version 2.0\n" );
    dataFile << ( "generated using OpenWalnut\n" );
    dataFile << ( "ASCII\n" );
    dataFile << ( "DATASET UNSTRUCTURED_GRID\n" );

    wmath::WPosition point;
    dataFile << "POINTS " << m_triMesh->vertSize() << " float\n";
    for ( size_t i = 0; i < m_triMesh->vertSize(); ++i )
    {
        point = m_triMesh->getVertexAsPosition( i );
        if( !( wmath::myIsfinite( point[0] ) && wmath::myIsfinite( point[1] ) && wmath::myIsfinite( point[2] ) ) )
        {
            WLogger::getLogger()->addLogMessage( "Will not write file from data that contains NAN or INF.", "Marching Cubes", LL_ERROR );
            return false;
        }
        dataFile << point[0] << " " << point[1] << " " << point[2] << "\n";
    }

    dataFile << "CELLS " << m_triMesh->triangleSize() << " " << m_triMesh->triangleSize() * 4 << "\n";
    for ( size_t i = 0; i < m_triMesh->triangleSize(); ++i )
    {
        dataFile << "3 " << m_triMesh->getTriVertId0( i ) << " "
                 <<  m_triMesh->getTriVertId1( i ) << " "
                 <<  m_triMesh->getTriVertId2( i ) << "\n";
    }
    dataFile << "CELL_TYPES "<< m_triMesh->triangleSize() <<"\n";
    for ( size_t i = 0; i < m_triMesh->triangleSize(); ++i )
    {
        dataFile << "5\n";
    }
    dataFile << "POINT_DATA " << m_triMesh->vertSize() << "\n";
    dataFile << "SCALARS scalars float\n";
    dataFile << "LOOKUP_TABLE default\n";
    for ( size_t i = 0; i < m_triMesh->vertSize(); ++i )
    {
        dataFile << "0\n";
    }
    dataFile.close();
    WLogger::getLogger()->addLogMessage( "saving done", "Marching Cubes", LL_DEBUG );
    return true;
}

void WMMarchingCubes::updateGraphicsForCallback()
{
    boost::unique_lock< boost::shared_mutex > lock;
    lock = boost::unique_lock< boost::shared_mutex >( m_updateLock );

    if( m_surfaceColor->changed() )
    {
        osg::Vec4Array* colors = new osg::Vec4Array;

        WColor c = m_surfaceColor->get( true );
        colors->push_back( osg::Vec4( c.getRed(), c.getGreen(), c.getBlue(), 1.0f ) );
        osg::ref_ptr< osg::Geometry > surfaceGeometry = m_surfaceGeode->getDrawable( 0 )->asGeometry();
        surfaceGeometry->setColorArray( colors );
        surfaceGeometry->setColorBinding( osg::Geometry::BIND_OVERALL );
    }

    if ( m_textureChanged || m_opacityProp->changed() || m_useTextureProp->changed()  )
    {
        bool localTextureChangedFlag = m_textureChanged;
        m_textureChanged = false;

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

            rootState->addUniform( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "useTexture", m_useTextureProp->get( true ) ) ) );
            rootState->addUniform( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "useLighting", m_shaderUseLighting ) ) );

            if( m_shaderUseTransparency )
            {
                rootState->addUniform( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "opacity", m_opacityProp->get( true ) ) ) );
            }
            else
            {
                rootState->addUniform( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "opacity", 100 ) ) );
            }

            // for each texture -> apply
            int c = 0;
            //////////////////////////////////////////////////////////////////////////////////////////////////
            if ( WKernel::getRunningKernel()->getSelectionManager()->getUseTexture() )
            {
                boost::shared_ptr< WGridRegular3D > grid = WKernel::getRunningKernel()->getSelectionManager()->getGrid();
                osg::ref_ptr< osg::Geometry > surfaceGeometry = m_surfaceGeode->getDrawable( 0 )->asGeometry();
                osg::Vec3Array* texCoords = new osg::Vec3Array;

                for( size_t i = 0; i < m_triMesh->vertSize(); ++i )
                {
                    osg::Vec3 vertPos = m_triMesh->getVertex( i );
                    texCoords->push_back( wge::wv3D2ov3( grid->worldCoordToTexCoord( wmath::WPosition( vertPos[0], vertPos[1], vertPos[2] ) ) ) );
                }
                surfaceGeometry->setTexCoordArray( c, texCoords );

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
                if( localTextureChangedFlag )
                {
                    osg::ref_ptr< osg::Geometry > surfaceGeometry = m_surfaceGeode->getDrawable( 0 )->asGeometry();
                    osg::Vec3Array* texCoords = new osg::Vec3Array;
                    boost::shared_ptr< WGridRegular3D > grid = ( *iter )->getGrid();
                    for( size_t i = 0; i < m_triMesh->vertSize(); ++i )
                    {
                        osg::Vec3 vertPos = m_triMesh->getVertex( i );
                        texCoords->push_back( wge::wv3D2ov3( grid->worldCoordToTexCoord( wmath::WPosition( vertPos[0], vertPos[1], vertPos[2] ) ) ) );
                    }
                    surfaceGeometry->setTexCoordArray( c, texCoords );
                }

                osg::ref_ptr<osg::Texture3D> texture3D = ( *iter )->getTexture();
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
    lock.unlock();
}

