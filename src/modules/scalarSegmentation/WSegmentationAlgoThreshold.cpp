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

#include "WSegmentationAlgoThreshold.h"

WSegmentationAlgoThreshold::WSegmentationAlgoThreshold()
    : WSegmentationAlgo()
{
}

WSegmentationAlgoThreshold::~WSegmentationAlgoThreshold()
{
}

void WSegmentationAlgoThreshold::properties()
{
    m_low_threshold = m_properties->addProperty( "Lower Threshold", "Threshold in %.", 0.0, m_propCondition );
    m_upp_threshold = m_properties->addProperty( "Upper Threshold", "Threshold in %.", 1.0, m_propCondition );

    m_low_threshold->setMin( 0.0 );
    m_low_threshold->setMax( 1.0 );
    m_upp_threshold->setMin( 0.0 );
    m_upp_threshold->setMax( 1.0 );

    m_binarize = m_properties->addProperty( "Binarize", "Replace values with 0 and 1. 0 Means outside, 1 inside.", true, m_propCondition );
}

std::string WSegmentationAlgoThreshold::getName()
{
    return "Threshold segmentation";
}

std::string WSegmentationAlgoThreshold::getDescription()
{
    return "Use thresholding for segmentation.";
}

bool WSegmentationAlgoThreshold::propChanged()
{
    return m_low_threshold->changed() || m_upp_threshold->changed() || m_binarize->changed();
}

WSegmentationAlgo::DataSetPtr WSegmentationAlgoThreshold::applyOperation()
{
    return m_dataSet->getValueSet()->applyFunction( *this );
}
