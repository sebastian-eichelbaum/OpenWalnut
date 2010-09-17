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

#include <boost/regex.hpp>

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Material>
#include <osg/StateSet>
#include <osg/StateAttribute>
#include <osg/PolygonMode>
#include <osg/LightModel>

#include "../../common/WAssert.h"
#include "../../dataHandler/WDataSetScalar.h"
#include "../../graphicsEngine/WGEUtils.h"
#include "../../graphicsEngine/algorithms/WMarchingCubesAlgorithm.h"
#include "../../kernel/WKernel.h"

#include "WMAtlasSurfaces.h"
#include "atlas.xpm"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMAtlasSurfaces )

WMAtlasSurfaces::WMAtlasSurfaces():
    WModule(),
    m_dataSet(),
    m_moduleNode( new WGEGroupNode() ),
    m_dirty( false ),
    m_labelsLoaded( false )
{
}

WMAtlasSurfaces::~WMAtlasSurfaces()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMAtlasSurfaces::factory() const
{
    // See "src/modules/template/" for an extensively documented example.
    return boost::shared_ptr< WModule >( new WMAtlasSurfaces() );
}

const char** WMAtlasSurfaces::getXPMIcon() const
{
    return atlas_xpm;
}

const std::string WMAtlasSurfaces::getName() const
{
    // Specify your module name here. This name must be UNIQUE!
    return "Atlas Surfaces";
}

const std::string WMAtlasSurfaces::getDescription() const
{
    // Specify your module description here. Be detailed. This text is read by the user.
    // See "src/modules/template/" for an extensively documented example.
    return "Someone should add some documentation here. "
    "Probably the best person would be the modules's creator, i.e. \"schurade\"";
}

void WMAtlasSurfaces::connectors()
{
    // initialize connectors
    m_input = boost::shared_ptr< WModuleInputData < WDataSetScalar > >(
        new WModuleInputData< WDataSetScalar >( shared_from_this(), "in", "Dataset to create atlas surfaces from." ) );
    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_input );

    // call WModules initialization
    WModule::connectors();
}

void WMAtlasSurfaces::properties()
{
    WPropertyBase::PropertyChangeNotifierType propertyCallback = boost::bind( &WMAtlasSurfaces::propertyChanged, this );
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );
}

void WMAtlasSurfaces::moduleMain()
{
    // use the m_input "data changed" flag
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );

    // signal ready state
    ready();

    // loop until the module container requests the module to quit
    while ( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        if ( m_shutdownFlag() )
        {
            break;
        }

        if( m_dataSet != m_input->getData() )
        {
            // acquire data from the input connector
            m_dataSet = m_input->getData();
            switch( ( *m_dataSet ).getValueSet()->getDataType() )
            {
                case W_DT_UNSIGNED_CHAR:
                case W_DT_INT16:
                case W_DT_SIGNED_INT:
                    debugLog() << "Starting creating region meshes";
                    createSurfaces();
                    debugLog() << "Finished creating region meshes";
                    debugLog() << "Starting creating OSG nodes";
                    createOSGNode();
                    debugLog() << "Finished creating OSG nodes";
                    m_dirty = true;
                    break;
                case W_DT_FLOAT:
                case W_DT_DOUBLE:
                default:
                    WAssert( false, "Wrong data type in AtlasSurfaces module" );
            }
        }
    }
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_moduleNode );
}

void WMAtlasSurfaces::createSurfaces()
{
    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( m_dataSet->getGrid() );

    std::string fn = m_dataSet->getFileName();
    std::string ext( ".nii.gz" );
    std::string csvExt( ".csv" );
    fn.replace( fn.find( ext ), ext.size(), csvExt );
    loadLabels( fn );

    size_t order = ( *m_dataSet ).getValueSet()->order();
    size_t vDim = ( *m_dataSet ).getValueSet()->dimension();
    std::vector< float > data;

    switch( ( *m_dataSet ).getValueSet()->getDataType() )
    {
        case W_DT_UNSIGNED_CHAR:
        {
            boost::shared_ptr< WValueSet< unsigned char > > vals;
            vals =  boost::shared_dynamic_cast< WValueSet< unsigned char > >( ( *m_dataSet ).getValueSet() );
            WAssert( vals, "Data type and data type indicator must fit." );

            infoLog() << "creating " << m_dataSet->getMax() << " regions" << std::endl;

            m_possibleSelections = boost::shared_ptr< WItemSelection >( new WItemSelection() );

            boost::shared_ptr<WProgressCombiner> newProgress = boost::shared_ptr<WProgressCombiner>( new WProgressCombiner() );

            boost::shared_ptr<WProgress>pro = boost::shared_ptr<WProgress>( new WProgress( "dummy", m_dataSet->getMax() ) );

            m_progress->addSubProgress( pro );
            for (size_t i = 1; i < m_dataSet->getMax() + 1; ++i )
            //for (size_t i = 1; i < 10; ++i )
            {
                data = cutArea( ( *m_dataSet ).getGrid(), vals, i );
                boost::shared_ptr< WValueSet< float > > newValueSet = boost::shared_ptr< WValueSet< float > >(
                        new WValueSet< float >( order, vDim, data, W_DT_FLOAT ) );
                WMarchingCubesAlgorithm mcAlgo;

                boost::shared_ptr< WTriangleMesh2 >triMesh = mcAlgo.generateSurface( grid->getNbCoordsX(), grid->getNbCoordsY(), grid->getNbCoordsZ(),
                                                        grid->getTransformationMatrix(),
                                                        newValueSet->rawDataVectorPointer(),
                                                        0.9,
                                                        newProgress );
                //triMesh->doLoopSubD();
                if ( triMesh->vertSize() != 0 )
                {
                    m_regionMeshes.push_back( triMesh );
                    m_regionMeshIds.push_back( i );

                    std::string label = boost::lexical_cast<std::string>( i ) + std::string( " " ) + m_labels[i].second;
                    m_possibleSelections->addItem( label, "" );
                }
                ++*pro;
            }
            pro->finish();
            m_aMultiSelection  = m_properties->addProperty( "Regions", "Regions", m_possibleSelections->getSelectorAll(),
                                                                    m_propCondition );

            break;
        }
        case W_DT_INT16:
        {
            boost::shared_ptr< WValueSet< int16_t > > vals;
            vals =  boost::shared_dynamic_cast< WValueSet< int16_t > >( ( *m_dataSet ).getValueSet() );
            WAssert( vals, "Data type and data type indicator must fit." );

            infoLog() << "creating " << m_dataSet->getMax() << " regions" << std::endl;

            for (size_t i = 0; i < m_dataSet->getMax(); ++i )
            {
                data = cutArea( ( *m_dataSet ).getGrid(), vals, i );
                boost::shared_ptr< WValueSet< float > > newValueSet = boost::shared_ptr< WValueSet< float > >(
                        new WValueSet< float >( order, vDim, data, W_DT_FLOAT ) );
                WMarchingCubesAlgorithm mcAlgo;
                boost::shared_ptr< WTriangleMesh2 >triMesh = mcAlgo.generateSurface( grid->getNbCoordsX(), grid->getNbCoordsY(), grid->getNbCoordsZ(),
                                                        grid->getTransformationMatrix(),
                                                        newValueSet->rawDataVectorPointer(),
                                                        0.9,
                                                        m_progress );
                m_regionMeshes.push_back( triMesh );
            }

            break;
        }
        case W_DT_SIGNED_INT:
        {
            boost::shared_ptr< WValueSet< int32_t > > vals;
            vals =  boost::shared_dynamic_cast< WValueSet< int32_t > >( ( *m_dataSet ).getValueSet() );
            WAssert( vals, "Data type and data type indicator must fit." );
            std::cout << "creating " << m_dataSet->getMax() << " regions" << std::endl;
            for (size_t i = 0; i < m_dataSet->getMax(); ++i )
            {
                data = cutArea( ( *m_dataSet ).getGrid(), vals, i );
                boost::shared_ptr< WValueSet< float > > newValueSet = boost::shared_ptr< WValueSet< float > >(
                        new WValueSet< float >( order, vDim, data, W_DT_FLOAT ) );
                WMarchingCubesAlgorithm mcAlgo;
                boost::shared_ptr< WTriangleMesh2 >triMesh = mcAlgo.generateSurface( grid->getNbCoordsX(), grid->getNbCoordsY(), grid->getNbCoordsZ(),
                                                        grid->getTransformationMatrix(),
                                                        newValueSet->rawDataVectorPointer(),
                                                        0.9,
                                                        m_progress );
                m_regionMeshes.push_back( triMesh );
            }
            break;
        }
        case W_DT_FLOAT:
        case W_DT_DOUBLE:
        default:
            WAssert( false, "Unknown data type in MarchingCubes module" );
    }
}

template< typename T > std::vector< float >  WMAtlasSurfaces::cutArea( boost::shared_ptr< WGrid > inGrid,
        boost::shared_ptr< WValueSet< T > > vals, unsigned int number )
{
    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( inGrid );

    size_t nx = grid->getNbCoordsX();
    size_t ny = grid->getNbCoordsY();
    size_t nz = grid->getNbCoordsZ();

//    double dx = grid->getOffsetX();
//    double dy = grid->getOffsetY();
//    double dz = grid->getOffsetZ();

    std::vector< float >newVals( nx * ny * nz, 0 );

    for ( size_t z = 0; z < nz; ++z )
    {
        for ( size_t y = 0 ; y < ny; ++y )
        {
            for ( size_t x = 0 ; x < nx; ++x )
            {
                 if ( static_cast<unsigned int>( vals->getScalar( x + nx * y + nx * ny * z ) ) == number )
                 {
                     newVals[ x + nx * y + nx * ny * z ] = 1.0;
                 }
            }
        }
    }
    return newVals;
}

void WMAtlasSurfaces::createOSGNode()
{
    for ( size_t i = 0; i < m_regionMeshes.size(); ++i )
    {
        osg::Geometry* surfaceGeometry = new osg::Geometry();
        osg::ref_ptr< osg::Geode > outputGeode = osg::ref_ptr< osg::Geode >( new osg::Geode );

        size_t id = m_regionMeshIds[i];
        std::string label = boost::lexical_cast<std::string>( id ) + std::string( " " ) + m_labels[id].second;
        outputGeode->setName( label );

        surfaceGeometry->setUseDisplayList( false );
        surfaceGeometry->setUseVertexBufferObjects( true );

        surfaceGeometry->setVertexArray( m_regionMeshes[i]->getVertexArray() );

        // ------------------------------------------------
        // normals
        surfaceGeometry->setNormalArray( m_regionMeshes[i]->getVertexNormalArray() );
        surfaceGeometry->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );

        // ------------------------------------------------
        // colors
        osg::Vec4Array* colors = new osg::Vec4Array;

        colors->push_back( wge::osgColor( wge::createColorFromIndex( i + 1 ) ) );

        surfaceGeometry->setColorArray( colors );
        surfaceGeometry->setColorBinding( osg::Geometry::BIND_OVERALL );

        osg::DrawElementsUInt* surfaceElement = new osg::DrawElementsUInt( osg::PrimitiveSet::TRIANGLES, 0 );

        std::vector< size_t >tris = m_regionMeshes[i]->getTriangles();
        surfaceElement->reserve( tris.size() );

        for( unsigned int vertId = 0; vertId < tris.size(); ++vertId )
        {
            surfaceElement->push_back( tris[vertId] );
        }
        surfaceGeometry->addPrimitiveSet( surfaceElement );
        outputGeode->addDrawable( surfaceGeometry );

        osg::StateSet* state = outputGeode->getOrCreateStateSet();
        osg::ref_ptr<osg::LightModel> lightModel = new osg::LightModel();
        lightModel->setTwoSided( true );
        state->setAttributeAndModes( lightModel.get(), osg::StateAttribute::ON );
        state->setMode(  GL_BLEND, osg::StateAttribute::ON );

    //    {
    //        osg::ref_ptr< osg::Material > material = new osg::Material();
    //        material->setDiffuse(   osg::Material::FRONT, osg::Vec4( 1.0, 1.0, 1.0, 1.0 ) );
    //        material->setSpecular(  osg::Material::FRONT, osg::Vec4( 0.0, 0.0, 0.0, 1.0 ) );
    //        material->setAmbient(   osg::Material::FRONT, osg::Vec4( 0.1, 0.1, 0.1, 1.0 ) );
    //        material->setEmission(  osg::Material::FRONT, osg::Vec4( 0.0, 0.0, 0.0, 1.0 ) );
    //        material->setShininess( osg::Material::FRONT, 25.0 );
    //        state->setAttribute( material );
    //    }

        m_moduleNode->insert( outputGeode );
    }
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_moduleNode );
    m_moduleNode->addUpdateCallback( new AtlasSurfaceNodeCallback( this ) );
}

void WMAtlasSurfaces::propertyChanged()
{
    m_dirty = true;
}

void WMAtlasSurfaces::updateGraphics()
{
    if ( !m_dirty && !m_aMultiSelection->changed() )
    {
        return;
    }

    WItemSelector s = m_aMultiSelection->get( true );
    for ( size_t i = 0; i < m_moduleNode->getNumChildren(); ++i )
    {
        m_moduleNode->getChild( i )->setNodeMask( 0x0 );

        for ( size_t j = 0; j < s.size(); ++j )
        {
            if ( s.getItemIndexOfSelected(j) == i )
            {
                m_moduleNode->getChild( i )->setNodeMask( 0xFFFFFFFF );
            }
        }
    }

    m_dirty = false;
}

std::vector< std::string > WMAtlasSurfaces::readFile( const std::string fileName )
{
    std::ifstream ifs( fileName.c_str(), std::ifstream::in );

    std::vector< std::string > lines;

    std::string line;

    while ( !ifs.eof() )
    {
        getline( ifs, line );

        lines.push_back( std::string( line ) );
    }

    ifs.close();

    return lines;
}

void WMAtlasSurfaces::loadLabels( std::string fileName )
{
    std::vector<std::string> lines;

    lines = readFile( fileName );

    std::vector<std::string>svec;

    for ( size_t i = 0; i < lines.size(); ++i )
    {
        svec.clear();
        boost::regex reg( "," );
        boost::sregex_token_iterator it( lines[i].begin(), lines[i].end(), reg, -1 );
        boost::sregex_token_iterator end;
        while ( it != end )
        {
            svec.push_back( *it++ );
        }
        if ( svec.size() == 3 )
        {
            std::pair< std::string, std::string >newLabel( svec[1], svec[2] );
            m_labels[boost::lexical_cast<size_t>( svec[0] )] = newLabel;
        }
    }
    m_labelsLoaded = true;
}
