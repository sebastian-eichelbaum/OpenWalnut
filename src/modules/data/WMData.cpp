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

#include <string>
#include <vector>


#include "../../dataHandler/exceptions/WDHException.h"
#include "../../dataHandler/io/WLoaderNIfTI.h"
#include "../../dataHandler/io/WLoaderBiosig.h"
#include "../../dataHandler/io/WLoaderEEGASCII.h"
#include "../../dataHandler/io/WLoaderFibers.h"
#include "../../dataHandler/io/WIOTools.hpp"

#include "WMData.h"

WMData::WMData():
    WModule()
{
    // initialize members
}

WMData::~WMData()
{
    // cleanup
}

boost::shared_ptr< WModule > WMData::factory() const
{
    return boost::shared_ptr< WModule >( new WMData() );
}

const std::string WMData::getName() const
{
    return "Data Module";
}

const std::string WMData::getDescription() const
{
    return "This module encapsulates data.";
}

boost::shared_ptr< WDataSet > WMData::getDataSet()
{
    return m_dataSet;
}

void WMData::connectors()
{
    // initialize connectors
    m_output= boost::shared_ptr< WModuleOutputData< WDataSet > >( new WModuleOutputData< WDataSet >( shared_from_this(),
                "out1", "A loaded dataset." )
            );

    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_output );

    // call WModules initialization
    WModule::connectors();
}

void WMData::properties()
{
    // properties

    // filename of file to load and handle
    m_properties->addString( "filename" );
    m_properties->hideProperty( "filename" );
    m_properties->addString( "name" );
    m_properties->addBool( "active", true );
    m_properties->hideProperty( "active" );
    m_properties->addBool( "interpolation", true );
    m_properties->addInt( "threshold", 0 );
    m_properties->addInt( "alpha", 100 );
    m_properties->setMax( "alpha", 100 );
}

void WMData::notifyConnectionEstablished( boost::shared_ptr<WModuleConnector> here,
                                           boost::shared_ptr<WModuleConnector> there )
{
    WLogger::getLogger()->addLogMessage( "Connected \"" + here->getCanonicalName() + "\" to \"" + there->getCanonicalName() + "\".",
            getName(), LL_DEBUG );
}

void WMData::notifyConnectionClosed( boost::shared_ptr<WModuleConnector> here, boost::shared_ptr<WModuleConnector> there )
{
    WLogger::getLogger()->addLogMessage( "Disconnected \"" + here->getCanonicalName() + "\" from \"" + there->getCanonicalName() + "\".",
            getName(), LL_DEBUG );
}

void WMData::notifyDataChange( boost::shared_ptr<WModuleConnector> /*input*/, boost::shared_ptr<WModuleConnector> /*output*/ )
{
    // not used here, since data modules can not receive an input.
}

void WMData::notifyStop()
{
    // not used here. It gets called whenever the module should stop running.
}

void WMData::moduleMain()
{
    using wiotools::getSuffix;
    std::string fileName = m_properties->getValue< std::string >( "filename" );

    WLogger::getLogger()->addLogMessage( "Loading data from \"" + fileName + "\".", getName(), LL_DEBUG );
    m_properties->setValue( "name", fileName );

    // load it now
    std::string suffix = getSuffix( fileName );

    if( suffix == ".nii" || suffix == ".gz" )
    {
        if( suffix == ".gz" )  // it may be a NIfTI file too
        {
            boost::filesystem::path p( fileName );
            p.replace_extension( "" );
            suffix = getSuffix( p.string() );
            assert( suffix == ".nii" && "currently only nii files may be gzipped" );
        }
        WLoaderNIfTI niiLoader( fileName, WKernel::getRunningKernel()->getDataHandler() );
        m_dataSet = niiLoader.load();
    }
    else if( suffix == ".edf" )
    {
        WLoaderBiosig biosigLoader( fileName, WKernel::getRunningKernel()->getDataHandler() );
        m_dataSet = biosigLoader.load();
    }
    else if( suffix == ".asc" )
    {
        WLoaderEEGASCII eegAsciiLoader( fileName, WKernel::getRunningKernel()->getDataHandler() );
        m_dataSet = eegAsciiLoader.load();
    }
    else if( suffix == ".vtk" )
    {
        // This is a dummy implementation.
        // You need to provide a real implementation here if you want to load vtk.
        std::cout << "VTK not implemented yet" << std::endl;
        assert( 0 );
    }
    else if( suffix == ".fib" )
    {
        WLoaderFibers fibLoader( fileName, WKernel::getRunningKernel()->getDataHandler() );
        m_dataSet = fibLoader.load();
    }
    else
    {
        throw WDHException( "Unknown file type: '" + suffix + "'" );
    }

    // notify
    m_output->updateData( m_dataSet );
    ready();

    // go to idle mode
    waitForStop();  // WThreadedRunner offers this for us. It uses boost::condition to avoid wasting CPU cycles with while loops.
}

osg::ref_ptr<osg::Texture3D> WMData::getTexture3D()
{
    if ( !m_texture3D )
    {
        boost::shared_ptr< WDataSetSingle > ds = boost::shared_dynamic_cast< WDataSetSingle >( m_dataSet );
        boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( ds->getGrid() );

        if ( ds->getValueSet()->getDataType() == 2 )
        {
            boost::shared_ptr< WValueSet< unsigned char > > vs = boost::shared_dynamic_cast< WValueSet< unsigned char > >( ds->getValueSet() );
            unsigned char* source = const_cast< unsigned char* > ( vs->rawData() );
            m_texture3D = createTexture3D( source, grid, ds->getValueSet()->dimension() );
        }

        else if ( ds->getValueSet()->getDataType() == 4 )
        {
            boost::shared_ptr< WValueSet< int16_t > > vs = boost::shared_dynamic_cast< WValueSet< int16_t > >( ds->getValueSet() );
            int16_t* source = const_cast< int16_t* > ( vs->rawData() );
            m_texture3D = createTexture3D( source, grid, ds->getValueSet()->dimension() );
        }

        else if ( ds->getValueSet()->getDataType() == 16 )
        {
            boost::shared_ptr< WValueSet< float > > vs = boost::shared_dynamic_cast< WValueSet< float > >( ds->getValueSet() );
            float* source = const_cast< float* > ( vs->rawData() );
            m_texture3D = createTexture3D( source, grid, ds->getValueSet()->dimension() );
        }
    }
    return m_texture3D;
}

osg::ref_ptr<osg::Texture3D> WMData::createTexture3D( unsigned char* source, boost::shared_ptr<WGridRegular3D> grid, int components )
{
    if ( components == 1 )
    {
        osg::ref_ptr< osg::Image > ima = new osg::Image;
        ima->allocateImage( grid->getNbCoordsX(), grid->getNbCoordsY(), grid->getNbCoordsZ(), GL_LUMINANCE, GL_UNSIGNED_BYTE );

        unsigned char* data = ima->data();

        for ( unsigned int i = 0; i < grid->getNbCoordsX() * grid->getNbCoordsY() * grid->getNbCoordsZ(); ++i )
        {
            data[i] = source[i];
        }
        osg::ref_ptr<osg::Texture3D> texture3D = osg::ref_ptr<osg::Texture3D>( new osg::Texture3D );
        texture3D->setFilter( osg::Texture3D::MIN_FILTER, osg::Texture3D::LINEAR );
        texture3D->setFilter( osg::Texture3D::MAG_FILTER, osg::Texture3D::LINEAR );
        texture3D->setWrap( osg::Texture3D::WRAP_R, osg::Texture3D::REPEAT );
        texture3D->setImage( ima );
        texture3D->setResizeNonPowerOfTwoHint( false );

        return texture3D;
    }
    else if ( components == 3 )
    {
        osg::ref_ptr< osg::Image > ima = new osg::Image;
        ima->allocateImage( grid->getNbCoordsX(), grid->getNbCoordsY(), grid->getNbCoordsZ(), GL_RGB, GL_UNSIGNED_BYTE );

        unsigned char* data = ima->data();

        for ( unsigned int i = 0; i < grid->getNbCoordsX() * grid->getNbCoordsY() * grid->getNbCoordsZ() * 3; ++i )
        {
            data[i] = source[i];
        }
        osg::ref_ptr<osg::Texture3D> texture3D = osg::ref_ptr<osg::Texture3D>( new osg::Texture3D );
        texture3D->setFilter( osg::Texture3D::MIN_FILTER, osg::Texture3D::LINEAR );
        texture3D->setFilter( osg::Texture3D::MAG_FILTER, osg::Texture3D::LINEAR );
        texture3D->setWrap( osg::Texture3D::WRAP_R, osg::Texture3D::REPEAT );
        texture3D->setImage( ima );
        texture3D->setResizeNonPowerOfTwoHint( false );

        return texture3D;
    }

    return 0;
}

osg::ref_ptr<osg::Texture3D> WMData::createTexture3D( int16_t* source, boost::shared_ptr<WGridRegular3D> grid, int components )
{
    if ( components == 1)
    {
        int nSize = grid->getNbCoordsX() * grid->getNbCoordsY() * grid->getNbCoordsZ();

        std::vector<int16_t> tempSource( nSize );

        for ( int i = 0; i < nSize; ++i )
        {
            tempSource[i] = static_cast<int16_t>( source[i] );
        }

        int max = 0;
        std::vector< int > histo( 65536, 0 );
        for ( int i = 0; i < nSize; ++i )
        {
            if ( max < tempSource[i])
            {
                max = tempSource[i];
            }
            ++histo[tempSource[i]];
        }
        int fivepercent = static_cast<int>( nSize * 0.001 );

        int newMax = 65535;
        int adder = 0;
        for ( int i = 65535; i > 0; --i )
        {
            adder += histo[i];
            newMax = i;
            if ( adder > fivepercent )
                break;
        }
        for ( int i = 0; i < nSize; ++i )
        {
            if ( tempSource[i] > newMax )
            {
                tempSource[i] = newMax;
            }
        }

        int mult = 65535 / newMax;
        std::cout << "mult:" << mult << std::endl;
        std::cout << "newMax:" << newMax << std::endl;
        for ( int i = 0; i < nSize; ++i )
        {
            tempSource[i] *= mult;
        }

        osg::ref_ptr< osg::Image > ima = new osg::Image;
        ima->allocateImage( grid->getNbCoordsX(), grid->getNbCoordsY(), grid->getNbCoordsZ(), GL_LUMINANCE, GL_UNSIGNED_SHORT );

        unsigned char* data = ima->data();

        unsigned char* charSource = ( unsigned char* )&tempSource[0];

        for ( unsigned int i = 0; i < grid->getNbCoordsX() * grid->getNbCoordsY() * grid->getNbCoordsZ() * 2 ; ++i )
        {
            data[i] = charSource[i];
        }
        osg::ref_ptr<osg::Texture3D> texture3D = osg::ref_ptr<osg::Texture3D>( new osg::Texture3D );
        texture3D->setFilter( osg::Texture3D::MIN_FILTER, osg::Texture3D::LINEAR );
        texture3D->setFilter( osg::Texture3D::MAG_FILTER, osg::Texture3D::LINEAR );
        texture3D->setWrap( osg::Texture3D::WRAP_R, osg::Texture3D::REPEAT );
        texture3D->setImage( ima );
        texture3D->setResizeNonPowerOfTwoHint( false );

        return texture3D;
    }
    return 0;
}

osg::ref_ptr<osg::Texture3D> WMData::createTexture3D( float* source, boost::shared_ptr<WGridRegular3D>  grid, int components )
{
    if ( components == 1)
    {
        osg::ref_ptr< osg::Image > ima = new osg::Image;
        ima->allocateImage( grid->getNbCoordsX(), grid->getNbCoordsY(), grid->getNbCoordsZ(), GL_LUMINANCE, GL_FLOAT );

        unsigned char* data = ima->data();

        unsigned char* charSource = ( unsigned char* )source;

        for ( unsigned int i = 0; i < grid->getNbCoordsX() * grid->getNbCoordsY() * grid->getNbCoordsZ() * 4 ; ++i )
        {
            data[i] = charSource[i];
        }
        osg::ref_ptr<osg::Texture3D> texture3D = osg::ref_ptr<osg::Texture3D>( new osg::Texture3D );
        texture3D->setFilter( osg::Texture3D::MIN_FILTER, osg::Texture3D::LINEAR );
        texture3D->setFilter( osg::Texture3D::MAG_FILTER, osg::Texture3D::LINEAR );
        texture3D->setWrap( osg::Texture3D::WRAP_R, osg::Texture3D::REPEAT );
        texture3D->setImage( ima );
        texture3D->setResizeNonPowerOfTwoHint( false );

        return texture3D;
    }
    return 0;
}
