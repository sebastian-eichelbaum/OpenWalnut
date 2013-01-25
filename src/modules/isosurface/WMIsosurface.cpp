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

#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/LightModel>
#include <osg/Material>
#include <osg/PolygonMode>
#include <osg/StateAttribute>
#include <osg/StateSet>
#include <osgDB/WriteFile>

#include "core/common/math/WLinearAlgebraFunctions.h"
#include "core/common/math/WMath.h"
#include "core/common/math/linearAlgebra/WLinearAlgebra.h"
#include "core/common/WAssert.h"
#include "core/common/WLimits.h"
#include "core/common/WPathHelper.h"
#include "core/common/WProgress.h"
#include "core/dataHandler/WDataHandler.h"
#include "core/dataHandler/WSubject.h"
#include "core/graphicsEngine/algorithms/WMarchingCubesAlgorithm.h"
#include "core/graphicsEngine/algorithms/WMarchingLegoAlgorithm.h"
#include "core/graphicsEngine/callbacks/WGEFunctorCallback.h"
#include "core/graphicsEngine/shaders/WGEPropertyUniform.h"
#include "core/graphicsEngine/shaders/WGEShaderPropertyDefineOptions.h"
#include "core/graphicsEngine/shaders/WGEShaderPropertyDefine.h"
#include "core/graphicsEngine/WGEColormapping.h"
#include "core/graphicsEngine/WGEUtils.h"
#include "core/kernel/WKernel.h"
#include "core/kernel/WSelectionManager.h"
#include "WMIsosurface.h"
#include "WMIsosurface.xpm"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMIsosurface )

WMIsosurface::WMIsosurface():
    WModule(),
    m_recompute( boost::shared_ptr< WCondition >( new WCondition() ) ),
    m_dataSet(),
    m_moduleNodeInserted( false ),
    m_surfaceGeode( 0 )
{
    // WARNING: initializing connectors inside the constructor will lead to an exception.
    // Implement WModule::initializeConnectors instead.
}

WMIsosurface::~WMIsosurface()
{
    // cleanup
    removeConnectors();
}

boost::shared_ptr< WModule > WMIsosurface::factory() const
{
    return boost::shared_ptr< WModule >( new WMIsosurface() );
}

const char** WMIsosurface::getXPMIcon() const
{
    return iso_surface_xpm;
}

const std::string WMIsosurface::getName() const
{
    return "Isosurface";
}

const std::string WMIsosurface::getDescription() const
{
    return "This module implements the marching cubes"
" algorithm with a consistent triangulation. It allows one to compute isosurfaces"
" for a given isovalue on data given on a grid only consisting of cubes. It yields"
" the surface as triangle soup.";
}

void WMIsosurface::moduleMain()
{
    // use the m_input "data changed" flag
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );
    m_moduleState.add( m_recompute );

    // signal ready state
    ready();

    m_moduleNode = new WGEManagedGroupNode( m_active );

    // loop until the module container requests the module to quit
    while( !m_shutdownFlag() )
    {
        debugLog() << "Waiting ...";
        m_moduleState.wait();

        // woke up since the module is requested to finish?
        if( m_shutdownFlag() )
        {
            break;
        }

        // query changes in data and data validity
        bool dataChanged = m_input->handledUpdate();
        boost::shared_ptr< WDataSetScalar > incomingDataSet = m_input->getData();
        bool propChanged = m_useMarchingLego->changed() || m_isoValueProp->changed();
        if( !incomingDataSet )
        {
            // OK, the output has not yet sent data
            debugLog() << "Waiting for data ...";
            continue;
        }

        // new data available?
        if( dataChanged )
        {
            // acquire data from the input connector
            m_dataSet = m_input->getData();

            // set appropriate constraints for properties
            m_isoValueProp->setMin( m_dataSet->getMin() );
            m_isoValueProp->setMax( m_dataSet->getMax() );

            if( m_isoValueProp->get() >= m_dataSet->getMax() || m_isoValueProp->get() <= m_dataSet->getMin() )
            {
                m_isoValueProp->set( 0.5 * ( m_dataSet->getMax() +  m_dataSet->getMin() ), true );
            }
        }

        // if nothing has changed, continue waiting
        if( !propChanged && !dataChanged )
        {
            continue;
        }

        // update isosurface
        debugLog() << "Computing surface ...";

        boost::shared_ptr< WProgress > progress = boost::shared_ptr< WProgress >( new WProgress( "Marching Cubes", 2 ) );
        m_progress->addSubProgress( progress );

        generateSurfacePre( m_isoValueProp->get( true ) );

        ++*progress;
        debugLog() << "Rendering surface ...";

        renderMesh();
        m_output->updateData( m_triMesh );

        debugLog() << "Done!";
        progress->finish();
    }

    WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getScene()->remove( m_moduleNode );
}

void WMIsosurface::connectors()
{
    // initialize connectors
    m_input = boost::shared_ptr< WModuleInputData < WDataSetScalar > >(
        new WModuleInputData< WDataSetScalar >( shared_from_this(),
                                                               "values", "Dataset to compute isosurface for." )
        );

    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_input );

    m_output = boost::shared_ptr< WModuleOutputData< WTriangleMesh > >(
            new WModuleOutputData< WTriangleMesh >( shared_from_this(), "surface mesh", "The mesh representing the isosurface." ) );

    addConnector( m_output );

    // call WModules initialization
    WModule::connectors();
}

void WMIsosurface::properties()
{
    m_nbTriangles = m_infoProperties->addProperty( "Triangles", "The number of triangles in the produced mesh.", 0 );
    m_nbTriangles->setMax( std::numeric_limits< int >::max() );

    m_nbVertices = m_infoProperties->addProperty( "Vertices", "The number of vertices in the produced mesh.", 0 );
    m_nbVertices->setMax( std::numeric_limits< int >::max() );

    m_isoValueProp = m_properties->addProperty( "Iso value", "The surface will show the area that has this value.", 100., m_recompute );
    m_isoValueProp->setMin( wlimits::MIN_DOUBLE );
    m_isoValueProp->setMax( wlimits::MAX_DOUBLE );

    m_opacityProp = m_properties->addProperty( "Opacity %", "Opaqueness of surface.", 100 );
    m_opacityProp->setMin( 0 );
    m_opacityProp->setMax( 100 );

    m_useTextureProp = m_properties->addProperty( "Use texture", "Use texturing of the surface?", false );

    m_surfaceColor = m_properties->addProperty( "Surface color", "Description.", WColor( 0.5, 0.5, 0.5, 1.0 ) );

    m_useMarchingLego = m_properties->addProperty( "Voxel surface", "Not interpolated surface", false, m_recompute );

    WModule::properties();
}

namespace
{
    /**
     * A template-less base class to combine both Marching-Cube-like algorithms using one interface
     */
    struct MCBase
    {
        virtual ~MCBase()
        {
        }

        virtual boost::shared_ptr< WTriangleMesh > execute( size_t x, size_t y, size_t z,
                                                            const WMatrix<double>& matrix,
                                                            boost::shared_ptr<WValueSetBase> valueSet,
                                                            double isoValue,
                                                            boost::shared_ptr<WProgressCombiner> ) = 0;
    };

    /**
     * Base class templetized by the algorithm used, i.e., real marching cubes or lego bricks
     * This class enhances the interface of the MC algorithm by the new execute function.
     */
    template<class AlgoBase>
    struct MCAlgoMapperBase : public AlgoBase, public MCBase
    {
        virtual ~MCAlgoMapperBase()
        {
        }
    };

    /**
     * this class matches a data type and an algorithm type
     *
     * It is a helper class that converts the type to call the appropriate function in the MC code
     * in the original algorithm.
     *
     * \param AlgoBase
     * AlgoBase is the algorithm that will be called and must implement
     * AlgoBase::generateSurface( x,y,z, matrix, vals_raw_ptr, isoValue, progress )
     */
    template<class AlgoBase, typename T>
    struct MCAlgoMapper : public MCAlgoMapperBase<AlgoBase>
    {
        virtual ~MCAlgoMapper()
        {
        }

        virtual boost::shared_ptr< WTriangleMesh > execute( size_t x, size_t y, size_t z,
                                                            const WMatrix<double>& matrix,
                                                            boost::shared_ptr<WValueSetBase> valueSet,
                                                            double isoValue,
                                                            boost::shared_ptr<WProgressCombiner> progress )
        {
            boost::shared_ptr< WValueSet< T > > vals(
                    boost::shared_dynamic_cast< WValueSet< T > >( valueSet ) );
            WAssert( vals, "Data type and data type indicator must fit." );
            return AlgoBase::generateSurface( x, y, z, matrix, vals->rawDataVectorPointer(), isoValue, progress );
        }
    };

    /**
     * Function to create the appropriate algorithm code for the data type.
     * The selection is done based on a list of valid types.
     * New data types for this module have to be added here ( and only here ).
     *
     * \param AlgoBase
     * AlgoBase is the algorithm that will be called and must implement
     * AlgoBase::generateSurface( x,y,z, matrix, vals_raw_ptr, isoValue, progress )
     *
     * \param enum_type the OpenWalnut type enum of the data on which the isosurface should be computed.
      */
    template<typename AlgoBase>
    boost::shared_ptr<MCAlgoMapperBase<AlgoBase> > createAlgo( int enum_type )
    {
#define CASE( enum_type ) \
        case enum_type:\
            return boost::shared_ptr<MCAlgoMapperBase<AlgoBase> >( new MCAlgoMapper<AlgoBase, DataTypeRT< enum_type >::type >() )

        switch( enum_type )
        {
            //CASE( W_DT_BINARY ); // the cast operation DataTypeRT is not implemented for binaries, so far
            CASE( W_DT_UNSIGNED_CHAR );
            CASE( W_DT_INT8 );
            CASE( W_DT_SIGNED_SHORT ); // INT16
            CASE( W_DT_SIGNED_INT ); // INT32
            CASE( W_DT_FLOAT );
            CASE( W_DT_DOUBLE );
            CASE( W_DT_UINT16 );
            CASE( W_DT_UINT32 );
            CASE( W_DT_INT64 );
            CASE( W_DT_UINT64 );
            CASE( W_DT_FLOAT128 );
            // no complex 128 or complex 256, no rgba
            default:
            WAssert( false, "Data type not implemented because it is not a scalar or just not implemented, yet" );
        }
        return boost::shared_ptr<MCAlgoMapperBase<AlgoBase> >(); // something went wrong, we got an unreasonable type
#undef CASE
    }
}  // namespace

void WMIsosurface::generateSurfacePre( double isoValue )
{
    debugLog() << "Isovalue: " << isoValue;
    WAssert( ( *m_dataSet ).getValueSet()->order() == 0, "This module only works on scalars." );

    WMarchingCubesAlgorithm mcAlgo;
    WMarchingLegoAlgorithm mlAlgo;
    boost::shared_ptr< WGridRegular3D > gridRegular3D = boost::shared_dynamic_cast< WGridRegular3D >( ( *m_dataSet ).getGrid() );
    WAssert( gridRegular3D, "Grid is not of type WGridRegular3D." );
    m_grid = gridRegular3D;

    boost::shared_ptr< WValueSetBase > valueSet( m_dataSet->getValueSet() );

    boost::shared_ptr<MCBase> algo;
    if( m_useMarchingLego->get( true ) )
    {
        algo = createAlgo<WMarchingLegoAlgorithm>( valueSet->getDataType() );
    }
    else
    {
        algo = createAlgo<WMarchingCubesAlgorithm>( valueSet->getDataType() );
    }

    if( algo )
    {
        m_triMesh = algo->execute( m_grid->getNbCoordsX(), m_grid->getNbCoordsY(), m_grid->getNbCoordsZ(),
                                          m_grid->getTransformationMatrix(),
                                          valueSet,
                                          isoValue, m_progress );

        // Set the info properties
        m_nbTriangles->set( m_triMesh->triangleSize() );
        m_nbVertices->set( m_triMesh->vertSize() );
    }
}

void WMIsosurface::renderMesh()
{
    {
        // Remove the previous node in a thread safe way.
        boost::unique_lock< boost::shared_mutex > lock;
        lock = boost::unique_lock< boost::shared_mutex >( m_updateLock );

        m_moduleNode->remove( m_surfaceGeode );

        lock.unlock();
    }

    osg::ref_ptr< osg::Geometry > surfaceGeometry( new osg::Geometry() );
    m_surfaceGeode = osg::ref_ptr< osg::Geode >( new osg::Geode );

    m_surfaceGeode->setName( "iso surface" );

    surfaceGeometry->setVertexArray( m_triMesh->getVertexArray() );
    osg::ref_ptr< osg::DrawElementsUInt > surfaceElement( new osg::DrawElementsUInt( osg::PrimitiveSet::TRIANGLES, 0 ) );

    std::vector< size_t > tris = m_triMesh->getTriangles();
    surfaceElement->reserve( tris.size() );

    for( unsigned int vertId = 0; vertId < tris.size(); ++vertId )
    {
        surfaceElement->push_back( tris[vertId] );
    }
    surfaceGeometry->addPrimitiveSet( surfaceElement );

    // ------------------------------------------------
    // normals
    if( m_useMarchingLego->get( true ) )
    {
        surfaceGeometry->setNormalArray( m_triMesh->getTriangleNormalArray() );
        surfaceGeometry->setNormalBinding( osg::Geometry::BIND_PER_PRIMITIVE );
    }
    else
    {
        surfaceGeometry->setNormalArray( m_triMesh->getVertexNormalArray() );
        surfaceGeometry->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );
    }

    m_surfaceGeode->addDrawable( surfaceGeometry );
    osg::ref_ptr< osg::StateSet > state = m_surfaceGeode->getOrCreateStateSet();

    // ------------------------------------------------
    // colors
    osg::ref_ptr< osg::Vec4Array > colors( new osg::Vec4Array );

    colors->push_back( m_surfaceColor->get( true ) );
    surfaceGeometry->setColorArray( colors );
    surfaceGeometry->setColorBinding( osg::Geometry::BIND_OVERALL );

    osg::ref_ptr<osg::LightModel> lightModel( new osg::LightModel() );
    lightModel->setTwoSided( true );
    state->setAttributeAndModes( lightModel.get(), osg::StateAttribute::ON );
    {
        osg::ref_ptr< osg::Material > material( new osg::Material() );
        material->setDiffuse(   osg::Material::FRONT, osg::Vec4( 1.0, 1.0, 1.0, 1.0 ) );
        material->setSpecular(  osg::Material::FRONT, osg::Vec4( 0.0, 0.0, 0.0, 1.0 ) );
        material->setAmbient(   osg::Material::FRONT, osg::Vec4( 0.1, 0.1, 0.1, 1.0 ) );
        material->setEmission(  osg::Material::FRONT, osg::Vec4( 0.0, 0.0, 0.0, 1.0 ) );
        material->setShininess( osg::Material::FRONT, 25.0 );
        state->setAttribute( material );
    }

    surfaceGeometry->setUseDisplayList( false );
    surfaceGeometry->setUseVertexBufferObjects( true );

    // ------------------------------------------------
    // Shader stuff

    osg::ref_ptr< WGEShader > shader = osg::ref_ptr< WGEShader >( new WGEShader( "WMIsosurface", m_localPath ) );
    shader->addPreprocessor( WGEShaderPreprocessor::SPtr(
        new WGEShaderPropertyDefineOptions< WPropBool >( m_useTextureProp, "COLORMAPPING_DISABLED", "COLORMAPPING_ENABLED" ) )
    );
    state->addUniform( new WGEPropertyUniform< WPropInt >( "u_opacity", m_opacityProp ) );
    shader->apply( m_surfaceGeode );

    // enable transparency
    wge::enableTransparency( m_surfaceGeode );

    // Colormapping
    WGEColormapping::apply( m_surfaceGeode, shader );

    m_moduleNode->insert( m_surfaceGeode );
    if( !m_moduleNodeInserted )
    {
        WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_moduleNode );
        m_moduleNodeInserted = true;
    }

    m_moduleNode->addUpdateCallback( new WGEFunctorCallback< osg::Node >( boost::bind( &WMIsosurface::updateGraphicsCallback, this ) ) );
}

void WMIsosurface::updateGraphicsCallback()
{
    boost::unique_lock< boost::shared_mutex > lock;
    lock = boost::unique_lock< boost::shared_mutex >( m_updateLock );

    if( m_surfaceColor->changed() )
    {
        osg::ref_ptr< osg::Vec4Array > colors( new osg::Vec4Array );

        colors->push_back( m_surfaceColor->get( true ) );
        osg::ref_ptr< osg::Geometry > surfaceGeometry = m_surfaceGeode->getDrawable( 0 )->asGeometry();
        surfaceGeometry->setColorArray( colors );
        surfaceGeometry->setColorBinding( osg::Geometry::BIND_OVERALL );
    }

    lock.unlock();
}
