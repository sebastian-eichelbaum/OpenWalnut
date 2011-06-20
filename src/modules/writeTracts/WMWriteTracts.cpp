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
    m_fileTypeSelectionsList->addItem( "VTK fib", "" );
    m_fileTypeSelectionsList->addItem( "json", "" );
    m_fileTypeSelectionsList->addItem( "json2", "" );
    m_fileTypeSelectionsList->addItem( "json triangles", "" );

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
