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

//---------------------------------------------------------------------------
//
// This file is also part of the
// Whole-Brain Connectivity-Based Hierarchical Parcellation Project
// David Moreno-Dominguez
// Ralph Schurade
// moreno@cbs.mpg.de
// www.cbs.mpg.de/~moreno
//
//---------------------------------------------------------------------------

#include <fstream>
#include <string>
#include <vector>

#include <boost/unordered_map.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>

#include "core/common/WStringUtils.h"
#include "core/kernel/WKernel.h"
#include "core/common/WIOTools.h"
#include "core/graphicsEngine/WGEUtils.h"

#include "WMPartition2Mesh.h"

WMPartition2Mesh::WMPartition2Mesh():
    WModule()
{
}

WMPartition2Mesh::~WMPartition2Mesh()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMPartition2Mesh::factory() const
{
    // See "src/modules/template/" for an extensively documented example.
    return boost::shared_ptr< WModule >( new WMPartition2Mesh() );
}

const std::string WMPartition2Mesh::getName() const
{
    // Specify your module name here. This name must be UNIQUE!
    return "Partition To Mesh";
}

const std::string WMPartition2Mesh::getDescription() const
{
    // Specify your module description here. Be detailed. This text is read by the user.
    // See "src/modules/template/" for an extensively documented example.
    return "Colors freesurfer meshes with results from clustering";
}

void WMPartition2Mesh::connectors()
{
    m_colorInput = WModuleInputData < WDataSetVector  >::createAndAdd( shared_from_this(), "colors", "The color vector." );
    m_coordInput = WModuleInputData < WDataSetScalar  >::createAndAdd( shared_from_this(), "coords", "The coordinate vector." );
    m_meshInput1 = WModuleInputData< WTriangleMesh >::createAndAdd( shared_from_this(), "mesh1", "The reference mesh" );
    m_meshInput2 = WModuleInputData< WTriangleMesh >::createAndAdd( shared_from_this(), "mesh2", "The mesh to color" );

    m_meshOutput = WModuleOutputData<WTriangleMesh>::createAndAdd( shared_from_this(), "mesh3", "The mesh to display" );

    WModule::connectors();
}

void WMPartition2Mesh::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    // Info properties
    m_infoTotalVertices = m_infoProperties->addProperty( "Total vertices: ", "", 0 );
    m_infoUnassignedVertices = m_infoProperties->addProperty( "Unassigned vertices", "", 0 );

    m_propProjectDistance = m_properties->addProperty
            ( "Max. project distance", "Maximum distance for projecting colors onto the surface", 2.5, m_propCondition );
    m_propProjectDistance ->setMin( 0.5 );
    m_propProjectDistance->setMax( 3.5 );
    m_propNonActiveColor = m_properties->addProperty( "Inactive color", "color for inactive clusters", WColor( 0.5, 0.5, 0.5, 1 ), m_propCondition );
    m_propHoleColor = m_properties->addProperty( "Unassigned color", "color for unassigned vertices", WColor( 0.9, 0.9, 0.9, 1 ), m_propCondition );


    m_propLabelFolder= m_properties->addProperty( "Output folder", "", boost::filesystem::path() );
    WPropertyHelper::PC_PATHEXISTS::addTo( m_propLabelFolder );
    WPropertyHelper::PC_ISDIRECTORY::addTo( m_propLabelFolder );
    m_propAnnotationFile= m_properties->addProperty( "Output file", "", boost::filesystem::path() );
    m_monochrome = m_properties->addProperty( "Monochrome", "Uses the non-active color for all partitions", false );
    m_propWriteTrigger = m_properties->addProperty( "Write Labels",  "Press!", WPVBaseTypes::PV_TRIGGER_READY, m_propCondition );
    WModule::properties();

    m_propAnnotationFile->setHidden( true );
}

void WMPartition2Mesh::requirements()
{
    // Put the code for your requirements here. See "src/modules/template/" for an extensively documented example.
}

void WMPartition2Mesh::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_coordInput->getDataChangedCondition() );
    m_moduleState.add( m_colorInput->getDataChangedCondition() );
    m_moduleState.add( m_meshInput1->getDataChangedCondition() );
    m_moduleState.add( m_meshInput2->getDataChangedCondition() );
    m_moduleState.add( m_active->getUpdateCondition() );
    m_moduleState.add( m_propCondition );
    m_blankOutMesh = false;

    ready();

    // wait for a dataset to be connected, most likely an anatomy dataset
    while( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        boost::shared_ptr< WDataSetScalar > newCoordVector = m_coordInput->getData();
        bool coordsChanged = ( m_coordinateVector != newCoordVector );
        bool coordsValid   = ( newCoordVector != NULL );

        if( coordsValid )
        {
            if( coordsChanged )
            {
                infoLog() << "Partition2Mesh: coords changed";
                m_coordinateVector = newCoordVector;
            }
        }

        boost::shared_ptr< WDataSetVector > newColorVector = m_colorInput->getData();
        bool colorsChanged = ( m_colorVector != newColorVector );
        bool colorsValid   = ( newColorVector != NULL );

        if( colorsValid )
        {
            if( colorsChanged )
            {
                //infoLog() << "Partition2Mesh: colors changed";
                m_colorVector = newColorVector;
            }
        }

        boost::shared_ptr< WTriangleMesh > newRefMesh = m_meshInput1->getData();
        bool refMeshChanged = ( m_referenceMesh != newRefMesh );
        bool refMeshValid   = ( newRefMesh != NULL );

        if( refMeshValid )
        {
            if( refMeshChanged )
            {
                infoLog() << "Partition2Mesh: ref mesh changed";
                m_referenceMesh = newRefMesh;
                m_infoTotalVertices->set( m_referenceMesh->vertSize() );
            }
        }

        boost::shared_ptr< WTriangleMesh > newOutMesh = m_meshInput2->getData();
        bool outMeshChanged = ( m_outMesh != newOutMesh );
        bool outMeshValid   = ( newOutMesh != NULL );

        if( outMeshValid )
        {
            if( outMeshChanged )
            {
                //infoLog() << "Partition2Mesh: out mesh changed";
                m_outMesh = newOutMesh;
                m_meshOutput->updateData( m_outMesh );
                m_blankOutMesh = true;
            }
        }

        bool allInputsValid = ( coordsValid && refMeshValid );
        bool canRecalc = ( coordsChanged || refMeshChanged || m_propProjectDistance->changed() ) && allInputsValid;

        //if( m_propRefTrigger->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )

        if( canRecalc )
        {
            infoLog() << "Partition2Mesh: start recalc refs";

            m_datasetSizeX = boost::dynamic_pointer_cast< WGridRegular3D >( m_colorInput->getData()->getGrid() )->getNbCoordsX();
            m_datasetSizeY = boost::dynamic_pointer_cast< WGridRegular3D >( m_colorInput->getData()->getGrid() )->getNbCoordsY();
            m_datasetSizeZ = boost::dynamic_pointer_cast< WGridRegular3D >( m_colorInput->getData()->getGrid() )->getNbCoordsZ();
            std::vector<size_t>volume( m_datasetSizeX * m_datasetSizeY * m_datasetSizeZ, 0 );
            std::cout << "color volume size " <<  m_datasetSizeX * m_datasetSizeY * m_datasetSizeZ << std::endl;

            boost::shared_ptr<WValueSet<int> > coords;
            coords = boost::dynamic_pointer_cast<WValueSet<int> >( ( *m_coordinateVector ).getValueSet() );

            std::cout << "fill volume" << std::endl;
            for( size_t i = 0; i < m_coordinateVector->getGrid()->size() / 3; ++i )
            {
                const size_t id1( coords->getScalar( i*3 ) );
                const size_t id2( coords->getScalar( i*3 + 1 ) * m_datasetSizeX );
                const size_t id3( coords->getScalar( i*3 + 2 ) * m_datasetSizeX * m_datasetSizeY );
                size_t id = id1 + id2 + id3;
                if( id < volume.size() )
                {
                    volume[ id ] = i+1;
                }
                else
                {
                    std::cout << "error" << std::endl;
                }
            }

            m_refs.resize( m_referenceMesh->vertSize() );
            boost::shared_ptr<WGridRegular3D> grid = boost::dynamic_pointer_cast< WGridRegular3D >( m_colorInput->getData()->getGrid() );
            float xs = grid->getOffsetX() / 2.0;
            float ys = grid->getOffsetY() / 2.0;
            float zs = grid->getOffsetZ() / 2.0;
            for( size_t i = 0; i < m_referenceMesh->vertSize(); ++i )
            {
                osg::Vec3 vert = m_referenceMesh->getVertex( i );
                int xd = static_cast<int>( vert.x() );
                int yd = static_cast<int>( vert.y() );
                int zd = static_cast<int>( vert.z() );
                size_t loc = xd + yd * m_datasetSizeX + zd * m_datasetSizeX * m_datasetSizeY;
                m_refs[i] = volume[loc];
                std::vector<size_t>candidates;

                if( m_refs[i] == 0 ) // didn't find a voxel have to keep looking
                {
                    if( ( m_propProjectDistance->get( true ) > 0.5) && ( m_propProjectDistance->get( true ) <= 1.5) )
                    {
                        // look in the 26 nbhood (3x3x3 voxels)

                        std::vector< size_t> neighbours = grid->getNeighbours27( loc );
                        for( size_t k = 0; k < neighbours.size(); ++k )
                        {
                            if( volume[neighbours[k]] != 0 )
                            {
                                candidates.push_back( neighbours[k] );
                            }
                        }

                        std::vector<float>dists;
                        for( size_t l = 0; l < candidates.size(); ++l )
                        {
                            WPosition voxPos = grid->getPosition( candidates[l] );
                            float dist = sqrt( ( vert.x() - voxPos.x() - xs ) *  ( vert.x() - voxPos.x() - xs ) +
                                              ( vert.y() - voxPos.y() - ys ) *  ( vert.y() - voxPos.y() - ys ) +
                                              ( vert.z() - voxPos.z() - zs ) *  ( vert.z() - voxPos.z() - zs ) );
                            dists.push_back( dist );
                        }
                        float curDist = 10;
                        for( size_t l = 0; l < candidates.size(); ++l )
                        {
                            if( dists[l] < curDist )
                            {
                                m_refs[i] = volume[candidates[l]];
                                curDist = dists[l];
                            }
                        }
                        // discard if distance is bigger than specified
                        if( curDist > m_propProjectDistance->get( true ) )
                        {
                            m_refs[i]=0;
                        }
                    }
                    else if( ( m_propProjectDistance->get( true ) > 1.5) && ( m_propProjectDistance->get( true ) <= 2.5) )
                    {
                        // look in the 124 nbhood (5x5x5 voxels)

                        std::vector< size_t> neighbours = grid->getNeighboursRange( loc, 2 );
                        for( size_t k = 0; k < neighbours.size(); ++k )
                        {
                            if( volume[neighbours[k]] != 0 )
                            {
                                candidates.push_back( neighbours[k] );
                            }
                        }

                        std::vector<float>dists;
                        for( size_t l = 0; l < candidates.size(); ++l )
                        {
                            WPosition voxPos = grid->getPosition( candidates[l] );
                            float dist = sqrt( ( vert.x() - voxPos.x() - xs ) *  ( vert.x() - voxPos.x() - xs ) +
                                              ( vert.y() - voxPos.y() - ys ) *  ( vert.y() - voxPos.y() - ys ) +
                                              ( vert.z() - voxPos.z() - zs ) *  ( vert.z() - voxPos.z() - zs ) );
                            dists.push_back( dist );
                        }
                        float curDist = 10;
                        for( size_t l = 0; l < candidates.size(); ++l )
                        {
                            if( dists[l] < curDist )
                            {
                                m_refs[i] = volume[candidates[l]];
                                curDist = dists[l];
                            }
                        }
                        // discard if distance is bigger than specified
                        if( curDist > m_propProjectDistance->get( true ) )
                        {
                            m_refs[i]=0;
                        }
                    }
                    else if( m_propProjectDistance->get( true ) > 2.5)
                    {
                        // look in the 342 nbhood (7x7x7 voxels)

                        std::vector< size_t> neighbours = grid->getNeighboursRange( loc, 3 );
                        for( size_t k = 0; k < neighbours.size(); ++k )
                        {
                            if( volume[neighbours[k]] != 0 )
                            {
                                candidates.push_back( neighbours[k] );
                            }
                        }

                        std::vector<float>dists;
                        for( size_t l = 0; l < candidates.size(); ++l )
                        {
                            WPosition voxPos = grid->getPosition( candidates[l] );
                            float dist = sqrt( ( vert.x() - voxPos.x() - xs ) *  ( vert.x() - voxPos.x() - xs ) +
                                              ( vert.y() - voxPos.y() - ys ) *  ( vert.y() - voxPos.y() - ys ) +
                                              ( vert.z() - voxPos.z() - zs ) *  ( vert.z() - voxPos.z() - zs ) );
                            dists.push_back( dist );
                        }
                        float curDist = 10;
                        for( size_t l = 0; l < candidates.size(); ++l )
                        {
                            if( dists[l] < curDist )
                            {
                                m_refs[i] = volume[candidates[l]];
                                curDist = dists[l];
                            }
                        }
                        // discard if distance is bigger than specified
                        if( curDist > m_propProjectDistance->get( true ) )
                        {
                            m_refs[i]=0;
                        }
                    }
                }
            }
            m_infoUnassignedVertices->set( std::count( m_refs.begin(), m_refs.end(), 0 ) );
            infoLog() << "Partition2Mesh: end recalc refs";
        }

        bool canRecolor = colorsValid && outMeshValid && ( m_refs.size() == m_outMesh->vertSize() );
        if( ( colorsChanged || m_propNonActiveColor->changed( true ) || m_propHoleColor->changed( true ) || m_blankOutMesh ) && canRecolor )
        {
            m_blankOutMesh = false;
            //infoLog() << "Partition2Mesh: write colors to out mesh";
#if 1
            boost::shared_ptr< WTriangleMesh > triMesh( new WTriangleMesh( m_outMesh->vertSize(), m_outMesh->triangleSize() ) );
            for( size_t i = 0; i < m_outMesh->vertSize(); ++i )
            {
                triMesh->addVertex( m_outMesh->getVertex( i ) );
            }
            for( size_t i = 0; i < m_outMesh->triangleSize(); ++i )
            {
                triMesh->addTriangle( m_outMesh->getTriVertId0( i ), m_outMesh->getTriVertId1( i ), m_outMesh->getTriVertId2( i ) );
            }
            triMesh->getNormal( 0 );

            int dimx = boost::dynamic_pointer_cast< WGridRegular3D >( m_colorInput->getData()->getGrid() )->getNbCoordsX();
            int dimy = boost::dynamic_pointer_cast< WGridRegular3D >( m_colorInput->getData()->getGrid() )->getNbCoordsY();
            //int dimz = boost::dynamic_pointer_cast< WGridRegular3D >( m_colorInput->getData()->getGrid() )->getNbCoordsZ();

            boost::shared_ptr<WValueSet<unsigned char> > colors;
            colors = boost::dynamic_pointer_cast<WValueSet<unsigned char> >( ( *m_colorVector ).getValueSet() );
            boost::shared_ptr<WValueSet<int> > coords;
            coords = boost::dynamic_pointer_cast<WValueSet<int> >( ( *m_coordinateVector ).getValueSet() );

            WColor col1( m_propHoleColor->get( true ) );
            for( size_t i = 0; i < triMesh->vertSize(); ++i )
            {
                triMesh->setVertexColor( i, col1 );
            }
            for( size_t i = 0; i < triMesh->vertSize(); ++i )
            {
                size_t id = m_refs[i];
                if( id > 0 )
                {
                    id = id - 1;
                    int x = coords->getScalar( id * 3 );
                    int y = coords->getScalar( id * 3 + 1 );
                    int z = coords->getScalar( id * 3 + 2 );

                    size_t loc = x + y * dimx + z * dimx * dimy;

                    WColor col( colors->getScalar( loc * 3 )/255.0,
                                colors->getScalar( loc * 3 + 1 )/255.0,
                                colors->getScalar( loc * 3 + 2 )/255.0,
                                1.0 );
                    if( col == WColor( 0, 0, 0, 1 ) )
                    {
                        col = m_propNonActiveColor->get( true );
                    }

                    triMesh->setVertexColor( i, col );
                }
            }

            m_outMesh = triMesh;

            // this code shouold work to recolor the mesh, for some reason it does not,
            // and so a new output mesh has to be crated every time colors change
#else
            int dimx = boost::dynamic_pointer_cast< WGridRegular3D >( m_colorInput->getData()->getGrid() )->getNbCoordsX();
            int dimy = boost::dynamic_pointer_cast< WGridRegular3D >( m_colorInput->getData()->getGrid() )->getNbCoordsY();
            //int dimz = boost::dynamic_pointer_cast< WGridRegular3D >( m_colorInput->getData()->getGrid() )->getNbCoordsZ();

            boost::shared_ptr<WValueSet<unsigned char> > colors;
            colors = boost::dynamic_pointer_cast<WValueSet<unsigned char> >( ( *m_colorVector ).getValueSet() );
            boost::shared_ptr<WValueSet<int> > coords;
            coords = boost::dynamic_pointer_cast<WValueSet<int> >( ( *m_coordinateVector ).getValueSet() );

            WColor col1( 0.9, 0.9, 0.9, 1.0 );
            for( size_t i = 0; i < m_outMesh->vertSize(); ++i )
            {
                m_outMesh->setVertexColor( i, col1 );
            }
            for( size_t i = 0; i < m_outMesh->vertSize(); ++i )
            {
                size_t id = m_refs[i];
                if( id > 0 )
                {
                    id = id - 1;
                    int x = coords->getScalar( id * 3 );
                    int y = coords->getScalar( id * 3 + 1 );
                    int z = coords->getScalar( id * 3 + 2 );

                    size_t loc = x + y * dimx + z * dimx * dimy;

                    WColor col( colors->getScalar( loc * 3 )/255.0,
                                colors->getScalar( loc * 3 + 1 )/255.0,
                                colors->getScalar( loc * 3 + 2 )/255.0,
                                1.0 );

                    m_outMesh->setVertexColor( i, col );
                }
            }
#endif
            m_meshOutput->updateData( m_outMesh );
        }

        if( m_propWriteTrigger->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
        {
            //writeAnnotation();
            writeLabels();
            m_propWriteTrigger->set( WPVBaseTypes::PV_TRIGGER_READY, false );
        }
    }
}

void WMPartition2Mesh::writeLabels()
{
    std::vector<std::vector<size_t> > labels;
    std::vector<osg::Vec4> colormap;

    // create label indexes
    {
        boost::unordered_map<std::string, int> id_hash;
        boost::unordered_map<std::string, osg::Vec4> color_hash;
        std::vector<std::string>keys;
        int id = 0;
        std::vector<size_t> vertexLabels( m_outMesh->vertSize(), 0 );


        for( size_t i = 0; i < m_outMesh->vertSize(); ++i )
        {
            osg::Vec4 color = m_outMesh->getVertColor( i );
            std::string key = string_utils::toString( static_cast<unsigned char>( color.r() * 255 ) ) +
                    string_utils::toString( static_cast<unsigned char>( color.g() * 255 ) ) +
                    string_utils::toString( static_cast<unsigned char>( color.b() * 255 ) );
            if( id_hash.find( key ) == id_hash.end() )
            {
                id_hash[key] = id;
                color_hash[key] = color;
                keys.push_back( key );
                ++id;
                std::vector<size_t> temp;
                labels.push_back( temp );
                colormap.push_back( color );
            }
            vertexLabels[i] = id_hash.at( key );
            labels[vertexLabels[i]].push_back( i );
        }
    }

    // ========== Write freesurfer labels ==========

    std::string currentPath( m_propLabelFolder->getAsString() );
    currentPath.erase( currentPath.begin() );
    currentPath.erase( currentPath.end()-1 );

    infoLog() << "Partition2Mesh: Writing out labels for " << labels.size() << " cluster partition in: " << currentPath;


    std::string tclFilePath( currentPath + "/get_labels.tcl" );

    std::ofstream tclFile( tclFilePath.c_str() );
    tclFile << "# " << labels.size() << " labels loading file. Use: >> tksurfer 'subject' 'hemisphere' 'surface' -tcl get_labels.tcl" << std::endl;

    std::string load_command( "labl_load " );
    std::string color_command( "labl_set_color " );

    for( size_t i = 0; i < labels.size(); ++i )
    {
        std::string labelnumstring = ( str( boost::format( "%04d" ) % i ) );
        std::string labelFileName( "pclust"+labelnumstring+".label" );

        std::string labelFilepath( currentPath + "/" + labelFileName );

        std::ofstream labelFile( labelFilepath.c_str() );
        labelFile << "#!ascii label " << labelFileName << " , out of a " << labels.size() << " labels partition" << std::endl << std::endl;
        labelFile << labels[i].size() << std::endl;

        for(size_t j = 0; j < labels[i].size(); ++j)
        {
            size_t vID( labels[i][j] );
            labelFile << vID << " ";
            labelFile << m_outMesh->getVertex( vID ).x()-( m_datasetSizeX*0.5 ) << " ";
            labelFile << m_outMesh->getVertex( vID ).y()-( m_datasetSizeY*0.5 ) << " ";
            labelFile << m_outMesh->getVertex( vID ).z()-( m_datasetSizeZ*0.5 ) << " 0.0" << std::endl;
        }

        tclFile << load_command << labelFileName << std::endl;
        tclFile << color_command << i << " ";
        if( m_monochrome->get( true ) )
        {
            tclFile << static_cast<unsigned int>( ( m_propNonActiveColor->get( true ).r() *255 ) ) << " ";
            tclFile << static_cast<unsigned int>( ( m_propNonActiveColor->get( true ).g() *255 ) ) << " ";
            tclFile << static_cast<unsigned int>( ( m_propNonActiveColor->get( true ).b() *255 ) ) << std::endl;
        }
        else
        {
            tclFile << static_cast<unsigned int>( ( colormap[i].r() *255 ) ) << " ";
            tclFile << static_cast<unsigned int>( ( colormap[i].g() *255 ) ) << " ";
            tclFile << static_cast<unsigned int>( ( colormap[i].b() *255 ) ) << std::endl;
        }
    }

    // ========== Write freesurfer tcl file ==========

    tclFile << "set gaLinkedVars(offset) 0.6" << std::endl;
    tclFile << "set gaLinkedVars(light0 ) 0.6" << std::endl;
    tclFile << "SendLinkedVarGroup scene" << std::endl;
    tclFile << "rotate_brain_y 90" << std::endl;
    tclFile << "rotate_brain_x -90" << std::endl;
    tclFile << "labl_select -1" << std::endl;
    tclFile << "redraw" << std::endl;
} // end writeLabels()

#if 0
void WMPartition2Mesh::writeAnnotation()
{
    // create label indexes
    boost::unordered_map<std::string, int> id_hash;
    boost::unordered_map<std::string, osg::Vec4> color_hash;
    std::vector<std::string>keys;
    int intVar( 0 );
    int id( 0 );

    int* vno = new int[ m_outMesh->vertSize() ];
    int* label = new int[ m_outMesh->vertSize() ];

    for( size_t i = 0; i < m_outMesh->vertSize(); ++i )
    {
        vno[i] = i;
    }
    for( size_t i = 0; i < m_outMesh->vertSize(); ++i )
    {
        osg::Vec4 color = m_outMesh->getVertColor( i );
        std::string key = string_utils::toString( ( unsigned char )( color.r() * 255 ) ) +
                string_utils::toString( ( unsigned char )( color.g() * 255 ) ) +
                string_utils::toString( ( unsigned char )( color.b() * 255 ) );
        if( id_hash.find( key ) == id_hash.end() )
        {
            id_hash[key] = id;
            color_hash[key] = color;
            keys.push_back( key );
            ++id;
        }
        label[i] = id_hash.at( key );
    }
    std::cout << "num labels: " << id_hash.size() << std::endl;

    int fileSize = 0;
    fileSize += m_outMesh->vertSize() * 2 * sizeof( intVar );
    fileSize += 6 * sizeof( intVar );
    fileSize += 10; // string filename
    fileSize += ( 6 * sizeof( intVar ) + 5 ) * id_hash.size();

    size_t counter = 0;
    char* buffer = new char[fileSize];

    int* intBuffer = new int[4];
    intBuffer[0] = m_outMesh->vertSize();
    switchByteOrderOfArray( intBuffer, 1 );
    char* charintbuffer = reinterpret_cast< char* >( intBuffer );
    for( size_t i = 0; i < sizeof( intVar) ; ++i )
    {
        buffer[counter] = charintbuffer[i];
        ++counter;
    }
    switchByteOrderOfArray( vno, m_outMesh->vertSize() );
    char* charvno = reinterpret_cast< char* >( vno );
    for( size_t i = 0; i < m_outMesh->vertSize() * sizeof( intVar ); ++i )
    {
        buffer[counter] = charvno[i];
        ++counter;
    }
    switchByteOrderOfArray( label, m_outMesh->vertSize() );
    char* charlabel = reinterpret_cast< char* >( label );
    for( size_t i = 0; i < m_outMesh->vertSize() * sizeof( intVar ); ++i )
    {
        buffer[counter] = charlabel[i];
        ++counter;
    }

    intBuffer[0] = 0; // Tag "TAG_OLD_COLORTABLE" indicating a color table comes next
    intBuffer[1] = -1; // >0 means "old format", <0 means "new format" (new shown below)
    intBuffer[2] = id_hash.size(); // Max structure index (could be more than actual entries, since some may be omitted)
    intBuffer[3] =  10; // Length of subsequent string

    switchByteOrderOfArray( intBuffer, 4 );
    charintbuffer = reinterpret_cast< char* >( intBuffer );
    for( size_t i = 0; i < 4 * sizeof( intVar ); ++i )
    {
        buffer[counter] = charintbuffer[i];
        ++counter;
    }
    for( size_t i = 0; i < 10; ++i )
    {
        buffer[counter] = 'a';
        ++counter;
    }
    intBuffer[0] = id_hash.size();
    switchByteOrderOfArray( intBuffer, 1 );
    charintbuffer = reinterpret_cast< char* >( intBuffer );
    for( size_t i = 0; i < sizeof( intVar ); ++i )
    {
        buffer[counter] = charintbuffer[i];
        ++counter;
    }
    for( size_t k = 0; k < keys.size(); ++k )
    {
        //std::cout << keys[k] << " " << id_hash.at( keys[k] ) << " " << color_hash.at( keys[k] ) << std::endl;
        intBuffer[0] = id_hash.at( keys[k] );
        std::string label = "label" + string_utils::toString( k );
        intBuffer[1] = label.length();
        switchByteOrderOfArray( intBuffer, 2 );
        charintbuffer = reinterpret_cast< char* >( intBuffer );
        for( size_t i = 0; i < 2 * sizeof( intVar ); ++i )
        {
            buffer[counter] = charintbuffer[i];
            ++counter;
        }
        for( size_t i = 0; i < label.length(); ++i )
        {
            buffer[counter] = label.at( i );
            ++counter;
        }
        osg::Vec4 col = color_hash.at( keys[k] );

        intBuffer[0] = static_cast<unsigned int>( col.r() * 255 );
        intBuffer[1] = static_cast<unsigned int>( col.g() * 255 );
        intBuffer[2] = static_cast<unsigned int>( col.b() * 255 );
        intBuffer[3] = 0;

        switchByteOrderOfArray( intBuffer, 4 );
        charintbuffer = reinterpret_cast< char* >( intBuffer );
        for( size_t i = 0; i < 4 * sizeof( intVar ); ++i )
        {
            buffer[counter] = charintbuffer[i];
            ++counter;
        }
        std::ofstream outfile(  m_propAnnotationFile->getAsString().c_str(), std::ofstream::binary );
        outfile.write( buffer, fileSize );
        outfile.close();
    }
}
#endif
