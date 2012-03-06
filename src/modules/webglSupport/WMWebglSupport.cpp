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

#include <boost/filesystem.hpp>

#include <osg/Image>
#include <osgDB/WriteFile>

#include "core/common/WStringUtils.h"
#include "core/common/WPathHelper.h"
#include "core/kernel/WKernel.h"
#include "WMWebglSupport.xpm"

#include "WMWebglSupport.h"

// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMWebglSupport )

WMWebglSupport::WMWebglSupport():
    WModule()
{
}

WMWebglSupport::~WMWebglSupport()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMWebglSupport::factory() const
{
    // See "src/modules/template/" for an extensively documented example.
    return boost::shared_ptr< WModule >( new WMWebglSupport() );
}

const char** WMWebglSupport::getXPMIcon() const
{
    return WMWebglSupport_xpm; // Please put a real icon here.
}
const std::string WMWebglSupport::getName() const
{
    return "WebGL Support";
}

const std::string WMWebglSupport::getDescription() const
{
    // Specify your module description here. Be detailed. This text is read by the user.
    // See "src/modules/template/" for an extensively documented example.
    return "Someone should add some documentation here. "
    "Probably the best person would be the modules's creator, i.e. \"ralph\"";
}

void WMWebglSupport::connectors()
{
    m_meshInput = boost::shared_ptr< WModuleInputData < WTriangleMesh > >(
        new WModuleInputData< WTriangleMesh >( shared_from_this(), "mesh", "The input mesh" )
        );
    addConnector( m_meshInput );

    m_meshOutput = boost::shared_ptr< WModuleOutputData< WTriangleMesh > >(
            new WModuleOutputData< WTriangleMesh >( shared_from_this(), "surface mesh", "The output mesh" ) );
    addConnector( m_meshOutput );

    m_datasetInputScalar = boost::shared_ptr< WModuleInputData < WDataSetScalar > >(
        new WModuleInputData< WDataSetScalar >( shared_from_this(), "values scalar", "Dataset to color the mesh with" )
        );

    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_datasetInputScalar );

    m_datasetInputVector = boost::shared_ptr< WModuleInputData < WDataSetVector > >(
        new WModuleInputData< WDataSetVector >( shared_from_this(), "values vector", "Dataset to color the mesh with" )
        );

    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_datasetInputVector );


    WModule::connectors();
}

void WMWebglSupport::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );
    m_propTriggerSaveGray = m_properties->addProperty( "Do Save gray",  "Press!", WPVBaseTypes::PV_TRIGGER_READY, m_propCondition );
    m_propTriggerSaveRGB = m_properties->addProperty( "Do Save RGB",  "Press!", WPVBaseTypes::PV_TRIGGER_READY, m_propCondition );
    m_fileName = m_properties->addProperty( "file name", "", WPathHelper::getAppPath() );

    m_propTriggerUpdateOutputGray = m_properties->addProperty( "Update output gray",  "Press!", WPVBaseTypes::PV_TRIGGER_READY, m_propCondition );
    m_propTriggerUpdateOutputRGB = m_properties->addProperty( "Update output rgb",  "Press!", WPVBaseTypes::PV_TRIGGER_READY, m_propCondition );

    WModule::properties();
}

void WMWebglSupport::requirements()
{
    // Put the code for your requirements here. See "src/modules/template/" for an extensively documented example.
}

void WMWebglSupport::moduleMain()
{
    m_moduleState.add( m_meshInput->getDataChangedCondition() );
    m_moduleState.add( m_datasetInputScalar->getDataChangedCondition() );
    m_moduleState.add( m_datasetInputVector->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );

    // signal ready state
    ready();

    // loop until the module container requests the module to quit
    while( !m_shutdownFlag() )
    {
        // this waits for m_moduleState to fire. By default, this is only the m_shutdownFlag condition.
        // NOTE: you can add your own conditions to m_moduleState using m_moduleState.add( ... )
        m_moduleState.wait();

        if( m_triMesh != m_meshInput->getData() )
        {
            m_triMesh = m_meshInput->getData();
            debugLog() << "new triangle mesh conected";
        }

        if( m_datasetScalar != m_datasetInputScalar->getData() )
        {
            m_datasetScalar = m_datasetInputScalar->getData();
            debugLog() << "new scalar dataset connected";
        }

        if( m_datasetVector != m_datasetInputVector->getData() )
        {
            m_datasetVector = m_datasetInputVector->getData();
            debugLog() << "new vector dataset connected";
        }

        if( m_propTriggerSaveGray->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
        {
            debugLog() << "Save pressed";
            saveSlicesGray();
            m_propTriggerSaveGray->set( WPVBaseTypes::PV_TRIGGER_READY, true );
        }
        if( m_propTriggerSaveRGB->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
        {
            debugLog() << "Save RGB pressed";
            saveSlicesRGB();
            m_propTriggerSaveRGB->set( WPVBaseTypes::PV_TRIGGER_READY, true );
        }
        if( m_propTriggerUpdateOutputGray->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
        {
            debugLog() << "Update pressed Gray";
            colorTriMeshGray();
            m_meshOutput->updateData( m_triMesh );
            m_propTriggerUpdateOutputGray->set( WPVBaseTypes::PV_TRIGGER_READY, true );
        }
        if( m_propTriggerUpdateOutputRGB->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
        {
            debugLog() << "Update pressed RGB";
            colorTriMeshRGB();
            m_meshOutput->updateData( m_triMesh );
            m_propTriggerUpdateOutputRGB->set( WPVBaseTypes::PV_TRIGGER_READY, true );
        }
    }
}

void WMWebglSupport::colorTriMeshGray()
{
    //boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( ( *m_datasetScalar ).getGrid() );
    //bool success;

    for( size_t i = 0; i < m_triMesh->vertSize(); ++i )
    {
        // get vertex position
        WPosition pos = m_triMesh->getVertex( i );

        // get color value for that vertex from texture
        //double value = m_datasetScalar->interpolate( pos, &success );
        double value = m_datasetScalar->getValueAt( pos.x(), pos.y(), pos.z() );
        //if( !success )
        //{
        //    debugLog() << "error at getting value from datatset";
        //}

        // write color into tri mesh
        float c = value / 255.0;
        osg::Vec4 color( c, c, c, 1.0 );
        m_triMesh->setVertexColor( i, color );
    }
}

void WMWebglSupport::colorTriMeshRGB()
{
    //boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( ( *m_datasetScalar ).getGrid() );
    bool success;

    for( size_t i = 0; i < m_triMesh->vertSize(); ++i )
    {
        // get vertex position
        WPosition pos = m_triMesh->getVertex( i );

        // get color value for that vertex from texture
        WVector3d value = m_datasetVector->interpolate( pos, &success );
        //double value = m_datasetScalar->getValueAt( pos.x(), pos.y(), pos.z() );
        if( !success)
        {
            debugLog() << "error at getting value from datatset";
        }

        // write color into tri mesh
        //float c = value / 255.0 ;
        value /= 255.0;
        osg::Vec4 color( value[0], value[1], value[2], 1.0 );
        m_triMesh->setVertexColor( i, color );
    }
}

void WMWebglSupport::saveSlicesGray()
{
    using namespace boost::filesystem; //NOLINT

    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( ( *m_datasetScalar ).getGrid() );
    debugLog() << grid->getNbCoordsX() << " " << grid->getNbCoordsY() << " " << grid->getNbCoordsZ();

    osg::ref_ptr< osg::Image > ima = new osg::Image;
    ima->allocateImage( 256, 256, 1, GL_LUMINANCE, GL_UNSIGNED_BYTE );
    unsigned char* data = ima->data();

    for( size_t y = 0; y < 200; ++y )
    {
        for( size_t i = 0; i < 256; ++i )
        {
            for( size_t j = 0; j < 256; ++j )
            {
                data[256 * i + j] = 0;
            }
        }

        for( size_t x = 0; x < 160; ++x )
        {
            for( size_t z = 0; z < 160; ++z )
            {
                uint8_t value = static_cast<uint8_t>( m_datasetScalar->getValueAt( x, y, z ) );
                data[ 256 * ( 48 + z ) + x + 48 ] = value;
            }
        }
        path fileName = m_fileName->get();
        fileName.remove_filename().string();
        std::string fn( std::string( "coronal_" ) +  string_utils::toString( y ) + std::string( ".bmp" ) );
        osgDB::writeImageFile( *ima, ( fileName.string() + fn ).c_str() );
    }

    osg::ref_ptr< osg::Image > ima1 = new osg::Image;
    ima1->allocateImage( 256, 256, 1, GL_LUMINANCE, GL_UNSIGNED_BYTE );
    data = ima1->data();

    for( size_t x = 0; x < 160; ++x )
    {
        for( size_t i = 0; i < 256; ++i )
        {
            for( size_t j = 0; j < 256; ++j )
            {
                data[256 * i + j] = 0;
            }
        }
        for( size_t y = 0; y < 200; ++y )
        {
            for( size_t z = 0; z < 160; ++z )
            {
                uint8_t value = static_cast<uint8_t>( m_datasetScalar->getValueAt( x, y, z ) );
                data[ 256 * ( 48 + z ) + y + 28 ] = value;
            }
        }
        path fileName = m_fileName->get();
        fileName.remove_filename().string();
        std::string fn( std::string( "sagittal_" ) +  string_utils::toString( x ) + std::string( ".bmp" ) );
        osgDB::writeImageFile( *ima1, ( fileName.string() + fn ).c_str() );
    }

    osg::ref_ptr< osg::Image > ima2 = new osg::Image;
    ima2->allocateImage( 256, 256, 1, GL_LUMINANCE, GL_UNSIGNED_BYTE );
    data = ima2->data();

    for( size_t z = 0; z < 160; ++z )
    {
        for( size_t i = 0; i < 256; ++i )
        {
            for( size_t j = 0; j < 256; ++j )
            {
                data[256 * i + j] = 0;
            }
        }
        for( size_t x = 0; x < 160; ++x )
        {
            for( size_t y = 0; y < 200; ++y )
            {
                uint8_t value = static_cast<uint8_t>( m_datasetScalar->getValueAt( x, y, z ) );
                data[ 256 * ( y + 28 ) + x + 48 ] = value;
            }
        }
        path fileName = m_fileName->get();
        fileName.remove_filename().string();
        std::string fn( std::string( "axial_" ) +  string_utils::toString( z ) + std::string( ".bmp" ) );
        osgDB::writeImageFile( *ima2, ( fileName.string() + fn ).c_str() );
    }
}

void WMWebglSupport::saveSlicesRGB()
{
    using namespace boost::filesystem; //NOLINT

//    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( ( *m_datasetScalar ).getGrid() );
//    debugLog() << grid->getNbCoordsX() << " " << grid->getNbCoordsY() << " " << grid->getNbCoordsZ();

    osg::ref_ptr< osg::Image > ima = new osg::Image;
    ima->allocateImage( 256, 256, 1, GL_RGB, GL_UNSIGNED_BYTE );
    unsigned char* data = ima->data();
    bool success = false;
    for( size_t y = 0; y < 200; ++y )
    {
        for( size_t i = 0; i < 256; ++i )
        {
            for( size_t j = 0; j < 256; ++j )
            {
                data[256 * i * 3 + j * 3     ] = 0;
                data[256 * i * 3 + j * 3 + 1 ] = 0;
                data[256 * i * 3 + j * 3 + 2 ] = 0;
            }
        }

        for( size_t x = 0; x < 160; ++x )
        {
            for( size_t z = 0; z < 160; ++z )
            {
                WPosition pos( x, y, z );
                WVector3d value = m_datasetVector->interpolate( pos, &success );
                data[ 256 * ( 48 + z ) * 3 + ( x + 48 ) * 3     ] = static_cast<uint8_t>( value[0] );
                data[ 256 * ( 48 + z ) * 3 + ( x + 48 ) * 3 + 1 ] = static_cast<uint8_t>( value[1] );
                data[ 256 * ( 48 + z ) * 3 + ( x + 48 ) * 3 + 2 ] = static_cast<uint8_t>( value[2] );
            }
        }
        path fileName = m_fileName->get();
        fileName.remove_filename().string();
        std::string fn( std::string( "coronal_" ) +  string_utils::toString( y ) + std::string( ".bmp" ) );
        osgDB::writeImageFile( *ima, ( fileName.string() + fn ).c_str() );
    }

    osg::ref_ptr< osg::Image > ima1 = new osg::Image;
    ima1->allocateImage( 256, 256, 1, GL_RGB, GL_UNSIGNED_BYTE );
    data = ima1->data();

    for( size_t x = 0; x < 160; ++x )
    {
        for( size_t i = 0; i < 256; ++i )
        {
            for( size_t j = 0; j < 256; ++j )
            {
                data[256 * i * 3 + j * 3     ] = 0;
                data[256 * i * 3 + j * 3 + 1 ] = 0;
                data[256 * i * 3 + j * 3 + 2 ] = 0;
            }
        }

        for( size_t y = 0; y < 200; ++y )
        {
            for( size_t z = 0; z < 160; ++z )
            {
                WPosition pos( x, y, z );
                WVector3d value = m_datasetVector->interpolate( pos, &success );
                data[ 256 * ( 48 + z ) * 3 + ( y + 28 ) * 3     ] = static_cast<uint8_t>( value[0] );
                data[ 256 * ( 48 + z ) * 3 + ( y + 28 ) * 3 + 1 ] = static_cast<uint8_t>( value[1] );
                data[ 256 * ( 48 + z ) * 3 + ( y + 28 ) * 3 + 2 ] = static_cast<uint8_t>( value[2] );
            }
        }
        path fileName = m_fileName->get();
        fileName.remove_filename().string();
        std::string fn( std::string( "sagittal_" ) + string_utils::toString( x ) + std::string( ".bmp" ) );
        osgDB::writeImageFile( *ima1, ( fileName.string() + fn ).c_str() );
    }

    osg::ref_ptr< osg::Image > ima2 = new osg::Image;
    ima2->allocateImage( 256, 256, 1, GL_RGB, GL_UNSIGNED_BYTE );
    data = ima2->data();

    for( size_t z = 0; z < 160; ++z )
    {
        for( size_t i = 0; i < 256; ++i )
        {
            for( size_t j = 0; j < 256; ++j )
            {
                data[256 * i * 3 + j * 3     ] = 0;
                data[256 * i * 3 + j * 3 + 1 ] = 0;
                data[256 * i * 3 + j * 3 + 2 ] = 0;
            }
        }

        for( size_t x = 0; x < 160; ++x )
        {
            for( size_t y = 0; y < 200; ++y )
            {
                WPosition pos( x, y, z );
                WVector3d value = m_datasetVector->interpolate( pos, &success );
                //256 * ( y + 28 ) + x + 48
                data[ 256 * ( y + 28 ) * 3 + ( x + 48 ) * 3     ] = static_cast<uint8_t>( value[0] );
                data[ 256 * ( y + 28 ) * 3 + ( x + 48 ) * 3 + 1 ] = static_cast<uint8_t>( value[1] );
                data[ 256 * ( y + 28 ) * 3 + ( x + 48 ) * 3 + 2 ] = static_cast<uint8_t>( value[2] );
            }
        }
        path fileName = m_fileName->get();
        fileName.remove_filename().string();
        std::string fn( std::string( "axial_" ) + string_utils::toString( z ) + std::string( ".bmp" ) );
        osgDB::writeImageFile( *ima2, ( fileName.string() + fn ).c_str() );
    }
}
