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

#include "core/common/WTransferFunction.h"
#include "core/dataHandler/datastructures/WValueSetHistogram.h"
#include "core/dataHandler/WDataSetScalar.h"
#include "core/dataHandler/WGrid.h"
#include "core/dataHandler/WGridRegular3D.h"
#include "core/dataHandler/WValueSet.h"
#include "core/dataHandler/WValueSetBase.h"
#include "core/kernel/WKernel.h"
#include "WMTransferFunction1D.xpm"
#include "WMTransferFunction1D.h"

// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMTransferFunction1D )

WMTransferFunction1D::WMTransferFunction1D():
    WModule()
{
}

WMTransferFunction1D::~WMTransferFunction1D()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMTransferFunction1D::factory() const
{
    return boost::shared_ptr< WModule >( new WMTransferFunction1D() );
}

const char** WMTransferFunction1D::getXPMIcon() const
{
    return WMTransferFunction1D_xpm;
}

const std::string WMTransferFunction1D::getName() const
{
    return "Transfer Function 1D";
}

const std::string WMTransferFunction1D::getDescription() const
{
    return "A module to modify a transfer function.";
}

void WMTransferFunction1D::connectors()
{
    // the data set we use for displaying the histogram
    m_input = WModuleInputData < WDataSetSingle >::createAndAdd( shared_from_this(),
            "histogram input", "The data set used to display a histogram." );

    // an output connector for the transfer function created
    m_output = WModuleOutputData < WDataSetSingle >::createAndAdd( shared_from_this(),
            "transferFunction1D", "The selected transfer function" );

    WModule::connectors();
}

void WMTransferFunction1D::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );
    WTransferFunction tf;
    tf.addAlpha( 0.0, 0.0 );
    tf.addColor( 0.0, WColor( 0.0, 0.0, 0.0, 1.0 ) );
    tf.addAlpha( 1.0, 0.5 );
    tf.addColor( 1.0, WColor( 1.0, 0.0, 0.0, 1.0 ) );
    m_transferFunction = m_properties->addProperty( "Transfer Function", "The transfer function editor.", tf, m_propCondition, false );

    m_opacityScale = m_properties->addProperty( "Opacity Scaling",
                                                "Factor used to scale opacity for easier interaction",
                                                1.0,
                                                m_propCondition );

    m_binSize = m_properties->addProperty( "Histogram Resolution", "Number of bins in histogram.", 64, m_propCondition );
    m_binSize->setMin( 2 );
    m_binSize->setMax( 512 );

    m_resolution = m_properties->addProperty( "Number of Samples",
            "Number of samples in the transfer function. "
            "Some modules connected to the output may have additional restrictions. Volume rendering, e.g., requires a power of two "
            "samples at the moment."
            , 128, m_propCondition );
    m_resolution->setMin( 4 );
    m_resolution->setMax( 1024 );
    WModule::properties();
}

void WMTransferFunction1D::requirements()
{
    //m_requirements.push_back( new WGERequirement() );
}

void WMTransferFunction1D::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );

    ready();

    while( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        if( m_shutdownFlag() )
          break;

        bool tfChanged = m_transferFunction->changed();
        WTransferFunction tf = m_transferFunction->get( true );
        debugLog() << "Current transfer function " << tf.numAlphas() << " alphas.";
        if( m_input->updated() || m_binSize->changed() )
        {
            boost::shared_ptr< WDataSetSingle > dataSet= m_input->getData();
            bool dataValid = ( dataSet != NULL );
            if( !dataValid )
            {
                // FIXME: invalidate histogram in GUI
                tf.removeHistogram();
            }
            else
            {
                int binsize = m_binSize->get( true );

                boost::shared_ptr< WValueSetBase > values = dataSet->getValueSet();
                WValueSetHistogram histogram( values, 0, 255, binsize );

                // because of the current semantics of WTransferFunction,
                // we have to create a copy of the data here.
                std::vector<double> vhistogram( histogram.size() );
                for( int i = 0; i < binsize; ++i )
                {
                    vhistogram[ i ] = histogram[ i ];
                }

                tf.setHistogram( vhistogram );
            }

            // either way, we changed the data and want to update the TF
            m_transferFunction->set( tf );
        }

        if( m_resolution->changed() || tfChanged )
        {
            // debugLog() << "resampling transfer function";
            unsigned int resolution = m_resolution->get( true );
            // debugLog() << "new resolution: " << resolution;
            boost::shared_ptr< std::vector<unsigned char> > data( new std::vector<unsigned char>( resolution * 4 ) );

            // FIXME: get transfer function and publish the function
            tf.setOpacityScale( m_opacityScale->get( true ) );
            tf.sample1DTransferFunction(  &( *data )[ 0 ], resolution, 0.0, 1.0 );

            boost::shared_ptr< WValueSetBase > newValueSet( new WValueSet<unsigned char>( 1, 4, data, W_DT_UNSIGNED_CHAR ) );

            WGridTransformOrtho transform;
            boost::shared_ptr< WGridRegular3D > newGrid( new WGridRegular3D( resolution, 1, 1, transform ) );
            boost::shared_ptr< WDataSetSingle > newData( new WDataSetSingle( newValueSet, newGrid ) );
            m_output->updateData( newData );
        }
    }
}
