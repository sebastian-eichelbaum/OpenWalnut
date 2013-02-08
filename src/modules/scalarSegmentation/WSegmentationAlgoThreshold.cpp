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

WSegmentationAlgoThreshold::WSegmentationAlgoThreshold( ThresholdType type )
    : WSegmentationAlgo(),
      m_type( type )
{
}

WSegmentationAlgoThreshold::~WSegmentationAlgoThreshold()
{
}

void WSegmentationAlgoThreshold::properties()
{
    if( m_type == LOWER_THRESHOLD )
    {
        m_threshold = m_properties->addProperty( "Lower Threshold", "Threshold in %.", 0.0, m_propCondition );
    }
    else
    {
        m_threshold = m_properties->addProperty( "Upper Threshold", "Threshold in %.", 0.0, m_propCondition );
    }

    m_threshold->setMin( 0.0 );
    m_threshold->setMax( 1.0 );
}

std::string WSegmentationAlgoThreshold::getName()
{
    if( m_type == LOWER_THRESHOLD )
    {
        return "Lower Threshold segmentation";
    }
    else
    {
        return "Upper Threshold segmentation";
    }
}

std::string WSegmentationAlgoThreshold::getDescription()
{
    if( m_type == LOWER_THRESHOLD )
    {
        return "Use lower thresholding for segmentation.";
    }
    else
    {
        return "Use upper thresholding for segmentation.";
    }
}

bool WSegmentationAlgoThreshold::propChanged()
{
    return m_threshold->changed();
}

WSegmentationAlgo::DataSetPtr WSegmentationAlgoThreshold::applyOperation()
{
    return m_dataSet->getValueSet()->applyFunction( *this );
}
