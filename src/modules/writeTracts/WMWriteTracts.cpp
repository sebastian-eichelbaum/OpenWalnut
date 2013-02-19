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

#include <fstream>
#include <string>
#include <vector>

#include "core/common/WPropertyHelper.h"
#include "core/dataHandler/io/WWriterFiberVTK.h"
#include "core/kernel/WKernel.h"
#include "WMWriteTracts.h"
#include "WMWriteTracts.xpm"

W_LOADABLE_MODULE( WMWriteTracts )

WMWriteTracts::WMWriteTracts():
    WModule()
{
}

WMWriteTracts::~WMWriteTracts()
{
}

boost::shared_ptr< WModule > WMWriteTracts::factory() const
{
    return boost::shared_ptr< WModule >( new WMWriteTracts() );
}

const char** WMWriteTracts::getXPMIcon() const
{
    return WMWriteTracts_xpm;
}

const std::string WMWriteTracts::getName() const
{
    return "Write Tracts";
}

const std::string WMWriteTracts::getDescription() const
{
    return "Writes tracts either from a cluster or from a WDataSetFibers to a file";
}

void WMWriteTracts::connectors()
{
    m_clusterIC = WModuleInputData< const WFiberCluster >::createAndAdd( shared_from_this(), "clusterInput", "A set of tracts behind the WFiberCluster" ); // NOLINT line length
    m_tractIC = WModuleInputData< const WDataSetFibers >::createAndAdd( shared_from_this(), "tractInput", "A dataset of tracts" );

    WModule::connectors();
}

void WMWriteTracts::properties()
{
    m_savePath = m_properties->addProperty( "Save Path", "Where to save the result", boost::filesystem::path( "/no/such/file" ) );
    WPropertyHelper::PC_NOTEMPTY::addTo( m_savePath );
    m_run      = m_properties->addProperty( "Save", "Start saving", WPVBaseTypes::PV_TRIGGER_READY );

    m_fileTypeSelectionsList = boost::shared_ptr< WItemSelection >( new WItemSelection() );
    m_fileTypeSelectionsList->addItem( "VTK fib", "Stores the fibers in the VTK line format." );
    m_fileTypeSelectionsList->addItem( "json", "" );
    m_fileTypeSelectionsList->addItem( "json2", "" );
    m_fileTypeSelectionsList->addItem( "json triangles", "" );
    m_fileTypeSelectionsList->addItem( "POVRay Cylinders", "Stores the fibers as cylinders in a POVRay SDL file." );

    m_fileTypeSelection = m_properties->addProperty( "File type",  "file type.", m_fileTypeSelectionsList->getSelectorFirst(),
        boost::bind( &WMWriteTracts::fileTypeChanged, this )
    );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_fileTypeSelection );

    m_povrayOptions = m_properties->addPropertyGroup( "POVRay Options", "Options for the POVRay Exporter." );
    m_povrayOptions->setHidden( true );
    m_povrayTubeDiameter = m_povrayOptions->addProperty( "Tube Diameter",
                                  "The tube diameter. Each fibers is represented as a tube with spheres as connections between them",
                                  0.25 );
    m_povrayTubeDiameter->setMin( 0.001 );
    m_povrayTubeDiameter->setMax( 2.0 );
    m_povrayRadiosity = m_povrayOptions->addProperty( "Enable Radiosity",
                                  "Enable POVRay's radiosity renderer. Creates more realistic lighting but is very slow.",
                                  false );
    m_povraySaveOnlyNth = m_povrayOptions->addProperty( "Save Every n'th", "Option allows thinning the data. This is useful in cases were fast"
                                                        " rendering is needed.", 1 );
    m_povraySaveOnlyNth->setMin( 1 );
    m_povraySaveOnlyNth->setMax( 1000 );

    WModule::properties();
}

void WMWriteTracts::moduleMain()
{
    m_moduleState.add( m_clusterIC->getDataChangedCondition() );
    m_moduleState.add( m_tractIC->getDataChangedCondition() );
    m_moduleState.add( m_run->getCondition() );

    ready();

    while( !m_shutdownFlag() )
    {
        debugLog() << "Waiting for data ...";
        m_moduleState.wait();

        if( !m_clusterIC->getData() && !m_tractIC->getData() )
        {
            continue;
        }

        if( m_run->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
        {
            switch( m_fileTypeSelection->get( true ).getItemIndexOfSelected( 0 ) )
            {
                case 0:
                    {
                        WWriterFiberVTK w( m_savePath->get(), true );
                        if( m_clusterIC->getData() )
                        {
                            w.writeFibs( m_clusterIC->getData()->getDataSetReference() );
                        }
                        else if( m_tractIC->getData() )
                        {
                            w.writeFibs( m_tractIC->getData() );
                        }
                    }
                    break;
                case 1:
                    saveJson();
                    break;
                case 2:
                    saveJson2();
                    break;
                case 3:
                    saveJsonTriangles();
                    break;
                case 4:
                        if( m_tractIC->getData() )
                        {
                            savePOVRay( m_tractIC->getData() );
                        }
                    break;
                default:
                    debugLog() << "this shouldn't be reached";
                    break;
            }
            m_run->set( WPVBaseTypes::PV_TRIGGER_READY, true );
        }
    }
}

bool WMWriteTracts::saveJson() const
{
    boost::shared_ptr< const WDataSetFibers > ds = m_tractIC->getData();

    if( !ds )
    {
        return false;
    }

    if( ds->getVertices()->size() == 0 )
    {
        WLogger::getLogger()->addLogMessage( "Will not write file that contains 0 vertices.", "Write Tracts", LL_ERROR );
        return false;
    }

    const char* c_file = m_savePath->get().string().c_str();
    std::ofstream dataFile( c_file, std::ios_base::binary );

    if( dataFile )
    {
        WLogger::getLogger()->addLogMessage( "opening file", "Write Tracts", LL_DEBUG );
    }
    else
    {
        WLogger::getLogger()->addLogMessage( "open file failed" + m_savePath->get().string() , "Write Tracts", LL_ERROR );
        return false;
    }

    dataFile.setf( std::ios_base::fixed );
    dataFile.precision( 3 );

    WLogger::getLogger()->addLogMessage( "start writing file", "Write Tracts", LL_DEBUG );

    dataFile << ( "{\n" );

    dataFile << ( "    \"vertices\" : [" );
    boost::shared_ptr<std::vector<float> > verts = ds->getVertices();
    float fValue;
    for( size_t i = 0; i < (verts->size() - 1 )/ 3; ++i )
    {
        fValue = verts->at( i * 3 );
        dataFile << fValue << ",";
        fValue = verts->at( i * 3 + 1 );
        dataFile << fValue << ",";
        fValue = verts->at( i * 3 + 2 );
        dataFile << fValue << ",";
    }
    fValue = verts->at( verts->size() - 3 );
    dataFile << fValue << ",";
    fValue = verts->at( verts->size() - 2 );
    dataFile << fValue << ",";
    fValue = verts->at( verts->size() - 1 );
    dataFile << fValue << "],\n";

    dataFile << ( "    \"normals\" : [" );
    boost::shared_ptr<std::vector<float> > tangents = ds->getTangents();
    for( size_t i = 0; i < tangents->size() - 1; ++i )
    {
        fValue = tangents->at( i );
        dataFile << fValue << ",";
    }
    fValue = tangents->at( tangents->size() - 1 );
    dataFile << fValue << "],\n";

    dataFile << ( "    \"colors\" : [" );
    boost::shared_ptr< std::vector< float > > colors = ds->getColorScheme( "Global Color" )->getColor();
    for( size_t i = 0; i < colors->size() - 3; i += 3 )
    {
        fValue = colors->at( i );
        dataFile << fValue << ",";
        fValue = colors->at( i + 1 );
        dataFile << fValue << ",";
        fValue = colors->at( i + 2 );
        dataFile << fValue << ",1.0,";
    }
    fValue = colors->at( colors->size() - 3 );
    dataFile << fValue << ",";
    fValue = colors->at( colors->size() - 2 );
    dataFile << fValue << ",";
    fValue = colors->at( colors->size() - 1 );
    dataFile << fValue << ",1.0],\n";

    int iValue;
    dataFile << ( "    \"indices\" : [" );
    boost::shared_ptr<std::vector<size_t> > lengths = ds->getLineLengths();
    for( size_t i = 0; i < lengths->size() - 1; ++i )
    {
        iValue = lengths->at( i );
        dataFile << iValue << ",";
    }
    iValue = lengths->at( lengths->size() - 1 );
    dataFile << iValue << "]";

    dataFile <<  "\n}";

    dataFile.close();
    WLogger::getLogger()->addLogMessage( "saving done", "Write Tracts", LL_DEBUG );
    return true;
}

bool WMWriteTracts::saveJson2() const
{
    boost::shared_ptr< const WDataSetFibers > ds = m_tractIC->getData();

    if( !ds )
    {
        return false;
    }

    if( ds->getVertices()->size() == 0 )
    {
        WLogger::getLogger()->addLogMessage( "Will not write file that contains 0 vertices.", "Write Tracts", LL_ERROR );
        return false;
    }

    const char* c_file = m_savePath->get().string().c_str();
    std::ofstream dataFile( c_file, std::ios_base::binary );

    if( dataFile )
    {
        WLogger::getLogger()->addLogMessage( "opening file", "Write Tracts", LL_DEBUG );
    }
    else
    {
        WLogger::getLogger()->addLogMessage( "open file failed" + m_savePath->get().string() , "Write Tracts", LL_ERROR );
        return false;
    }

    dataFile.setf( std::ios_base::fixed );
    dataFile.precision( 3 );

    //**************************************************************************************
    // create arrays;

    std::vector<float> nVertices;
    std::vector<float> nNormals;
    std::vector<float> nColors;
    std::vector<size_t> nIndices;

    boost::shared_ptr<std::vector<size_t> > starts = ds->getLineStartIndexes();
    boost::shared_ptr<std::vector<size_t> > lengths = ds->getLineLengths();


    boost::shared_ptr<std::vector<float> > verts = ds->getVertices();
    boost::shared_ptr<std::vector<float> > tangents = ds->getTangents();
    boost::shared_ptr< std::vector< float > > colors = ds->getColorScheme( "Global Color" )->getColor();

    for( size_t k = 0; k < lengths->size(); ++k )
    {
        size_t newLength = 0;
        for( size_t i = starts->at( k ); i < ( starts->at( k ) + lengths->at( k ) ); ++i )
        {
            if( i % 2 == 0 )
            {
                nVertices.push_back( verts->at( i * 3 ) );
                nVertices.push_back( verts->at( i * 3 + 1 ) );
                nVertices.push_back( verts->at( i * 3 + 2 ) );

                nNormals.push_back( - tangents->at( i * 3 ) );
                nNormals.push_back( - tangents->at( i * 3 + 1 ) );
                nNormals.push_back( tangents->at( i * 3 + 2 ) );

                nColors.push_back( colors->at( i * 3 ) );
                nColors.push_back( colors->at( i * 3 + 1 ) );
                nColors.push_back( colors->at( i * 3 + 2 ) );
                nColors.push_back( 1.0 );

                ++newLength;
            }
        }
        nIndices.push_back( newLength );
    }



    WLogger::getLogger()->addLogMessage( "start writing file", "Write Tracts", LL_DEBUG );

    dataFile << ( "{\n" );

    dataFile << ( "    \"vertices\" : [" );

    float fValue;
    for( size_t i = 0; i < nVertices.size() - 1 ; ++i )
    {
        fValue = nVertices[i];
        dataFile << fValue << ",";
    }
    fValue = verts->at( verts->size() - 1 );
    dataFile << fValue << "],\n";

    dataFile << ( "    \"normals\" : [" );

    for( size_t i = 0; i < nNormals.size() - 1; ++i )
    {
        fValue = nNormals[i];
        dataFile << fValue << ",";
    }
    fValue = nNormals[nNormals.size() - 1];
    dataFile << fValue << "],\n";

    dataFile << ( "    \"colors\" : [" );

    for( size_t i = 0; i < nColors.size()- 1; ++i )
    {
        fValue = nColors[i];
        dataFile << fValue << ",";
    }
    fValue = nColors[nColors.size() - 1];
    dataFile << fValue << ",1.0],\n";

    int iValue;
    dataFile << ( "    \"indices\" : [" );

    for( size_t i = 0; i < nIndices.size() - 1; ++i )
    {
        iValue = nIndices[i];
        dataFile << iValue << ",";
    }
    iValue = nIndices[nIndices.size() - 1];
    dataFile << iValue << "]";

    dataFile <<  "\n}";

    dataFile.close();
    WLogger::getLogger()->addLogMessage( "saving done", "Write Tracts", LL_DEBUG );
    return true;
}


bool WMWriteTracts::saveJsonTriangles() const
{
    boost::shared_ptr< const WDataSetFibers > ds = m_tractIC->getData();

    if( !ds )
    {
        return false;
    }

    if( ds->getVertices()->size() == 0 )
    {
        WLogger::getLogger()->addLogMessage( "Will not write file that contains 0 vertices.", "Write Tracts", LL_ERROR );
        return false;
    }

    const char* c_file = m_savePath->get().string().c_str();
    std::ofstream dataFile( c_file, std::ios_base::binary );

    if( dataFile )
    {
        WLogger::getLogger()->addLogMessage( "opening file", "Write Tracts", LL_DEBUG );
    }
    else
    {
        WLogger::getLogger()->addLogMessage( "open file failed" + m_savePath->get().string() , "Write Tracts", LL_ERROR );
        return false;
    }

    dataFile.precision( 7 );

    WLogger::getLogger()->addLogMessage( "start writing file", "Write Tracts", LL_DEBUG );

    dataFile << ( "{\n" );

    dataFile << ( "    \"vertices\" : [" );
    boost::shared_ptr<std::vector<float> > verts = ds->getVertices();
    float fValue0;
    float fValue1;
    float fValue2;
    for( size_t i = 0; i < verts->size() - 3; ++i )
    {
        fValue0 = verts->at( i );
        i += 1;
        fValue1 = verts->at( i );
        i += 1;
        fValue2 = verts->at( i );
        dataFile << fValue0 << "," << fValue1 << "," << fValue2 << ",";
        dataFile << fValue0 << "," << fValue1 << "," << fValue2 << ",";
    }
    fValue0 = verts->at( verts->size() - 3 );
    fValue1 = verts->at( verts->size() - 2 );
    fValue2 = verts->at( verts->size() - 1 );
    dataFile << fValue0 << "," << fValue1 << "," << fValue2 << ",";
    dataFile << fValue0 << "," << fValue1 << "," << fValue2 << "],\n";

    dataFile << ( "    \"indices\" : [" );
    boost::shared_ptr<std::vector<size_t> > starts = ds->getLineStartIndexes();
    boost::shared_ptr<std::vector<size_t> > lengths = ds->getLineLengths();

    int counter = 0;
    for( size_t i = 0; i < lengths->size(); ++i )
    {
        for( size_t k = 0; k < lengths->at( i ); ++k )
        {
            dataFile << counter << "," << counter + 1 << "," << counter + 2 << ",";
            dataFile << counter + 1 << "," << counter + 3 << "," << counter + 2 << ",";
        }
    }

    dataFile <<  "\n}";

    dataFile.close();
    WLogger::getLogger()->addLogMessage( "saving done", "Write Tracts", LL_DEBUG );
    return true;
}

bool WMWriteTracts::savePOVRay( boost::shared_ptr< const WDataSetFibers > fibers ) const
{
    // open file
    boost::filesystem::path meshFile( m_savePath->get() );
    std::string fnPath = meshFile.parent_path().string();
    std::string fnBase = meshFile.stem().string();
    std::string fnExt = meshFile.extension().string();

    // construct the filenames
    // the meshfile
    std::string fnMesh = fnBase + ".mesh" + fnExt;
    std::string fnScene = fnBase + ".scene" + fnExt;

    // absolute paths
    std::string fnMeshAbs = fnPath + "/" + fnMesh;
    std::string fnSceneAbs = fnPath + "/" + fnScene;

    debugLog() << "Opening " << fnMeshAbs << " for writing the mesh data.";
    std::ofstream dataFile( fnMeshAbs.c_str(), std::ios_base::binary );
    if( !dataFile )
    {
        errorLog() << "Opening " << fnMeshAbs << " failed.";
        return false;
    }

    // needed arrays for iterating the fibers
    WDataSetFibers::IndexArray  fibStart = fibers->getLineStartIndexes();
    WDataSetFibers::LengthArray fibLen   = fibers->getLineLengths();
    WDataSetFibers::VertexArray fibVerts = fibers->getVertices();
    WDataSetFibers::TangentArray fibTangents = fibers->getTangents();

    // get current color scheme - the mode is important as it defines the number of floats in the color array per vertex.
    WDataSetFibers::ColorScheme::ColorMode fibColorMode = fibers->getColorScheme()->getMode();
    debugLog() << "Color mode is " << fibColorMode << ".";
    WDataSetFibers::ColorArray  fibColors = fibers->getColorScheme()->getColor();

    // for each fiber:
    debugLog() << "Iterating over all fibers.";

    // find min and max
    double minX = wlimits::MAX_DOUBLE;
    double minY = wlimits::MAX_DOUBLE;
    double minZ = wlimits::MAX_DOUBLE;
    double maxX = wlimits::MIN_DOUBLE;
    double maxY = wlimits::MIN_DOUBLE;
    double maxZ = wlimits::MIN_DOUBLE;

    size_t currentStart = 0;
    size_t increment = m_povraySaveOnlyNth->get();

    boost::shared_ptr< WProgress > progress1 = boost::shared_ptr< WProgress >( new WProgress( "Converting fibers", fibStart->size() / increment ) );
    m_progress->addSubProgress( progress1 );
    for( size_t fidx = 0; fidx < fibStart->size(); fidx += increment )
    {
        ++*progress1;

        // the start vertex index
        size_t sidx = fibStart->at( fidx ) * 3;
        size_t csidx = fibStart->at( fidx ) * fibColorMode;

        // the length of the fiber
        size_t len = fibLen->at( fidx );

        // walk along the fiber
        WVector3d lastvert( fibVerts->at( sidx ),
                            fibVerts->at( sidx + 1 ),
                            fibVerts->at( sidx + 2 ) );
        for( size_t k = 1; k < len; ++k )
        {
            // grab vector and color
            WVector3d vert( fibVerts->at( ( 3 * k ) + sidx ),
                            fibVerts->at( ( 3 * k ) + sidx + 1 ),
                            fibVerts->at( ( 3 * k ) + sidx + 2 ) );
            WColor color( fibColors->at( ( fibColorMode * k ) + csidx + ( 0 % fibColorMode ) ),
                          fibColors->at( ( fibColorMode * k ) + csidx + ( 1 % fibColorMode ) ),
                          fibColors->at( ( fibColorMode * k ) + csidx + ( 2 % fibColorMode ) ),
                          ( fibColorMode == WDataSetFibers::ColorScheme::RGBA ) ?
                            fibColors->at( ( fibColorMode * k ) + csidx + ( 3 % fibColorMode ) ) : 1.0 );

            if( vert.x() > maxX )
                maxX = vert.x();
            if( vert.y() > maxY )
                maxY = vert.y();
            if( vert.z() > maxZ )
                maxZ = vert.z();
            if( vert.x() < minX )
                minX = vert.x();
            if( vert.y() < minY )
                minY = vert.y();
            if( vert.z() < minZ )
                minZ = vert.z();

            // write it in POVRay style
            dataFile << "cylinder" << std::endl <<
                        "{" << std::endl <<
                        " <" << lastvert.x() << "," << lastvert.y() << "," << lastvert.z() << ">," <<
                          "<" << vert.x() << "," << vert.y() << "," << vert.z() << ">,Diameter" << std::endl <<
                        " pigment{color rgb <" << color.x() << "," << color.y() << "," << color.z() << ">}" << std::endl <<
                        " transform MoveToCenter" << std::endl <<
                        "}" << std::endl;
            dataFile << "sphere {" << std::endl <<
                        " <" << vert.x() << "," << vert.y() << "," << vert.z() << ">,Diameter" << std::endl <<
                        " pigment{ color rgb <" << color.x() << "," << color.y() << "," << color.z() << ">}" << std::endl <<
                        " transform MoveToCenter" << std::endl <<
                        "}" << std::endl;

            lastvert = vert;
        }
        currentStart += len;
    }

    double sizeX = maxX - minX;
    double sizeY = maxY - minY;
    double sizeZ = maxZ - minZ;
    double mX = minX + ( sizeX / 2.0 );
    double mY = minY + ( sizeY / 2.0 );
    double mZ = minZ + ( sizeZ / 2.0 );

    // done writing mesh. Close
    infoLog() << "Done. Closing " << fnMesh << ".";
    dataFile.close();
    progress1->finish();

    debugLog() << "Opening " << fnSceneAbs << " for writing.";
    std::ofstream dataFileScene( fnSceneAbs.c_str(), std::ios_base::binary );
    if( !dataFileScene )
    {
        errorLog() << "Opening " << fnSceneAbs << " failed.";
        return false;
    }

    // write some head data
    dataFileScene << "#version 3.6;" << std::endl << std::endl;

    dataFileScene << "// run with  povray -w800 -h600 -Q0 fibsLarge.scene.pov " << std::endl <<
                     "// * this creates a fast preview of the scene with a resolution of 800x600." << std::endl <<
                     "// * the Q parameter defines the quality Q0 means plain colors. Q11 is best, including radiosity." << std::endl << std::endl;

    if( m_povrayRadiosity->get() )
    {
        dataFileScene << "global_settings {" << std::endl <<
        " ambient_light 0" << std::endl << std::endl <<
        " radiosity {" << std::endl <<
        "  pretrace_start 0.08" << std::endl <<
        "  pretrace_end   0.005" << std::endl <<
        "  count 350" << std::endl <<
        "  error_bound 0.15" << std::endl <<
        "  recursion_limit 2" << std::endl <<
        " }"  << std::endl <<
        "}" << std::endl << std::endl;
    }

    dataFileScene << "// Enable Phong lighting for all the geometry" << std::endl <<
    dataFileScene << "#default{" << std::endl <<
                   " finish{" << std::endl <<
                   "  ambient 0" << std::endl <<
                   "  phong 1" << std::endl <<
                   "  // reflection 0.9 " << std::endl <<
                   " }" << std::endl <<
                   "}" << std::endl << std::endl;

    // save camera and add a light
    double camX = 0;
    double camY = sizeY;
    double camZ = 0;

    dataFileScene << "#declare MoveToCenter = transform{ translate < " << -mX << ", " << -mY << ", " << -mZ << " > };" << std::endl;
    dataFileScene << "#declare CamPosition = < " << camX << ", " << camY << ", " << camZ << " >;" << std::endl << std::endl;
    dataFileScene << "// Tube diameter" << std::endl;
    dataFileScene << "#declare Diameter = " << m_povrayTubeDiameter->get() << ";" << std::endl << std::endl;

    // this camera should produce a direct front view. The user surely needs to modify the camera
    dataFileScene << "camera {" << std::endl <<
                "  orthographic angle 45" << std::endl <<
                "  location CamPosition" << std::endl <<
                "  right 1.33*x" << std::endl <<
                "  // use this with 1280x1024" << std::endl <<
                "  // right 1.25*x" << std::endl <<
                "  up y " << std::endl <<
                "  look_at  < 0, 0, 0 >" << std::endl <<
                "}" << std::endl << std::endl;
    // headlight
    dataFileScene << "light_source {" << std::endl <<
                     "  CamPosition" << std::endl <<
                     "  color rgb <1.0, 1.0, 1.0>" << std::endl <<
                     "}" << std::endl << std::endl;

    // do not forget the mesh
    dataFileScene << "#include \"" << fnMesh << "\"" << std::endl;

    // done. Close
    infoLog() << "Done.";
    dataFileScene.close();
    return true;
}

void WMWriteTracts::fileTypeChanged()
{
    if( m_fileTypeSelection->get().getItemIndexOfSelected( 0 ) == 4 )
    {
        m_povrayOptions->setHidden( false );
    }
    else
    {
        m_povrayOptions->setHidden( true );
    }
}

