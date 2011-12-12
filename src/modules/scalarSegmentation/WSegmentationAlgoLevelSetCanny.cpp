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

#include "WSegmentationAlgoLevelSetCanny.h"

#ifdef OW_USE_ITK

WSegmentationAlgoLevelSetCanny::WSegmentationAlgoLevelSetCanny()
    : WSegmentationAlgo()
{
}

WSegmentationAlgoLevelSetCanny::~WSegmentationAlgoLevelSetCanny()
{
}

void WSegmentationAlgoLevelSetCanny::properties()
{
    m_smoothingIter = m_properties->addProperty( "Smoothing iterations", "Number of smoothing passes.", 10, m_propCondition );
    m_conductance = m_properties->addProperty( "Conductance", "Conductance parameter for smoothing.", 1.0, m_propCondition );
    m_conductance->setMax( 10.0 );
    m_level = m_properties->addProperty( "Level", "The isovalue.", 0.0, m_propCondition );
    m_variance = m_properties->addProperty( "Variance", "variance for the level set.", 1.0, m_propCondition );
    m_threshold = m_properties->addProperty( "Threshold", "Threshold in %.", 0.0, m_propCondition );
}

std::string WSegmentationAlgoLevelSetCanny::getName()
{
    return "Levelset segmentation (Canny)";
}

std::string WSegmentationAlgoLevelSetCanny::getDescription()
{
    return "Use canny levelsets for segmentation.";
}

bool WSegmentationAlgoLevelSetCanny::propChanged()
{
    return m_smoothingIter->changed() || m_conductance->changed() || m_level->changed() || m_variance->changed();
}

WSegmentationAlgo::DataSetPtr WSegmentationAlgoLevelSetCanny::applyOperation()
{
    return m_dataSet->getValueSet()->applyFunction( *this );
}

#endif  // OW_USE_ITK
