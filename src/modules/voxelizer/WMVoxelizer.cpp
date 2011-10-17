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

#include <algorithm>
#include <iomanip>
#include <list>
#include <string>
#include <utility>
#include <vector>

#include <boost/shared_ptr.hpp>

#include <osg/Geode>
#include <osg/Geometry>

#include "core/common/datastructures/WFiber.h"
#include "core/common/WColor.h"
#include "core/common/WLogger.h"
#include "core/common/WPropertyHelper.h"
#include "core/dataHandler/datastructures/WFiberCluster.h"
#include "core/dataHandler/WDataSetFiberVector.h"
#include "core/dataHandler/WDataSetScalar.h"
#include "core/dataHandler/WSubject.h"
#include "core/dataHandler/WGridTransformOrtho.h"
#include "core/graphicsEngine/WGEGeodeUtils.h"
#include "core/graphicsEngine/WGEGeometryUtils.h"
#include "core/graphicsEngine/WGEManagedGroupNode.h"
#include "core/graphicsEngine/WGEUtils.h"
#include "core/kernel/WKernel.h"
#include "core/kernel/WModuleInputData.h"
#include "WBresenham.h"
#include "WBresenhamDBL.h"
#include "WCenterlineParameterization.h"
#include "WIntegrationParameterization.h"
#include "WMVoxelizer.h"
#include "WMVoxelizer.xpm"
#include "WRasterAlgorithm.h"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMVoxelizer )

WMVoxelizer::WMVoxelizer()
    : WModule(),
      m_fullUpdate( new WCondition() )
{
}

WMVoxelizer::~WMVoxelizer()
{
}

boost::shared_ptr< WModule > WMVoxelizer::factory() const
{
    return boost::shared_ptr< WModule >( new WMVoxelizer() );
}

const char** WMVoxelizer::getXPMIcon() const
{
    return voxelizer_xpm;
}

void WMVoxelizer::properties()
{
    m_antialiased     = m_properties->addProperty( "Antialiasing", "Enable/Disable antialiased drawing of voxels.", true, m_fullUpdate );
    m_voxelsPerUnit   = m_properties->addProperty( "Voxels per Unit", "Specified the number of voxels per unit in the coordinate system. This "
                                                                       "is useful to increase the resolution of the grid", 1, m_fullUpdate );
    // for selecting the parameterization method
    m_paramAlgoSelections = boost::shared_ptr< WItemSelection >( new WItemSelection() );
    m_paramAlgoSelections->addItem( "No Parameterization", "Disable parameterization." );          // NOTE: you can add XPM images here.
    m_paramAlgoSelections->addItem( "By Longest Line", "Use the longest line and parameterize the bundle along it." );
    m_paramAlgoSelections->addItem( "By Centerline", "Use the centerline and parameterize the bundle along it." );
    m_paramAlgoSelections->addItem( "By Integration", "Integrate along the voxelized line." );
    m_parameterAlgo   = m_properties->addProperty( "Parameterization", "Select the parameterization algorithm.",
                                                   m_paramAlgoSelections->getSelectorFirst(), m_fullUpdate );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_parameterAlgo );
    WPropertyHelper::PC_NOTEMPTY::addTo( m_parameterAlgo );

    // for selecting the rasterization method
    boost::shared_ptr< WItemSelection > rasterAlgos( new WItemSelection() );
    rasterAlgos->addItem( "Bresenham", "Voxelization with 3D Bresenham" );
    m_rasterAlgo = m_properties->addProperty( "Rasterization", "Which method for rasterizing the tracts", rasterAlgos->getSelectorFirst(),
            m_fullUpdate );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_rasterAlgo );
    WPropertyHelper::PC_NOTEMPTY::addTo( m_rasterAlgo );

    WModule::properties();
}

void WMVoxelizer::connectors()
{
    m_tractIC = WModuleInputData< const WDataSetFibers >::createAndAdd( shared_from_this(), "tractInput", "Deterministic tracts" );
    m_clusterIC = WModuleInputData< const WFiberCluster >::createAndAdd( shared_from_this(), "clusterInput", "A subset (e.g. a cluster) of tracts" );
    m_voxelizedOC = WModuleOutputData< WDataSetScalar >::createAndAdd( shared_from_this(), "voxelOutput", "The voxelized data set" );
    m_paramOC = WModuleOutputData< WDataSetScalar >::createAndAdd( shared_from_this(), "parameterizationOutput",
                                                                                  "The parameter field for the voxelized fibers." );
    WModule::connectors();  // call WModules initialization
}

void WMVoxelizer::moduleMain()
{
    m_moduleState.setResetable();
    m_moduleState.add( m_tractIC->getDataChangedCondition() );
    m_moduleState.add( m_clusterIC->getDataChangedCondition() );
    m_moduleState.add( m_fullUpdate );

    m_rootNode = new WGEManagedGroupNode( m_active );
    m_rootNode->getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_rootNode );

    ready();

    while( !m_shutdownFlag() ) // loop until the module container requests the module to quit
    {
        bool selectionPresent = m_clusterIC->getData();

        if( !m_tractIC->getData() )
        {
            continue;
        }

        size_t numTracts = ( selectionPresent ? m_clusterIC->getData()->size() : m_tractIC->getData()->size() );

        infoLog() << "Start voxelization with: " << numTracts << " tracts";

        boost::array< boost::shared_ptr< WDataSetScalar >, 2 > result = generateDatasets( m_tractIC->getData(), m_clusterIC->getData() );

        if( !result.empty() )
        {
            m_voxelizedOC->updateData( result[0] );
            if( result.size() == 2 ) // parameterized dataset available
            {
                m_paramOC->updateData( result[1] );
            }

            m_rootNode->clear();
            m_rootNode->insert( genDataSetGeode( result[0] ) );
        }

        infoLog() << "Finished.";

        m_moduleState.wait(); // waits for firing of m_moduleState ( dataChanged, shutdown, etc. )
    }

    m_rootNode->clear();

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode );
}

boost::shared_ptr< WGridRegular3D > WMVoxelizer::constructGrid( boost::shared_ptr< const WDataSetFibers > tracts,
        boost::shared_ptr< const WFiberCluster > cluster ) const
{
    WBoundingBox bb;
    if( cluster )
    {
        bb = cluster->getBoundingBox();
    }
    else
    {
        bb = tracts->getBoundingBox();
    }

    int32_t nbVoxelsPerUnit = m_voxelsPerUnit->get( true );

    // TODO(math): remove hardcoded meta grid here.
    // the "+1" in the following three statements is because there may be some more voxels
    // The first and last voxel are only half sized! hence one more position is needed
    size_t nbPosX = std::ceil( bb.xMax() - bb.xMin() ) + 1;
    size_t nbPosY = std::ceil( bb.yMax() - bb.yMin() ) + 1;
    size_t nbPosZ = std::ceil( bb.zMax() - bb.zMin() ) + 1;

    WMatrix< double > mat( 4, 4 );
    mat.makeIdentity();
    mat( 0, 0 ) = mat( 1, 1 ) = mat( 2, 2 ) = 1.0 / nbVoxelsPerUnit;
    mat( 0, 3 ) = bb.getMin()[ 0 ];
    mat( 1, 3 ) = bb.getMin()[ 1 ];
    mat( 2, 3 ) = bb.getMin()[ 2 ];

    WGridTransformOrtho transform( mat );

    boost::shared_ptr< WGridRegular3D > grid( new WGridRegular3D( nbVoxelsPerUnit * nbPosX,
                                                                  nbVoxelsPerUnit * nbPosY,
                                                                  nbVoxelsPerUnit * nbPosZ,
                                                                  transform ) );
    debugLog() << "Created grid of size: " << grid->size();
    return grid;
}

boost::array< boost::shared_ptr< WDataSetScalar >, 2 > WMVoxelizer::generateDatasets(
        boost::shared_ptr< const WDataSetFibers > tracts,
        boost::shared_ptr< const WFiberCluster > cluster ) const
{
    boost::array< boost::shared_ptr< WDataSetScalar >, 2 > result; // unusable instances

    if( !tracts ) // mean while input connector has changed => abort
    {
        return result;
    }

    boost::shared_ptr< WGridRegular3D > grid = constructGrid( tracts, cluster );

    boost::shared_ptr< WRasterAlgorithm > rasterAlgo;
    std::string rasterName = m_rasterAlgo->get().at( 0 )->getName();
    if( rasterName == "Bresenham" )
    {
        rasterAlgo = boost::shared_ptr< WBresenham >( new WBresenham( grid, m_antialiased->get() ) );
    }
    else
    {
        errorLog() << "Invalid rasterization algorithm selected: " << rasterName << " aborting.";
        return result;
    }

    // decide which param algo to use:
    boost::shared_ptr< WRasterParameterization > paramAlgo;
    std::string paramName = m_parameterAlgo->get( true ).at( 0 )->getName();
    if( paramName == "By Longest Line" )
    {
        paramAlgo = boost::shared_ptr< WRasterParameterization >( new WCenterlineParameterization( grid, longestLine( tracts, cluster ) ) );
    }
    else if( paramName == "By Centerline" )
    {
        paramAlgo = boost::shared_ptr< WRasterParameterization >( new WCenterlineParameterization( grid, centerLine( tracts, cluster ) ) );
    }
    else if( paramName == "By Integration" )
    {
        paramAlgo = boost::shared_ptr< WRasterParameterization >( new WIntegrationParameterization( grid ) );
    }
    debugLog() << paramName << " as parameterization method selected.";

    if( paramAlgo )
    {
        rasterAlgo->addParameterizationAlgorithm( paramAlgo );
    }

    raster( rasterAlgo, tracts, cluster );

    result[0] = rasterAlgo->generateDataSet();

    if( paramAlgo )
    {
        result[1] = paramAlgo->getDataSet();
    }

    return result;
}

boost::shared_ptr< WFiber > WMVoxelizer::longestLine( boost::shared_ptr< const WDataSetFibers > tracts,
        boost::shared_ptr< const WFiberCluster > cluster ) const
{
    if( cluster )
    {
        return cluster->getLongestLine();
    }
    return longestLine( tracts );
}

boost::shared_ptr< WFiber > WMVoxelizer::centerLine( boost::shared_ptr< const WDataSetFibers > tracts,
        boost::shared_ptr< const WFiberCluster > cluster ) const
{
    if( cluster )
    {
        return cluster->getCenterLine();
    }
    return centerLine( tracts );
}

void WMVoxelizer::raster( boost::shared_ptr< WRasterAlgorithm > algo, boost::shared_ptr< const WDataSetFibers > tracts,
        boost::shared_ptr< const WFiberCluster > cluster ) const
{
    // for each tract apply a call to algo->raster( tract );

    if( cluster )
    {
        boost::shared_ptr< const WDataSetFiberVector > clusterTracts = cluster->getDataSetReference();
        const std::list< size_t >& tractIDs = cluster->getIndices();
        std::list< size_t >::const_iterator cit = tractIDs.begin();
        for( cit = tractIDs.begin(); cit != tractIDs.end(); ++cit )
        {
            algo->raster( clusterTracts->at( *cit ) );
        }
    }
    else
    {
        boost::shared_ptr< WDataSetFiberVector > allTracts( new WDataSetFiberVector( tracts ) );
        for( WDataSetFiberVector::const_iterator cit = allTracts->begin(); cit != allTracts->end(); ++cit )
        {
            algo->raster( *cit );
        }
    }

    algo->finished();
}

osg::ref_ptr< osg::Geode > WMVoxelizer::genDataSetGeode( boost::shared_ptr< WDataSetScalar > dataset ) const
{
    using osg::ref_ptr;
    ref_ptr< osg::Vec3Array > vertices = ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
    ref_ptr< osg::Vec4Array > colors = ref_ptr< osg::Vec4Array >( new osg::Vec4Array );
    ref_ptr< osg::Geometry > geometry = ref_ptr< osg::Geometry >( new osg::Geometry );
    ref_ptr< osg::Vec3Array > normals = ref_ptr< osg::Vec3Array >( new osg::Vec3Array );

    // cycle through all positions in the dataSet
    boost::shared_ptr< WValueSet< double > > valueset = boost::shared_dynamic_cast< WValueSet< double > >( dataset->getValueSet() );
    WAssert( valueset != 0, "No scalar double valueset was given while generating the dataset geode" );
    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( dataset->getGrid() );
    WAssert( grid != 0, "No WGridRegular3D was given while generating the dataset geode"  );
    const std::vector< double >& values = *valueset->rawDataVectorPointer();
    for( size_t i = 0; i < values.size(); ++i )
    {
        if( values[i] != 0.0 )
        {
            WPosition pos = grid->getPosition( i );
            boost::shared_ptr< std::vector< WPosition > > voxelCornerVertices = grid->getVoxelVertices( pos, 0.01 );
            osg::ref_ptr< osg::Vec3Array > ver = wge::generateCuboidQuads( *voxelCornerVertices );
            vertices->insert( vertices->end(), ver->begin(), ver->end() );
            osg::ref_ptr< osg::Vec3Array > nor = wge::generateCuboidQuadNormals( *voxelCornerVertices );
            normals->insert( normals->end(), nor->begin(), nor->end() );
            geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::QUADS, vertices->size() - ver->size(), ver->size() ) );
            for( size_t j = 0; j < ver->size(); ++j )
            {
                double transparency = ( values[i] <= 1.0 ? values[i] : 1.0 );
                colors->push_back( WColor( 1.0, 0.0, 0.0, transparency ) );
            }
        }
    }

    geometry->setVertexArray( vertices );
    colors->push_back( WColor( 1.0, 0.0, 0.0, 0.1 ) );
    geometry->setColorArray( colors );
    geometry->setColorBinding( osg::Geometry::BIND_PER_VERTEX );
    geometry->setNormalArray( normals );
    geometry->setNormalBinding( osg::Geometry::BIND_PER_PRIMITIVE );
    osg::ref_ptr< osg::Geode > geode = osg::ref_ptr< osg::Geode >( new osg::Geode );
    geode->addDrawable( geometry );

    osg::StateSet* state = geode->getOrCreateStateSet();

    // Enable blending, select transparent bin.
    state->setMode( GL_BLEND, osg::StateAttribute::ON );
    state->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

    // Enable depth test so that an opaque polygon will occlude a transparent one behind it.
    state->setMode( GL_DEPTH_TEST, osg::StateAttribute::ON );

    // Conversely, disable writing to depth buffer so that a transparent polygon will allow polygons behind it to shine through.
    // OSG renders transparent polygons after opaque ones.
    osg::Depth* depth = new osg::Depth;
    depth->setWriteMask( false );
    state->setAttributeAndModes( depth, osg::StateAttribute::ON );

    return geode;
}
