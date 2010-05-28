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
#include <sstream>
#include <vector>

#include "../../kernel/WKernel.h"
#include "../../dataHandler/WDataHandler.h"

#include "HARDIExtractor.xpm"
#include "WMHARDIExtractor.h"

WMHARDIExtractor::WMHARDIExtractor():
    WModule()
{
}

WMHARDIExtractor::~WMHARDIExtractor()
{
}

boost::shared_ptr< WModule > WMHARDIExtractor::factory() const
{
    return boost::shared_ptr< WModule >( new WMHARDIExtractor() );
}

const std::string WMHARDIExtractor::getName() const
{
    return "HARDI Extractor";
}

const std::string WMHARDIExtractor::getDescription() const
{
    return "This module allows extracting of single images from a HARDI dataset.";
}

const char** WMHARDIExtractor::getXPMIcon() const
{
    return HARDIExtractor_xpm;
}

void WMHARDIExtractor::connectors()
{
    m_input = boost::shared_ptr< WModuleInputData < WDataSetRawHARDI  > >(
        new WModuleInputData< WDataSetRawHARDI >( shared_from_this(), "in", "The HARDI dataset." ) );
    addConnector( m_input );

    m_output = boost::shared_ptr< WModuleOutputData < WDataSetScalar  > >(
        new WModuleOutputData< WDataSetScalar >( shared_from_this(), "out", "The extracted image." ) );
    addConnector( m_output );

    WModule::connectors();
}

void WMHARDIExtractor::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    m_selectedImage = m_properties->addProperty( "Image", "The image to be extracted.", 0, m_propCondition );
    m_selectedImage->setMin( 0 );
    m_selectedImage->setMax( 1 );
}

void WMHARDIExtractor::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );

    ready();

    while( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        if( m_shutdownFlag() )
        {
            break;
        }

        boost::shared_ptr< WDataSetRawHARDI > newDataSet = m_input->getData();
        bool dataChanged = ( m_dataSet != newDataSet );
        bool dataValid   = ( newDataSet );

        if( dataChanged && dataValid )
        {
            m_dataSet = newDataSet;
            m_selectedImage->setMax( m_dataSet->getMeasures() - 1 );
            m_selectedImage->ensureValidity( 0 );
        }

        if( dataValid )
        {
            if( m_outData )
            {
                WDataHandler::deregisterDataSet( m_outData );
            }

            std::size_t i = static_cast< std::size_t >( m_selectedImage->get( true ) );
            m_outData = extract( i );
            m_outData->setFileName( makeImageName( i ) );
            m_output->updateData( m_outData );

            if( m_outData )
            {
                WDataHandler::registerDataSet( m_outData );
            }
        }
    }

    if( m_outData )
    {
        WDataHandler::deregisterDataSet( m_outData );
    }
}

boost::shared_ptr< WDataSetScalar > WMHARDIExtractor::extract( std::size_t i ) const
{
    WAssert( m_dataSet, "" );
    WAssert( m_dataSet->getValueSet(), "" );
    WAssert( m_dataSet->getGrid(), "" );

    if( i >= m_dataSet->getMeasures() )
    {
        return boost::shared_ptr< WDataSetScalar >();
    }

    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( m_dataSet->getGrid() );
    WAssert( grid, "" );

    switch( m_dataSet->getValueSet()->getDataType() )
    {
    case W_DT_FLOAT:
        {
            std::vector< float > values( m_dataSet->getGrid()->size() );
            boost::shared_ptr< WValueSet< float > > v = boost::shared_dynamic_cast< WValueSet< float > >( m_dataSet->getValueSet() );
            WAssert( v, "" );
            for( std::size_t k = 0; k < grid->size(); ++k )
            {
                values[ k ] =  v->rawData()[ m_dataSet->getMeasures() * k + i ];
            }

            boost::shared_ptr< WValueSet< float > > vs =
                    boost::shared_ptr< WValueSet< float > >( new WValueSet< float >( 0, 1, values, W_DT_FLOAT ) );

            return boost::shared_ptr< WDataSetScalar >( new WDataSetScalar( vs, grid ) );
        }
    case W_DT_DOUBLE:
        {
            std::vector< double > values( m_dataSet->getGrid()->size() );
            boost::shared_ptr< WValueSet< double > > v = boost::shared_dynamic_cast< WValueSet< double > >( m_dataSet->getValueSet() );
            WAssert( v, "" );
            for( std::size_t k = 0; k < grid->size(); ++k )
            {
                values[ k ] =  v->rawData()[ m_dataSet->getMeasures() * k + i ];
            }

            boost::shared_ptr< WValueSet< double > > vs =
                    boost::shared_ptr< WValueSet< double > >( new WValueSet< double >( 0, 1, values, W_DT_DOUBLE ) );

            return boost::shared_ptr< WDataSetScalar >( new WDataSetScalar( vs, grid ) );
        }
    case W_DT_UINT8:
        {
            std::vector< unsigned char > values( m_dataSet->getGrid()->size() );
            boost::shared_ptr< WValueSet< unsigned char > > v = boost::shared_dynamic_cast< WValueSet< unsigned char > >( m_dataSet->getValueSet() );
            WAssert( v, "" );
            for( std::size_t k = 0; k < grid->size(); ++k )
            {
                values[ k ] =  v->rawData()[ m_dataSet->getMeasures() * k + i ];
            }

            boost::shared_ptr< WValueSet< unsigned char > > vs =
                    boost::shared_ptr< WValueSet< unsigned char > >( new WValueSet< unsigned char >( 0, 1, values, W_DT_UINT8 ) );

            return boost::shared_ptr< WDataSetScalar >( new WDataSetScalar( vs, grid ) );
        }
    case W_DT_UINT16:
        {
            std::vector< uint16_t > values( m_dataSet->getGrid()->size() );
            boost::shared_ptr< WValueSet< uint16_t > > v = boost::shared_dynamic_cast< WValueSet< uint16_t > >( m_dataSet->getValueSet() );
            WAssert( v, "" );
            for( std::size_t k = 0; k < grid->size(); ++k )
            {
                values[ k ] =  v->rawData()[ m_dataSet->getMeasures() * k + i ];
            }

            boost::shared_ptr< WValueSet< uint16_t > > vs =
                    boost::shared_ptr< WValueSet< uint16_t > >( new WValueSet< uint16_t >( 0, 1, values, W_DT_UINT16 ) );

            return boost::shared_ptr< WDataSetScalar >( new WDataSetScalar( vs, grid ) );
        }
    case W_DT_UINT32:
        {
            std::vector< unsigned int > values( m_dataSet->getGrid()->size() );
            boost::shared_ptr< WValueSet< unsigned int > > v = boost::shared_dynamic_cast< WValueSet< unsigned int > >( m_dataSet->getValueSet() );
            WAssert( v, "" );
            for( std::size_t k = 0; k < grid->size(); ++k )
            {
                values[ k ] =  v->rawData()[ m_dataSet->getMeasures() * k + i ];
            }

            boost::shared_ptr< WValueSet< unsigned int > > vs =
                    boost::shared_ptr< WValueSet< unsigned int > >( new WValueSet< unsigned int >( 0, 1, values, W_DT_UINT32 ) );

            return boost::shared_ptr< WDataSetScalar >( new WDataSetScalar( vs, grid ) );
        }
    case W_DT_INT8:
        {
            std::vector< char > values( m_dataSet->getGrid()->size() );
            boost::shared_ptr< WValueSet< char > > v = boost::shared_dynamic_cast< WValueSet< char > >( m_dataSet->getValueSet() );
            WAssert( v, "" );
            for( std::size_t k = 0; k < grid->size(); ++k )
            {
                values[ k ] =  v->rawData()[ m_dataSet->getMeasures() * k + i ];
            }

            boost::shared_ptr< WValueSet< char > > vs =
                    boost::shared_ptr< WValueSet< char > >( new WValueSet< char >( 0, 1, values, W_DT_INT8 ) );

            return boost::shared_ptr< WDataSetScalar >( new WDataSetScalar( vs, grid ) );
        }
    case W_DT_INT16:
        {
            std::vector< int16_t > values( m_dataSet->getGrid()->size() );
            boost::shared_ptr< WValueSet< int16_t > > v = boost::shared_dynamic_cast< WValueSet< int16_t > >( m_dataSet->getValueSet() );
            WAssert( v, "" );
            for( std::size_t k = 0; k < grid->size(); ++k )
            {
                values[ k ] =  v->rawData()[ m_dataSet->getMeasures() * k + i ];
            }

            boost::shared_ptr< WValueSet< int16_t > > vs =
                    boost::shared_ptr< WValueSet< int16_t > >( new WValueSet< int16_t >( 0, 1, values, W_DT_INT16 ) );

            return boost::shared_ptr< WDataSetScalar >( new WDataSetScalar( vs, grid ) );
        }
    case W_DT_SIGNED_INT:
        {
            std::vector< int > values( m_dataSet->getGrid()->size() );
            boost::shared_ptr< WValueSet< int > > v = boost::shared_dynamic_cast< WValueSet< int > >( m_dataSet->getValueSet() );
            WAssert( v, "" );
            for( std::size_t k = 0; k < grid->size(); ++k )
            {
                values[ k ] = v->rawData()[ m_dataSet->getMeasures() * k + i ];
            }

            boost::shared_ptr< WValueSet< int > > vs =
                    boost::shared_ptr< WValueSet< int > >( new WValueSet< int >( 0, 1, values, W_DT_SIGNED_INT ) );

            return boost::shared_ptr< WDataSetScalar >( new WDataSetScalar( vs, grid ) );
        }
    default:
        warnLog() << "Encountered unsupported data format in HARDIExtractor!";
        return boost::shared_ptr< WDataSetScalar >();
    }
}

const std::string WMHARDIExtractor::makeImageName( std::size_t i )
{
    std::stringstream s;
    WAssert( m_dataSet, "" );
    s << m_dataSet->getFileName() << " (" << ( i + 1 ) << " of " << m_dataSet->getMeasures() << ")";
    return s.str();
}
