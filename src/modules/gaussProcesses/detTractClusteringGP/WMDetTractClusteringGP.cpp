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

#include "../../../kernel/WKernel.h"
#include "../../emptyIcon.xpm" // Please put a real icon here.
#include "WMDetTractClusteringGP.h"

WMDetTractClusteringGP::WMDetTractClusteringGP():
    WModule()
{
}

WMDetTractClusteringGP::~WMDetTractClusteringGP()
{
}

boost::shared_ptr< WModule > WMDetTractClusteringGP::factory() const
{
    return boost::shared_ptr< WModule >( new WMDetTractClusteringGP() );
}

const char** WMDetTractClusteringGP::getXPMIcon() const
{
    return emptyIcon_xpm; // Please put a real icon here.
}
const std::string WMDetTractClusteringGP::getName() const
{
    return "Deterministic Tract Clustering using Gaussian Proccesses";
}

const std::string WMDetTractClusteringGP::getDescription() const
{
    return "Clusters Gaussian Processes which represents deterministic tracts with the Wassermann "
           "approach described in the paper: http://dx.doi.org/10.1016/j.neuroimage.2010.01.004";
}

void WMDetTractClusteringGP::connectors()
{
    m_gpIC = WModuleInputData< WDataSetGP >::createAndAdd( shared_from_this(), "gpInput", "WDataSetGP providing the gaussian processes" );

    WModule::connectors();
}

void WMDetTractClusteringGP::properties()
{
    WModule::properties();
}

void WMDetTractClusteringGP::moduleMain()
{
    m_moduleState.setResetable( true, true ); // remember actions when actually not waiting for actions
    m_moduleState.add( m_gpIC->getDataChangedCondition() );

    ready();

    while ( !m_shutdownFlag() ) // loop until the module container requests the module to quit
    {
        debugLog() << "Waiting..";
        m_moduleState.wait();
        if ( !m_gpIC->getData().get() ) // ok, the output has not yet sent data
        {
            continue;
        }

        boost::shared_ptr< WDataSetGP > dataSet = m_gpIC->getData();
        if( !dataSet )
        {
            debugLog() << "Invalid data--> continue";
            continue;
        }
        if( m_gpIC->handledUpdate() )
        {
            debugLog() << "Input has been updated...";
        }
        debugLog() << "Start Clustering";

        m_maxSegmentLength = searchGlobalMaxSegementLength( dataSet );
        computeDistanceMatrix( dataSet );
    }
}

double WMDetTractClusteringGP::searchGlobalMaxSegementLength( boost::shared_ptr< const WDataSetGP > dataSet ) const
{
    WDataSetGP::const_iterator cit = std::max_element( dataSet->begin(), dataSet->end(),
        boost::bind( &WGaussProcess::getMaxSegmentLength, _1 ) < boost::bind( &WGaussProcess::getMaxSegmentLength, _2 ) );

    return cit->getMaxSegmentLength();
    // NOLINT return std::max_element( dataSet->begin(), dataSet->end(), [](WGaussProcess p1, WGaussProcess p2){ return p1.getMaxSegmentLength() < p2.getMaxSegmentLength(); } )->getMaxSegmentLength();
}

void WMDetTractClusteringGP::computeDistanceMatrix( boost::shared_ptr< const WDataSetGP > dataSet )
{
    m_similarities = WMatrixSym( dataSet->size() );
}
