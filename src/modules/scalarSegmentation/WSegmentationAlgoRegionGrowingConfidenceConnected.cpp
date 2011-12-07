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

#include "WSegmentationAlgoRegionGrowingConfidenceConnected.h"

#ifdef OW_USE_ITK

WSegmentationAlgoRegionGrowingConfidenceConnected::WSegmentationAlgoRegionGrowingConfidenceConnected()
    : WSegmentationAlgo()
{
}

WSegmentationAlgoRegionGrowingConfidenceConnected::~WSegmentationAlgoRegionGrowingConfidenceConnected()
{
}

void WSegmentationAlgoRegionGrowingConfidenceConnected::properties()
{
    m_smoothingIter = m_properties->addProperty( "Smoothing iterations", "Number of smoothing passes.", 10, m_propCondition );
    m_conductance = m_properties->addProperty( "Conductance", "Conductance parameter for smoothing.", 1.0, m_propCondition );
    m_conductance->setMax( 10.0 );
    m_regionGrowingIterations = m_properties->addProperty( "Iterations", "Number of region growing iterations.", 10, m_propCondition );
    m_neighborhoodRadius = m_properties->addProperty( "Neighborhood radius", "Initial neighborhood radius.", 2, m_propCondition );
    m_neighborhoodRadius->setMin( 1 );
    m_neighborhoodRadius->setMax( 35 );
    m_multiplier = m_properties->addProperty( "Variance multiplier", "Variance multiplier.", 2.5, m_propCondition );
    m_multiplier->setMax( 20.0 );
}

std::string WSegmentationAlgoRegionGrowingConfidenceConnected::getName()
{
    return "Confidence connected region growing";
}

std::string WSegmentationAlgoRegionGrowingConfidenceConnected::getDescription()
{
    return "Confidence connected region growing";
}

bool WSegmentationAlgoRegionGrowingConfidenceConnected::propChanged()
{
    return m_smoothingIter->changed() || m_conductance->changed()
        || m_regionGrowingIterations->changed() || m_neighborhoodRadius->changed()
        || m_multiplier->changed();
}

WSegmentationAlgo::DataSetPtr WSegmentationAlgoRegionGrowingConfidenceConnected::applyOperation()
{
    return m_dataSet->getValueSet()->applyFunction( *this );
}

#endif  // OW_USE_ITK
