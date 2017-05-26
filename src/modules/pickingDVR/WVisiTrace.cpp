//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2017 OpenWalnut Community
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

#include "core/common/math/linearAlgebra/WPosition.h"
#include "core/common/WAssert.h"

#include "WVisiTrace.h"

WVisiTrace::WVisiTrace():
    m_candidatePositions(),
    m_candidateJumps(),
    m_curve3D(),
    m_dataChanged( false )
{
}

std::vector<WPosition> WVisiTrace::getLine()
{
    if( m_dataChanged )
    {
        performVisiTrace();
        m_dataChanged = false;
    }

    return m_curve3D;
}

void WVisiTrace::addCandidatesForRay( const std::vector< WPosition >& positions,
                                      const std::vector< double >& opacityJumps )
{
    WAssert( positions.size() == opacityJumps.size(),
             "Each cndidate position needs to have an opacity jump value." );

    m_candidatePositions.push_back( positions );
    m_candidateJumps.push_back( opacityJumps );

    m_dataChanged = true;
}

void WVisiTrace::performVisiTrace()
{
    assert( false && "Not yet implemented" );
}

void WVisiTrace::reset()
{
    m_candidatePositions.clear();
    m_candidateJumps.clear();
    m_dataChanged = true;
    m_curve3D.clear(); // not really needed because m_dataChanged is true.
}
