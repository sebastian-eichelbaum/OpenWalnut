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

#include <vector>
#include <fstream>
#include <string>

#include "core/kernel/WKernel.h"
#include "core/common/WPropertyHelper.h"
#include "core/dataHandler/WDataHandler.h"

#include "WMHistogramEqualization.h"
#include "WMHistogramEqualization.xpm"

// This line is needed by the module loader to actually find your module. You need to add this to your module too. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMHistogramEqualization )

WMHistogramEqualization::WMHistogramEqualization():
    WModule()
{
}

WMHistogramEqualization::~WMHistogramEqualization()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMHistogramEqualization::factory() const
{
    return boost::shared_ptr< WModule >( new WMHistogramEqualization() );
}

const char** WMHistogramEqualization::getXPMIcon() const
{
    return WMHistogramEqualization_xpm;
}

const std::string WMHistogramEqualization::getName() const
{
    return "Histogram Equalization";
}

const std::string WMHistogramEqualization::getDescription() const
{
    return "This module takes an arbitrary data set and equalizes its histogram. This increases contrast in several regions of the data.";
}

void WMHistogramEqualization::connectors()
{
    // the dataset to process. Only accept scalar data.
    m_input = boost::shared_ptr< WModuleInputData < WDataSetScalar  > >(
        new WModuleInputData< WDataSetScalar >( shared_from_this(),
                                                               "in", "The dataset whose histogram gets equalized." )
        );
    addConnector( m_input );

    // the output containing the equalized data.
    m_output = boost::shared_ptr< WModuleOutputData < WDataSetScalar  > >(
        new WModuleOutputData< WDataSetScalar >( shared_from_this(),
                                                               "out", "The dataset which has a linear cumulative histogram." )
        );
    addConnector( m_output );

    // call WModule's initialization
    WModule::connectors();
}

void WMHistogramEqualization::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    // clamping related stuff
    m_clamping = m_properties->addPropertyGroup( "Clamping",  "Clamping values in the dataset." );

    m_clamp = m_clamping->addProperty( "Clamp values?", "Values below the specified threshold are clamped to min and max respectively.",
                                        true, m_propCondition );

    m_histogramResolution = m_clamping->addProperty( "Histogram resolution", "How many buckets should be used for the initial data histogram?",
                                                       10000, m_propCondition );
    m_histogramResolution->setMin( 10 );
    m_histogramResolution->setMax( 1000000 );

    m_clampPerc = m_clamping->addProperty( "Min-Max clamping in %", "Percent that are clamped from the beginning and the end of the histogram.",
                                             1.0, m_propCondition );
    m_clampPerc->setMin( 0.0 );
    m_clampPerc->setMax( 100.0 );

    // equalizing related props
    m_equalizing = m_properties->addPropertyGroup( "Equalizing",  "Equalizing values in the dataset." );

    m_equalize  = m_equalizing->addProperty( "Equalize histogram", "If true, the dataset's cumulative histogram gets linearized.",
                                             true, m_propCondition );

    m_cdfResolution = m_equalizing->addProperty( "CDF histogram resolution",
                                                 "How many buckets should be used for the data histogram used for equalizing?",
                                                 10000, m_propCondition );
    m_cdfResolution->setMin( 10 );
    m_cdfResolution->setMax( 1000000 );

    // call WModule's initialization
    WModule::properties();
}

void WMHistogramEqualization::moduleMain()
{
    // get notified about data changes
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );
    // Remember the condition provided to some properties in properties()? The condition can now be used with this condition set.
    m_moduleState.add( m_propCondition );

    ready();

    // main loop
    while( !m_shutdownFlag() )
    {
        debugLog() << "Waiting ...";
        m_moduleState.wait();

        // woke up since the module is requested to finish?
        if( m_shutdownFlag() )
        {
            break;
        }

        // To query whether an input was updated, simply ask the input:
        bool dataUpdated = m_input->updated();

        // Remember the above criteria. We now need to check if the data is valid. After a connect-update, it might be NULL.
        boost::shared_ptr< WDataSetScalar > dataSet = m_input->getData();
        if( !dataSet )
        {
            debugLog() << "Resetting output.";
            m_output->reset();
            continue;
        }
        boost::shared_ptr< WValueSetBase > valueSet = dataSet->getValueSet();
        dataUpdated = dataUpdated && dataSet;

        // prepare progress indicators
        boost::shared_ptr< WProgress > progress( new WProgress( "Processing", 4 ) );
        m_progress->addSubProgress( progress );

        // The data is valid and we received an update. The data is not NULL but may be the same as in previous loops.
        size_t histRes = m_histogramResolution->get( true );
        size_t cdfHistRes = m_cdfResolution->get( true );
        debugLog() << "Calculating histogram with resolution " << histRes;

        // Grab the histogram whose modus (interval with most of the action) is used as interval for histogram equalization
        boost::shared_ptr< const WValueSetHistogram > hist = dataSet->getHistogram( histRes );
        ++*progress;

        // find interval borders and remove first and last p%
        double lower = hist->getMinimum();
        double upper = hist->getMaximum();

        double perc = m_clampPerc->get( true );

        // should the histogram be clamped before processing?
        ++*progress;
        if( m_clamp->get( true ) )
        {
            debugLog() << "Clamping histogram";

            size_t accumL = 0;  // accumulation of values from below
            size_t accumU = 0;  // accumulation of values from upper side
            size_t accumMax = static_cast< size_t >( static_cast< double >( hist->getTotalElementCount() ) * perc / 100.0 );
            bool foundL = false;
            bool foundU = false;
            size_t curI = 0;
            size_t maxI = hist->size() - 1; // the largest index in hist

            // search the histogram until the bucket with needed accumulative value is found
            while( curI <= maxI )
            {
                accumL += ( *hist )[ curI ];
                accumU += ( *hist )[ maxI - curI ];

                // lower border found?
                if( !foundL && ( accumL >= accumMax ) )
                {
                    foundL = true;
                    lower = hist->getIntervalForIndex( curI ).first;
                }
                if( !foundU && ( accumU >= accumMax ) )
                {
                    foundU = true;
                    upper = hist->getIntervalForIndex( maxI - curI ).second;
                }

                curI++;
            }

            debugLog() << "Clamped " << perc << "% of [" << hist->getMinimum() << ", " << hist->getMaximum() << "]" <<
                          " resulting in new interval [" << lower << ", " << upper <<").";

            // with this new interval, extract a new histogram and use it for equalization
            hist = boost::shared_ptr< const WValueSetHistogram >( new WValueSetHistogram( valueSet, lower, upper, cdfHistRes ) );
        }

        // the new data
        std::vector< double > newData;
        newData.resize( hist->getTotalElementCount(), 0 );

        // these values are needed to rescale the data to the original interval
        double valueMin = hist->getMinimum();
        double valueScaler = hist->getMaximum() - valueMin;

        // equalize?
        ++*progress;
        if( m_equalize->get( true ) )
        {
            // calculate the cumulative distribution function
            debugLog() << "Calculating cumulative distribution function";
            std::vector< double > cdf;
            cdf.resize( hist->size() );

            // go through each CDF item and fill it, which is the sum of all previous items in hist
            size_t accum = 0;
            double cdfMin = ( *hist )[ 0 ];
            double cdfScaler = static_cast< double >( valueSet->rawSize() ) - cdfMin;
            for( size_t i = 0; i < hist->size(); ++i )
            {
                // the CDF at i is the summed up histogram from 0 to i
                // we additionally require the histogram to be normalized so divide by the total count
                accum += ( *hist )[ i ];
                cdf[ i ] = accum - cdfMin;
            }

            // finally, build the new dataset
            debugLog() << "Calculating equalized value-set";
            for( size_t vi = 0; vi < valueSet->rawSize(); ++vi )
            {
                size_t idx = hist->getIndexForValue( valueSet->getScalarDouble( vi ) );
                double cdfVI = cdf[ idx ];
                newData[ vi ] = valueMin + ( valueScaler * cdfVI / cdfScaler );
            }
        }
        else
        {
            // finally, build the new dataset
            debugLog() << "Calculating value-set";
            size_t maxI = hist->size() - 1;
            for( size_t vi = 0; vi < valueSet->rawSize(); ++vi )
            {
                size_t idx = hist->getIndexForValue( valueSet->getScalarDouble( vi ) );
                double idxScaled = ( static_cast< double >( idx )/ static_cast< double >( maxI ) );
                newData[ vi ] = valueMin + ( valueScaler * idxScaled );
            }
        }
        ++*progress;

        // update output with a new dataset, reuse grid
        debugLog() << "Updating output";

        // construct
        boost::shared_ptr< WDataSetScalar > d = boost::shared_ptr< WDataSetScalar >(
            new WDataSetScalar( boost::shared_ptr< WValueSetBase >(
                                    new WValueSet< double >( 0,
                                                             1,
                                                             boost::shared_ptr< std::vector< double > >(
                                                                 new std::vector< double >( newData ) ),
                                                             W_DT_DOUBLE ) ), dataSet->getGrid() )
        );
        m_output->updateData( d );

        debugLog() << "Done";

        progress->finish();
        m_progress->removeSubProgress( progress );
    }
}

