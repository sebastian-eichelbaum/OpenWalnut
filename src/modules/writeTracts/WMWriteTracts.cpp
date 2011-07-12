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
    m_clusterIC = WModuleInputData< const WFiberCluster >::createAndAdd( shared_from_this(), "clusterInput", "A the tracts behind the WFiberCluster" ); // NOLINT line length
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

    m_fileTypeSelection = m_properties->addProperty( "File type",  "file type.", m_fileTypeSelectionsList->getSelectorFirst() );
       WPropertyHelper::PC_SELECTONLYONE::addTo( m_fileTypeSelection );


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
            switch ( m_fileTypeSelection->get( true ).getItemIndexOfSelected( 0 ) )
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

    const char* c_file = m_savePath->get().file_string().c_str();
    std::ofstream dataFile( c_file, std::ios_base::binary );

    if ( dataFile )
    {
        WLogger::getLogger()->addLogMessage( "opening file", "Write Tracts", LL_DEBUG );
    }
    else
    {
        WLogger::getLogger()->addLogMessage( "open file failed" + m_savePath->get().file_string() , "Write Tracts", LL_ERROR );
        return false;
    }

    dataFile.setf( std::ios_base::fixed );
    dataFile.precision( 3 );

    WLogger::getLogger()->addLogMessage( "start writing file", "Write Tracts", LL_DEBUG );

    dataFile << ( "{\n" );

    dataFile << ( "    \"vertices\" : [" );
    boost::shared_ptr<std::vector<float> > verts = ds->getVertices();
    float fValue;
    for ( size_t i = 0; i < (verts->size() - 1 )/ 3; ++i )
    {
        fValue = 160 - verts->at( i * 3 );
        dataFile << fValue << ",";
        fValue = 200 - verts->at( i * 3 + 1 );
        dataFile << fValue << ",";
        fValue = verts->at( i * 3 + 2 );
        dataFile << fValue << ",";
    }
    fValue = 160 - verts->at( verts->size() - 3 );
    dataFile << fValue << ",";
    fValue = 200 - verts->at( verts->size() - 2 );
    dataFile << fValue << ",";
    fValue = verts->at( verts->size() - 1 );
    dataFile << fValue << "],\n";

    dataFile << ( "    \"normals\" : [" );
    boost::shared_ptr<std::vector<float> > tangents = ds->getTangents();
    for ( size_t i = 0; i < tangents->size() - 1; ++i )
    {
        fValue = tangents->at( i );
        dataFile << fValue << ",";
    }
    fValue = tangents->at( tangents->size() - 1 );
    dataFile << fValue << "],\n";

    dataFile << ( "    \"colors\" : [" );
    boost::shared_ptr<std::vector<float> > colors = ds->getGlobalColors();
    for ( size_t i = 0; i < colors->size() - 3; i += 3 )
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
    for ( size_t i = 0; i < lengths->size() - 1; ++i )
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

    const char* c_file = m_savePath->get().file_string().c_str();
    std::ofstream dataFile( c_file, std::ios_base::binary );

    if ( dataFile )
    {
        WLogger::getLogger()->addLogMessage( "opening file", "Write Tracts", LL_DEBUG );
    }
    else
    {
        WLogger::getLogger()->addLogMessage( "open file failed" + m_savePath->get().file_string() , "Write Tracts", LL_ERROR );
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
    boost::shared_ptr<std::vector<float> > colors = ds->getGlobalColors();

    for ( size_t k = 0; k < lengths->size(); ++k )
    {
        size_t newLength = 0;
        for ( size_t i = starts->at( k ); i < ( starts->at( k ) + lengths->at( k ) ); ++i )
        {
            if ( i % 2 == 0 )
            {
                nVertices.push_back( 160 - verts->at( i * 3 ) );
                nVertices.push_back( 200 - verts->at( i * 3 + 1 ) );
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
    for ( size_t i = 0; i < nVertices.size() - 1 ; ++i )
    {
        fValue = nVertices[i];
        dataFile << fValue << ",";
    }
    fValue = verts->at( verts->size() - 1 );
    dataFile << fValue << "],\n";

    dataFile << ( "    \"normals\" : [" );

    for ( size_t i = 0; i < nNormals.size() - 1; ++i )
    {
        fValue = nNormals[i];
        dataFile << fValue << ",";
    }
    fValue = nNormals[nNormals.size() - 1];
    dataFile << fValue << "],\n";

    dataFile << ( "    \"colors\" : [" );

    for ( size_t i = 0; i < nColors.size()- 1; ++i )
    {
        fValue = nColors[i];
        dataFile << fValue << ",";
    }
    fValue = nColors[nColors.size() - 1];
    dataFile << fValue << ",1.0],\n";

    int iValue;
    dataFile << ( "    \"indices\" : [" );

    for ( size_t i = 0; i < nIndices.size() - 1; ++i )
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

    const char* c_file = m_savePath->get().file_string().c_str();
    std::ofstream dataFile( c_file, std::ios_base::binary );

    if ( dataFile )
    {
        WLogger::getLogger()->addLogMessage( "opening file", "Write Tracts", LL_DEBUG );
    }
    else
    {
        WLogger::getLogger()->addLogMessage( "open file failed" + m_savePath->get().file_string() , "Write Tracts", LL_ERROR );
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
    for ( size_t i = 0; i < verts->size() - 3; ++i )
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
    for ( size_t i = 0; i < lengths->size(); ++i )
    {
        for ( size_t k = 0; k < lengths->at( i ); ++k )
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
    const char* file = m_savePath->get().file_string().c_str();
    debugLog() << "Opening " << file << " for write.";
    std::ofstream dataFile( file, std::ios_base::binary );
    if ( !dataFile )
    {
        errorLog() << "Opening " << file << " failed.";
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

    boost::shared_ptr< WProgress > progress1 = boost::shared_ptr< WProgress >( new WProgress( "Converting fibers", fibStart->size() ) );
    m_progress->addSubProgress( progress1 );

    // write some head data
    dataFile << "#version 3.5;" << std::endl << std::endl;
    dataFile << "#include \"colors.inc\"" << std::endl;
    dataFile << "#include \"rad_def.inc\"" << std::endl << std::endl;
    dataFile << "global_settings {" << std::endl <<
    " ambient_light 0" << std::endl << std::endl <<
    " radiosity {" << std::endl <<
    "  pretrace_start 0.08" << std::endl <<
    "  pretrace_end   0.005" << std::endl <<
    "  count 350" << std::endl <<
    "  error_bound 0.15" << std::endl <<
    "  recursion_limit 2" << std::endl <<
    " }"  << std::endl <<
    "}" << std::endl << std::endl <<
    "#default{" << std::endl <<
    " finish{" << std::endl <<
    "  ambient 0" << std::endl <<
    "  phong 1" << std::endl <<
    //"  reflection 0.9yy " << std::endl <<
    " }" << std::endl <<
    "}" << std::endl << std::endl;

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
    for( size_t fidx = 0; fidx < fibStart->size() ; ++fidx )
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
                          "<" << vert.x() << "," << vert.y() << "," << vert.z() << ">,0.5" << std::endl <<
                        " pigment{color rgb <" << color.x() << "," << color.y() << "," << color.z() << ">}" << std::endl <<
                        "}" << std::endl;
            dataFile << "sphere {" << std::endl <<
                        " <" << vert.x() << "," << vert.y() << "," << vert.z() << ">,0.5" << std::endl <<
                        " pigment{ color rgb <" << color.x() << "," << color.y() << "," << color.z() << ">}" << std::endl <<
                        "}" << std::endl;

            lastvert = vert;
        }
        currentStart += len;
    }

    double mX = minX + ( ( maxX - minX ) / 2.0 );
    double mY = minY + ( ( maxY - minY ) / 2.0 );
    double mZ = minZ + ( ( maxZ - minZ ) / 2.0 );

    // save camera and add a light
    dataFile << "camera {" << std::endl <<
                "  location < " << mX << ", " << mY << ", -120.0 >" << std::endl <<
                "  look_at  < " << mX << ", " << mY << ", 0.0 >" << std::endl <<
                "}" << std::endl;
    dataFile << "light_source {" << std::endl <<
                "  < " << mX << ", " << mY << ", -10000.0 >" << std::endl <<
                "  color rgb <1.0, 1.0, 1.0>" << std::endl <<
                "}" << std::endl;

    // done. Close
    infoLog() << "Done. Closing " << file << ".";
    dataFile.close();
    progress1->finish();
    return true;
}
